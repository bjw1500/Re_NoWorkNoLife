#include "NoWorkGameplayAbility_Interact_Chest.h"

#include "CommonActivatableWidget.h"
#include "LyraGameplayTags.h"
#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Player/LyraPlayerController.h"

#include "NoWorkNoLife/Actors/NoWorkChestBase.h"
#include "NoWorkNoLife/UI/NoWorkEntryWidget.h"
#include "NoWorkNoLife/UI/Inventory/NoWorkInventorySlotsWidget.h"

#include "NoWorkNoLife/Item/Managers/NoWorkInventoryManagerComponent.h"
#include "NoWorkNoLife/Item/Managers/NoWorkItemManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkGameplayAbility_Interact_Chest)

UNoWorkGameplayAbility_Interact_Chest::UNoWorkGameplayAbility_Interact_Chest(
	const FObjectInitializer& ObjectInitializer)
{
}

// 유효성 검사 → 상자 열기 → 서버는 접근 허용, 로컬은 루팅 UI 푸시
void UNoWorkGameplayAbility_Interact_Chest::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ANoWorkChestBase* ChestActor = Cast<ANoWorkChestBase>(InteractableActor);
	if (ChestActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (ChestActor->GetChestState() == EChestState::Open)
	{
		ChestActor->SetChestState(EChestState::Close);
	}
	else
	{
		ChestActor->SetChestState(EChestState::Open);
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		UNoWorkItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UNoWorkItemManagerComponent>();
		UNoWorkInventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UNoWorkInventoryManagerComponent>();
		MyItemManager->AddAllowedComponent(OtherInventoryManager);
	}

	//상자와 상호작용시 해당 플레이어에게 상자의 Inventory를 볼 수 있는 UI를 열어준다.
	if (IsLocallyControlled())
	{
		if (UAsyncAction_PushContentToLayerForPlayer* PushWidgetAction =
			UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(
				GetLyraPlayerControllerFromActorInfo(),
				WidgetClass,
				WidgetLayerTag,
				true))
		{
			PushWidgetAction->AfterPush.AddDynamic(this, &ThisClass::OnAfterPushWidget);
			PushWidgetAction->Activate();
		}
	}
}

void UNoWorkGameplayAbility_Interact_Chest::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UNoWorkItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UNoWorkItemManagerComponent>())
		{
			UNoWorkInventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UNoWorkInventoryManagerComponent>();
			MyItemManager->RemoveAllowedComponent(OtherInventoryManager);
		}
	}
	
	if (IsLocallyControlled() && PushedWidget)
	{
		PushedWidget->DeactivateWidget();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UNoWorkGameplayAbility_Interact_Chest::OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget)
{
	PushedWidget = InPushedWidget;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FInventoryInitializeMessage OtherInventoryInitMessage;
	OtherInventoryInitMessage.InventoryManager = InteractableActor->GetComponentByClass<UNoWorkInventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(LyraGameplayTags::Message_Initialize_OtherInventory, OtherInventoryInitMessage);
	
	InPushedWidget->OnDeactivated().AddLambda([this]()
	{	
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	});
}
