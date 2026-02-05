#include "NoWorkGameplayAbility_Interact_Chest.h"

#include "CommonActivatableWidget.h"
#include "LyraGameplayTags.h"
#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "Character/LyraCharacter.h"
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

	//TODO
	//열린 상자를 닫을 때 인벤토리 UI 열리는 걸 어떻게 해보자.
	//상자가 열린 상태에서 짧게 누르면 내용물을 확인 할 수 있고, 길게 누르면 인벤토리 열리지 않고 닫히게 해보자..

	
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

		if (UNoWorkItemManagerComponent* OtherItemManager = InteractableActor->GetComponentByClass<UNoWorkItemManagerComponent>())
		{
			UNoWorkInventoryManagerComponent* MyInventoryManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UNoWorkInventoryManagerComponent>();
			OtherItemManager->RemoveAllowedComponent(MyInventoryManager);
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

	//상자의 인벤토리 정보를 업데이트하게 브로드캐스트
	FInventoryInitializeMessage OtherInventoryInitMessage;
	OtherInventoryInitMessage.InventoryManager = InteractableActor->GetComponentByClass<UNoWorkInventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(LyraGameplayTags::Message_Initialize_OtherInventory, OtherInventoryInitMessage);

	//플레이어의 인벤토리 정보를 업데이트하게 브로드캐스트
	FInventoryInitializeMessage MyInventoryInitMessage;
	MyInventoryInitMessage.InventoryManager = GetLyraCharacterFromActorInfo()->GetComponentByClass<UNoWorkInventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(LyraGameplayTags::Message_Initialize_MyInventory, MyInventoryInitMessage);
	
	InPushedWidget->OnDeactivated().AddLambda([this]()
	{	
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	});
}
