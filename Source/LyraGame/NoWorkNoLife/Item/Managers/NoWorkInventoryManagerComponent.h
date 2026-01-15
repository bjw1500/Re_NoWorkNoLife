// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "NoWorkInventoryManagerComponent.generated.h"


class UNoWorkItemInstance;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UNoWorkItemInstance*, int32/*ItemCount*/);


USTRUCT(BlueprintType)
struct FNoWorkInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	UNoWorkItemInstance* Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity);
	void Init(UNoWorkItemInstance* InItemInstance, int32 InItemCount);
	UNoWorkItemInstance* Reset();
	
public:
	UNoWorkItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }
	
private:
	friend struct FNoWorkInventoryList;
	friend class UNoWorkInventoryManagerComponent;
	
	UPROPERTY(VisibleInstanceOnly, Category="Inventory", meta=(EditInline))
	TObjectPtr<UNoWorkItemInstance> ItemInstance;

	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	int32 ItemCount = 0;
};

USTRUCT(BlueprintType)
struct FNoWorkInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FNoWorkInventoryList() : InventoryManager(nullptr) { }
	FNoWorkInventoryList(UNoWorkInventoryManagerComponent* InOwnerComponent) : InventoryManager(InOwnerComponent) { }

public:
	// FastArray 복제 훅 및 변경 브로드캐스트
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UNoWorkItemInstance* ItemInstance, int32 ItemCount);
	
public:
	const TArray<FNoWorkInventoryEntry>& GetAllEntries() const { return Entries; }
	
private:
	friend class UNoWorkInventoryManagerComponent;
	
	UPROPERTY(VisibleInstanceOnly, Category="Inventory")
	TArray<FNoWorkInventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UNoWorkInventoryManagerComponent> InventoryManager;
};

template<>
struct TStructOpsTypeTraits<FNoWorkInventoryList> : public TStructOpsTypeTraitsBase2<FNoWorkInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


UCLASS( BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LYRAGAME_API UNoWorkInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNoWorkInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;

public:
	// 템플릿/희귀도/수량을 기준으로 추가/제거 가능 분배 계획 계산
	int32 CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	// 희귀도 고정 방식으로 아이템 추가 시도(성공 수량 반환)
	int32 TryAddItemByRarity(TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount);
private:
	void MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
	
public:	

	// 슬롯 점유 확인
	bool IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsAllEmpty();
	
	UNoWorkItemInstance* GetItemInstance(const FIntPoint& ItemSlotPos) const;
	int32 GetItemCount(const FIntPoint& ItemSlotPos) const;
	
	const TArray<FNoWorkInventoryEntry>& GetAllEntries() const;
	int32 GetTotalCountByID(int32 ItemTemplateID) const;
	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
	TArray<bool>& GetSlotChecks() { return SlotChecks; }

	void ResetSlot(FIntPoint NewSlotCount);
	
public:

	FOnInventoryEntryChanged OnInventoryEntryChanged;
	
private:
	UPROPERTY(VisibleInstanceOnly, Replicated, Category="Inventory", meta=(ShowOnlyInnerProperties))
	FNoWorkInventoryList InventoryList;
	
	UPROPERTY(VisibleInstanceOnly, Replicated, Category="Inventory")
	TArray<bool> SlotChecks;

	UPROPERTY(VisibleInstanceOnly, Replicated, Category="Inventory")
	FIntPoint InventorySlotCount = FIntPoint(10, 5);
	
};
