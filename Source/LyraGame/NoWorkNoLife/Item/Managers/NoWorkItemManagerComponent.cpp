// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/Managers/NoWorkItemManagerComponent.h"

#include "NoWorkInventoryManagerComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/LyraAssetManager.h"

UNoWorkItemManagerComponent::UNoWorkItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UNoWorkItemManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
		return;

	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

		if (APawn* ControlledPawn = Controller->GetPawn())
		{
			OnPossessedPawnChanged(nullptr, ControlledPawn);
		}
	}
}

void UNoWorkItemManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (AController* Controller = Cast<AController>(GetOwner()))
		{
			Controller->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = Controller->GetPawn())
			{
				if (UNoWorkInventoryManagerComponent* Inventory = ControlledPawn->GetComponentByClass<UNoWorkInventoryManagerComponent>())
				{
					RemoveAllowedComponent(Inventory);
				}
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}


void UNoWorkItemManagerComponent::Server_InventoryToInventory_Implementation(
	UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos,
	UNoWorkInventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (HasAuthority() == false)
		return;

	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false ||  IsAllowedComponent(ToInventoryManager) == false)
		return;

	if (FromInventoryManager == ToInventoryManager && FromItemSlotPos == ToItemSlotPos)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableCount > 0)
	{
		UNoWorkItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
	}
}

void UNoWorkItemManagerComponent::Server_QuickFromInventory_Implementation(
	UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (HasAuthority() == false)
		return;
	
	if (FromInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(FromInventoryManager) == false)
		return;
	
	UNoWorkInventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	if (MyInventoryManager == nullptr)
		return;

	if (IsAllowedComponent(MyInventoryManager) == false)
		return;

	UNoWorkItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;


	{
		// 2. [일반 아이템]
		// 2-1. [내 인벤토리] -> X
		// 2-2. [다른 인벤토리] -> 내 인벤토리
		if (MyInventoryManager != FromInventoryManager)
		{
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;
			
			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, ToItemSlotPoses, ToItemCounts);
			if (MovableItemCount > 0)
			{
				UNoWorkItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
		}
	}
}

// void UNoWorkItemManagerComponent::Server_DropItemFromInventory_Implementation(
// 	UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
// {
// 	if (HasAuthority() == false)
// 		return;
//
// 	if (FromInventoryManager == nullptr)
// 		return;
//
// 	if (IsAllowedComponent(FromInventoryManager) == false)
// 		return;
//
// 	UNoWorkItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
// 	if (FromItemInstance == nullptr)
// 		return;
//
// 	int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
// 	if (FromItemCount <= 0)
// 		return;
// 	
// 	if (TryDropItem(FromItemInstance, FromItemCount))
// 	{
// 		FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
// 	}
// }

// bool UNoWorkItemManagerComponent::TryDropItem(UNoWorkItemInstance* FromItemInstance, int32 FromItemCount)
// {
// 	if (HasAuthority() == false)
// 		return false;
//
// 	if (FromItemInstance == nullptr || FromItemCount <= 0)
// 		return false;
//
// 	AController* Controller = Cast<AController>(GetOwner());
// 	ACharacter* Character = Controller ? Cast<ACharacter>(Controller->GetPawn()) : Cast<ACharacter>(GetOwner());
// 	if (Character == nullptr)
// 		return false;
// 	
// 	const float MaxDistance = 100.f;
// 	const int32 MaxTryCount = 5.f;
// 	float HalfRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius() / 2.f;
// 	float QuarterHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2.f;
// 	TArray<AActor*> ActorsToIgnore = { Character };
//
// 	FActorSpawnParameters SpawnParameters;
// 	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
// 	for (int32 i = 0; i < MaxTryCount; i++)
// 	{
// 		FHitResult HitResult;
// 		FVector2D RandPoint = FMath::RandPointInCircle(MaxDistance);
// 		FVector TraceStartLocation = Character->GetCapsuleComponent()->GetComponentLocation();
// 		FVector TraceEndLocation = TraceStartLocation + FVector(RandPoint.X, RandPoint.Y, 0.f);
// 		
// 		if (UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), TraceStartLocation, TraceEndLocation, HalfRadius, QuarterHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
// 			continue;
// 		
// 		TSubclassOf<ANoWorkPickupableItemBase> PickupableItemBaseClass = ULyraAssetManager::Get().GetSubclassByName<ANoWorkPickupableItemBase>("PickupableItemBaseClass");
// 		ANoWorkPickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<ANoWorkPickupableItemBase>(PickupableItemBaseClass, TraceEndLocation, FRotator::ZeroRotator, SpawnParameters);
// 		if (PickupableItemActor == nullptr)
// 			continue;
// 		
// 		FNoWorkPickupInfo PickupInfo;
// 		PickupInfo.PickupInstance.ItemInstance = FromItemInstance;
// 		PickupInfo.PickupInstance.ItemCount = FromItemCount;
// 		PickupableItemActor->SetPickupInfo(PickupInfo);
// 		return true;
// 	}
// 	
// 	return false;
// }

void UNoWorkItemManagerComponent::AddAllowedComponent(UActorComponent* ActorComponent)
{
	if (ActorComponent == nullptr)
		return;

	AllowedComponents.AddUnique(ActorComponent);
}

void UNoWorkItemManagerComponent::RemoveAllowedComponent(UActorComponent* ActorComponent)
{
	if (ActorComponent == nullptr)
		return;

	AllowedComponents.Remove(ActorComponent);
}

bool UNoWorkItemManagerComponent::IsAllowedComponent(UActorComponent* ActorComponent) const
{
	return AllowedComponents.Contains(ActorComponent);
}

UNoWorkInventoryManagerComponent* UNoWorkItemManagerComponent::GetMyInventoryManager() const
{
	UNoWorkInventoryManagerComponent* MyInventoryManager = nullptr;
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyInventoryManager = Pawn->GetComponentByClass<UNoWorkInventoryManagerComponent>();
		}
	}

	return MyInventoryManager;
}

void UNoWorkItemManagerComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (HasAuthority() == false)
		return;

	if (OldPawn)
	{
		if (UNoWorkInventoryManagerComponent* OldInventory = OldPawn->GetComponentByClass<UNoWorkInventoryManagerComponent>())
		{
			RemoveAllowedComponent(OldInventory);
		}
	}

	if (NewPawn)
	{
		if (UNoWorkInventoryManagerComponent* NewInventory = NewPawn->GetComponentByClass<UNoWorkInventoryManagerComponent>())
		{
			AddAllowedComponent(NewInventory);
		}
	}
}

