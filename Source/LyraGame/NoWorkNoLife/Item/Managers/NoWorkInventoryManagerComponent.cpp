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
	DOREPLIFETIME(ThisClass, InventorySlotCount);
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

int32 UNoWorkInventoryManagerComponent::CanMoveOrMergeItem(UNoWorkInventoryManagerComponent* OtherComponent,
	const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	// 인벤토리 -> 인벤토리 간 이동/합치기 가능 수량 계산.
	// - 위치 유효성, 템플릿/희귀도 일치, 스택 한도, 대상 영역 여유 등을 검사.
	// 반환: 실제 이동 가능한 개수(0이면 불가).
	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;
	
	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;
	
	const UNoWorkItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);
	
	if (this == OtherComponent && FromItemSlotPos == ToItemSlotPos)
		return FromItemCount;
	
	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return 0;
	
	const UNoWorkItemInstance* ToItemInstance = GetItemInstance(ToItemSlotPos);
	const int32 ToItemCount = GetItemCount(ToItemSlotPos);
	
	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UNoWorkItemTemplate& FromItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(FromTemplateID);
	
	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		if (FromItemInstance->GetItemRarity() != ToItemInstance->GetItemRarity())
			return 0;
		
		if (FromItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, FromItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		//내 인벤토리에서 옮기는 경우라면,
		if (this == OtherComponent)
		{
			//시뮬레이션을 위해 복사한 슬롯으로 테스트 한다.
			TArray<bool> TempSlotChecks = SlotChecks;
			MarkSlotChecks(TempSlotChecks, false, FromItemSlotPos, FromItemSlotCount);

			//해당 아이템이 해당 위치에 들어갈 수 있는지 판단한다.
			return IsEmpty(TempSlotChecks, ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
		else
		{
			return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
	}
}

int32 UNoWorkInventoryManagerComponent::CanMoveOrMergeItem_Quick(UNoWorkInventoryManagerComponent* OtherComponent,
	const FIntPoint& FromItemSlotPos, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	// 인벤토리 -> 인벤토리 빠른 이동 경로.
	// 목적: 한 번의 질의로 배치 가능한 후보 슬롯과 각 슬롯에 넣을 수량을 산출.
	// 출력: OutToItemSlotPoses/OutToItemCounts에 병렬로 채움.
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();
	
	if (OtherComponent == nullptr || this == OtherComponent)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;
	
	const UNoWorkItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	if (FromItemInstance == nullptr)
		return 0;
	
	return CanAddItem(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, OutToItemSlotPoses, OutToItemCounts);
}

int32 UNoWorkInventoryManagerComponent::CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount,
                                                   TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	// 주어진 템플릿/희귀도/수량을 인벤토리에 '최대 몇 개'까지 배치할 수 있는지 시뮬레이션.
	// 절차:
	// 1) 기존 동일 스택에 합칠 수 있는 만큼 먼저 채움
	// 2) 남은 분량은 빈 슬롯을 탐색하여 아이템 SlotCount에 맞춰 배치
	// 결과는 OutToItemSlotPoses/OutToItemCounts로 반환.
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();
	
	if (ItemTemplateID <= 0 || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;
	
	const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemTemplateID);
	int32 LeftItemCount = ItemCount;
	
	if (ItemTemplate.MaxStackCount > 1)
	{
		const TArray<FNoWorkInventoryEntry>& ToEntries = GetAllEntries();
		
		for (int32 i = 0; i < ToEntries.Num(); i++)
		{
			const FNoWorkInventoryEntry& ToEntry = ToEntries[i];
			const UNoWorkItemInstance* ToItemInstance = ToEntry.GetItemInstance();
			const int32 ToItemCount = ToEntry.GetItemCount();
			
			if (ToItemInstance == nullptr)
				continue;

			if (ToItemInstance->GetItemTemplateID() != ItemTemplateID)
				continue;

			if (ToItemInstance->GetItemRarity() != ItemRarity)
				continue;
			
			if (int32 AddCount = FMath::Min(ToItemCount + LeftItemCount, ItemTemplate.MaxStackCount) - ToItemCount)
			{
				OutToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
				OutToItemCounts.Emplace(AddCount);
				LeftItemCount -= AddCount;

				if (LeftItemCount == 0)
					return ItemCount;
			}
		}
	}
	
	const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;
	TArray<bool> TempSlotChecks = SlotChecks;
	
	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = InventorySlotCount - ItemSlotCount;
	
	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (IsEmpty(TempSlotChecks, ItemSlotPos, ItemSlotCount))
			{
				MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, ItemSlotCount);
				
				int32 AddCount = FMath::Min(LeftItemCount, ItemTemplate.MaxStackCount);
				OutToItemSlotPoses.Emplace(ItemSlotPos);
				OutToItemCounts.Emplace(AddCount);
				
				LeftItemCount -= AddCount;
				
				if (LeftItemCount == 0)
					return ItemCount;
			}
		}
	}
	
	return ItemCount - LeftItemCount;
}

