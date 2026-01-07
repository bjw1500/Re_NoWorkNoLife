// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/Managers/NoWorkInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

// Ensure generated code is compiled in the same TU to avoid linker mismatches
//#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkInventoryManagerComponent)

UNoWorkItemInstance* FNoWorkInventoryEntry::Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity)
{
	check(InItemTemplateID > 0 && InItemCount > 0 && InItemRarity != EItemRarity::Count);
	
	UNoWorkItemInstance* NewItemInstance = NewObject<UNoWorkItemInstance>();
	NewItemInstance->Init(InItemTemplateID, InItemRarity);
	Init(NewItemInstance, InItemCount);
	
	return NewItemInstance;
}

void FNoWorkInventoryEntry::Init(UNoWorkItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);
	
	ItemInstance = InItemInstance;
	
	const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);
}

UNoWorkItemInstance* FNoWorkInventoryEntry::Reset()
{
	UNoWorkItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;
	
	return RemovedItemInstance;
}

bool FNoWorkInventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FNoWorkInventoryEntry, FNoWorkInventoryList>(Entries, DeltaParams, *this);
}

void FNoWorkInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	// 클라이언트 측: FastArray(Entries)에서 '새 항목이 복제되어 추가됨' 이벤트 처리.
	// - 서버가 특정 슬롯에 아이템을 채웠다면, 그 변화가 클라이언트로 도달했을 때
	//   UI나 게임플레이 시스템이 반응할 수 있도록 브로드캐스트를 발생시킨다.
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	for (int32 AddedIndex : AddedIndices)
	{
		FNoWorkInventoryEntry& Entry = Entries[AddedIndex];
		if (Entry.ItemInstance)
		{
			const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
			BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
		}
	}
}

void FNoWorkInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 클라이언트 측: FastArray(Entries)에서 '기존 항목이 변경됨' 이벤트 처리.
	// - ChangedIndices 중 ItemInstance가 nullptr이면 해당 슬롯이 비워졌음을 의미한다.
	// - 비워진 슬롯은 즉시 알리고, 이후 추가/수정된 슬롯을 한 번 더 알린다.
	TArray<int32> AddedIndices;
	AddedIndices.Reserve(FinalSize);

	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	
	for (int32 ChangedIndex : ChangedIndices)
	{
		FNoWorkInventoryEntry& Entry = Entries[ChangedIndex];
		if (Entry.ItemInstance)
		{
			AddedIndices.Add(ChangedIndex);
		}
		else
		{
			const FIntPoint ItemSlotPos = FIntPoint(ChangedIndex % InventorySlotCount.X, ChangedIndex / InventorySlotCount.X);
			BroadcastChangedMessage(ItemSlotPos, nullptr, 0);
		}
	}

	for (int32 AddedIndex : AddedIndices)
	{
		FNoWorkInventoryEntry& Entry = Entries[AddedIndex];
		const FIntPoint ItemSlotPos = FIntPoint(AddedIndex % InventorySlotCount.X, AddedIndex / InventorySlotCount.X);
		BroadcastChangedMessage(ItemSlotPos, Entry.ItemInstance, Entry.ItemCount);
	}
}

void FNoWorkInventoryList::BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UNoWorkItemInstance* ItemInstance,
	int32 ItemCount)
{
	// 인벤토리 슬롯 변경 사실을 외부(예: UI)에 알리는 공통 루틴.
	// - ItemSlotPos: 변경된 슬롯 위치
	// - ItemInstance: 슬롯에 들어있는 아이템(없으면 nullptr)
	// - ItemCount: 해당 슬롯의 현재 수량(없으면 0)
	if (InventoryManager->OnInventoryEntryChanged.IsBound())
	{
		InventoryManager->OnInventoryEntryChanged.Broadcast(ItemSlotPos, ItemInstance, ItemCount);
	}
}

UNoWorkInventoryManagerComponent::UNoWorkInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) , InventoryList(this)
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void UNoWorkInventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FNoWorkInventoryEntry>& Entries = InventoryList.Entries;
		Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
	
		for (FNoWorkInventoryEntry& Entry : Entries)
		{
			InventoryList.MarkItemDirty(Entry);
		}

		SlotChecks.SetNumZeroed(InventorySlotCount.X * InventorySlotCount.Y);
	}
}

void UNoWorkInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
	DOREPLIFETIME(ThisClass, SlotChecks);
}

bool UNoWorkInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	// 서브오브젝트(UD1ItemInstance) 복제의 레거시 경로.
	// 목적: 인벤토리의 각 아이템 인스턴스를 개별 UObject로서 클라이언트에 동기화.
	// 동작: 상위 처리 후, 모든 엔트리의 ItemInstance를 순회하며 유효한 것만 복제 요청.
	// 반환: 이번 틱에 하나라도 기록했다면 true.
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (FNoWorkInventoryEntry& Entry : InventoryList.Entries)
	{
		UNoWorkItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}
	
	return bWroteSomething;
}

void UNoWorkInventoryManagerComponent::ReadyForReplication()
{
	// 복제 준비 완료 시점의 훅.
	// Iris의 등록식(SubObject Registration) 경로를 사용하는 경우,
	// 현재 보유 중인 모든 UD1ItemInstance를 등록하여 이후 자동 복제를 활성화한다.
	// 주의: 이후 동적 추가/삭제는 TryAdd/Remove 및 *_Unsafe 경로에서 Add/RemoveReplicatedSubObject로 유지.
	Super::ReadyForReplication();
	
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FNoWorkInventoryEntry& Entry : InventoryList.Entries)
		{
			UNoWorkItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

bool UNoWorkInventoryManagerComponent::IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos,
                                               const FIntPoint& ItemSlotCount) const
{
	// 주어진 그리드(슬롯 테이블)에서 특정 영역이 완전히 비어있는지 검사.
	// 영역이 인벤토리 범위를 벗어나면 배치 불가로 보고 false.
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return false;

	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return false;

	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;

	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index) == false || InSlotChecks[Index])
				return false;
		}
	}
	return true;
}

bool UNoWorkInventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	return IsEmpty(SlotChecks, ItemSlotPos, ItemSlotCount);
}

bool UNoWorkInventoryManagerComponent::IsAllEmpty()
{
	// 인벤토리 전체가 비어있는지 여부.
	for (FNoWorkInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.GetItemInstance())
			return false;
	}
	return true;
}

UNoWorkItemInstance* UNoWorkInventoryManagerComponent::GetItemInstance(const FIntPoint& ItemSlotPos) const
{
	// 지정 슬롯의 아이템 인스턴스를 반환(nullptr이면 빈 슬롯).
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return nullptr;
	
	const TArray<FNoWorkInventoryEntry>& Entries = InventoryList.GetAllEntries();
	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FNoWorkInventoryEntry& Entry = Entries[EntryIndex];
	
	return Entry.GetItemInstance();
}



