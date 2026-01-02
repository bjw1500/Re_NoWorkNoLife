#include "NoWorkGameplayAbility_Interact_Chest.h"

#include "NoWorkNoLife/Actors/NoWorkChestBase.h"

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
	
	// if (HasAuthority(&CurrentActivationInfo))
	// {
	// 	UD1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UD1ItemManagerComponent>();
	// 	UD1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UD1InventoryManagerComponent>();
	// 	MyItemManager->AddAllowedComponent(OtherInventoryManager);
	// }
	//
	// if (IsLocallyControlled())
	// {
	// 	if (UAsyncAction_PushContentToLayerForPlayer* PushWidgetAction = UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(GetLyraPlayerControllerFromActorInfo(), WidgetClass, WidgetLayerTag, true))
	// 	{
	// 		PushWidgetAction->AfterPush.AddDynamic(this, &ThisClass::OnAfterPushWidget);
	// 		PushWidgetAction->Activate();
	// 	}
	// }
}

void UNoWorkGameplayAbility_Interact_Chest::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	// if (HasAuthority(&CurrentActivationInfo))
	// {
	// 	if (UD1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UD1ItemManagerComponent>())
	// 	{
	// 		UD1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UD1InventoryManagerComponent>();
	// 		MyItemManager->RemoveAllowedComponent(OtherInventoryManager);
	// 	}
	// }
	//
	// if (IsLocallyControlled() && PushedWidget)
	// {
	// 	PushedWidget->DeactivateWidget();
	// }
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UNoWorkGameplayAbility_Interact_Chest::OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget)
{
}