int32 UNoWorkInventoryManagerComponent::TryAddItemByRarity(TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass,
                                                           EItemRarity ItemRarity, int32 ItemCount)
{
	// 서버 전용: 특정 템플릿/희귀도의 아이템을 실제로 추가.
	// 1) CanAddItem으로 배치 계획 수립
	// 2) 기존 스택 증가 또는 새 UD1ItemInstance 생성/초기화
	// 3) SlotChecks 갱신, FastArray 더티 마킹으로 복제 트리거
	// 4) 등록식 경로 사용 시 AddReplicatedSubObject로 새 인스턴스 등록
	// 반환: 실제 추가된 개수(최대 ItemCount)
	check(GetOwner()->HasAuthority());

	if (ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;
	
	int32 ItemTemplateID = UNoWorkItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemTemplateID);
	
	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;

	int32 AddableItemCount = CanAddItem(ItemTemplateID, ItemRarity, ItemCount, ToItemSlotPoses, ToItemCounts);
	if (AddableItemCount > 0)
	{
		TArray<UNoWorkItemInstance*> AddedItemInstances;
		
		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
		{
			const FIntPoint& ToItemSlotPos = ToItemSlotPoses[i];
			const int32 ToItemCount = ToItemCounts[i];

			const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
			FNoWorkInventoryEntry& ToEntry = InventoryList.Entries[ToIndex];

			if (ToEntry.ItemInstance)
			{
				ToEntry.ItemCount += ToItemCount;
				InventoryList.MarkItemDirty(ToEntry);
			}
			else
			{
				AddedItemInstances.Add(ToEntry.Init(ItemTemplateID, ToItemCount, ItemRarity));
				MarkSlotChecks(true, ToItemSlotPos, ItemTemplate.SlotCount);
				InventoryList.MarkItemDirty(ToEntry);
			}
		}

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			for (UNoWorkItemInstance* AddedItemInstance : AddedItemInstances)
			{
				if (AddedItemInstance)
				{
					AddReplicatedSubObject(AddedItemInstance);
				}
			}
		}
		return AddableItemCount;
	}

	return 0;
}

void UNoWorkInventoryManagerComponent::AddItem_Unsafe(const FIntPoint& ItemSlotPos, UNoWorkItemInstance* ItemInstance,
	int32 ItemCount)
{
	// 서버 전용 내부 유틸: 지정 슬롯에 아이템을 강제 삽입/스택 증가.
	// 전제: 외부에서 유효성(CanAddItem 등)을 보장해야 한다.
	// - 새 삽입 시 SlotChecks 점유 처리, FastArray 더티 마킹, 등록식 경로면 AddReplicatedSubObject 호출.
	check(GetOwner()->HasAuthority());
	
	const int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FNoWorkInventoryEntry& Entry = InventoryList.Entries[Index];
	
	if (Entry.GetItemInstance())
	{
		Entry.ItemCount += ItemCount;
		InventoryList.MarkItemDirty(Entry);
	}
	else
	{
		if (ItemInstance == nullptr)
			return;
		
		const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		
		Entry.Init(ItemInstance, ItemCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
		{
			AddReplicatedSubObject(ItemInstance);
		}

		MarkSlotChecks(true, ItemSlotPos, ItemTemplate.SlotCount);
		InventoryList.MarkItemDirty(Entry);
	}
}

UNoWorkItemInstance* UNoWorkInventoryManagerComponent::RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount)
{
	// 서버 전용 내부 유틸: 지정 슬롯의 수량을 차감하고, 0 이하면 슬롯을 비움.
	// - 슬롯 비울 때 SlotChecks 해제 및(등록식 경로 시) RemoveReplicatedSubObject 호출.
	// - FastArray 더티 마킹으로 복제를 트리거.
	// 반환: 해당 슬롯의 아이템 인스턴스(비우기 전 포인터)
	check(GetOwner()->HasAuthority());
	
	const int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FNoWorkInventoryEntry& Entry = InventoryList.Entries[Index];
	UNoWorkItemInstance* ItemInstance = Entry.GetItemInstance();
	
	Entry.ItemCount -= ItemCount;
	if (Entry.GetItemCount() <= 0)
	{
		const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		MarkSlotChecks(false, ItemSlotPos, ItemTemplate.SlotCount);
		
		UNoWorkItemInstance* RemovedItemInstance = Entry.Reset();
		if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
		{
			RemoveReplicatedSubObject(RemovedItemInstance);
		}
	}
	
	InventoryList.MarkItemDirty(Entry);
	return ItemInstance;
}

void UNoWorkInventoryManagerComponent::MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing,
                                                      const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	// 내부 그리드(슬롯 점유 테이블)에 대해 아이템이 차지하는 직사각형 영역을
	// 사용/미사용(bIsUsing)으로 표시한다.
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return;

	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return;

	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;

	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index))
			{
				InSlotChecks[Index] = bIsUsing;
			}
		}
	}
}

void UNoWorkInventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos,
	const FIntPoint& ItemSlotCount)
{
	// 멤버 SlotChecks에 직접 적용하는 단축 함수.
	MarkSlotChecks(SlotChecks, bIsUsing, ItemSlotPos, ItemSlotCount);
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

int32 UNoWorkInventoryManagerComponent::GetItemCount(const FIntPoint& ItemSlotPos) const
{
	// 지정 슬롯의 아이템 수량(빈 슬롯이면 0)을 반환.
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;
	
	const TArray<FNoWorkInventoryEntry>& Entries = InventoryList.GetAllEntries();
	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FNoWorkInventoryEntry& Entry = Entries[EntryIndex];
	
	return Entry.GetItemCount();
}

const TArray<FNoWorkInventoryEntry>& UNoWorkInventoryManagerComponent::GetAllEntries() const
{
	return InventoryList.GetAllEntries();
}

int32 UNoWorkInventoryManagerComponent::GetTotalCountByID(int32 ItemTemplateID) const
{	// 특정 템플릿 ID의 총 수량을 합산하여 반환.
	int32 TotalCount = 0;
	
	for (const FNoWorkInventoryEntry& Entry : GetAllEntries())
	{
		if (UNoWorkItemInstance* ItemInstance = Entry.ItemInstance)
		{
			if (ItemInstance->GetItemTemplateID() == ItemTemplateID)
			{
				TotalCount += Entry.ItemCount;
			}
		}
	}
	
	return TotalCount;
}

void UNoWorkInventoryManagerComponent::ResetSlot(FIntPoint NewSlotCount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// 1) 기존 아이템 캐싱 (템플릿 ID/레어리티/개수)
		struct FCachedItem
		{
			int32 ItemTemplateID = INDEX_NONE;
			EItemRarity Rarity = EItemRarity::Poor;
			int32 Count = 0;
		};

		TArray<FCachedItem> CachedItems;
		CachedItems.Reserve(InventoryList.Entries.Num());

		for (const FNoWorkInventoryEntry& Entry : InventoryList.Entries)
		{
			if (UNoWorkItemInstance* ItemInstance = Entry.GetItemInstance())
			{
				const int32 Count = Entry.GetItemCount();
				if (Count > 0)
				{
					FCachedItem Cached;
					Cached.ItemTemplateID = ItemInstance->GetItemTemplateID();
					Cached.Rarity = ItemInstance->GetItemRarity();
					Cached.Count = Count;
					CachedItems.Add(Cached);
				}
			}
		}

		// 2) 슬롯 재설정 (비우고 새 사이즈로 구성)
		InventorySlotCount = NewSlotCount;
		
		TArray<FNoWorkInventoryEntry>& Entries = InventoryList.Entries;
		Entries.Empty();
		Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
	
		for (FNoWorkInventoryEntry& Entry : Entries)
		{
			InventoryList.MarkItemDirty(Entry);
		}

		SlotChecks.Empty();
		SlotChecks.SetNumZeroed(InventorySlotCount.X * InventorySlotCount.Y);

		// 3) 캐싱한 아이템 재배치 시도 (수용 불가분은 로그)
		if (CachedItems.Num() > 0)
		{
			for (const FCachedItem& Item : CachedItems)
			{
				const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(Item.ItemTemplateID);
				const TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass = ItemTemplate.GetClass();

				const int32 Added = TryAddItemByRarity(ItemTemplateClass, Item.Rarity, Item.Count);
				if (Added < Item.Count)
				{
					UE_LOG(LogTemp, Warning, TEXT("[InventoryManager] Resize overflow: TemplateID=%d Rarity=%d Count=%d"), Item.ItemTemplateID, static_cast<int32>(Item.Rarity), Item.Count - Added);
				}
			}
		}
	}
}



