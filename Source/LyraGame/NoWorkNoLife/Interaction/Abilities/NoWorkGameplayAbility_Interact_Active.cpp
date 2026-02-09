#include "NoWorkGameplayAbility_Interact_Active.h"

#include "AbilitySystemComponent.h"
#include "NoWorkGameplayAbility_Interact.h"
#include "LyraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractable.h"
#include "NoWorkNoLife/Interaction/NoWorkWorldInteractable.h"
#include "Tasks/NoWorkAbilityTask_WaitForInvalidInteraction.h"
#include "EnhancedInputSubsystems.h"
#include "NoWorkNoLife/Input/NoWorkEnhancedPlayerInput.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkGameplayAbility_Interact_Active)

// 입력 유지/시간/연출/무효화 감시를 통합 관리하는 진행 능력
UNoWorkGameplayAbility_Interact_Active::UNoWorkGameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(LyraGameplayTags::Ability_Interact_Active);
	ActivationOwnedTags.AddTag(LyraGameplayTags::Status_Interact);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = LyraGameplayTags::Ability_Interact_Active;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UNoWorkGameplayAbility_Interact_Active::FlushPressedInput(UInputAction* InputAction)
{
	if (CurrentActorInfo)
	{
		if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
		{
			if (UNoWorkEnhancedPlayerInput* PlayerInput = Cast<UNoWorkEnhancedPlayerInput>(PlayerController->PlayerInput))
			{
				PlayerInput->FlushPressedInput(InputAction);
			}
		}
	}
}

// 즉시형(Duration<=0)이면 즉시 트리거, 유지형이면 이동정지/장비숨김/루프 Cue/몽타주/입력해제/무효화/타이머 설정
void UNoWorkGameplayAbility_Interact_Active::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (InitializeAbility(const_cast<AActor*>(TriggerEventData->Target.Get())) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (ANoWorkWorldInteractable* WorldInteractable = Cast<ANoWorkWorldInteractable>(InteractableActor))
	{
		WorldInteractable->OnInteractActiveStarted(GetLyraCharacterFromActorInfo());
	}

	if (InteractionInfo.Duration <= 0.f)
	{
		TriggerInteraction();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	FlushPressedInput(MoveInputAction);
	
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		if (UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement())
		{
			CharacterMovement->StopMovementImmediately();
		}

		// if (UNoWorkEquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UNoWorkEquipManagerComponent>())
		// {
		// 	EquipManager->ChangeShouldHiddenEquipments(true);
		// }
	}

	FGameplayCueParameters Parameters;
	Parameters.Instigator = InteractableActor;
	K2_AddGameplayCueWithParams(InteractionInfo.ActiveLoopGameplayCueTag, Parameters, true);

	FNoWorkInteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = true;
	Message.InteractionInfo = InteractionInfo;
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(LyraGameplayTags::Message_Interaction_Progress, Message);

	if (UAnimMontage* ActiveStartMontage = InteractionInfo.ActiveStartMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveStartMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}
	
	if (UNoWorkAbilityTask_WaitForInvalidInteraction* InvalidInteractionTask = UNoWorkAbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidInteractionTask->OnInvalidInteraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidInteractionTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}

	bInteractionTriggered = false;
	GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, &ThisClass::OnDurationEnded, InteractionInfo.Duration, false);
}

// 연출 복구 및 UI 알림(Notice) 송신
void UNoWorkGameplayAbility_Interact_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
	{
		if (bWasCancelled)
		{
			// if (UNoWorkEquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UNoWorkEquipManagerComponent>())
			// {
			// 	EquipManager->ChangeShouldHiddenEquipments(false);
			//
			// 	if (EquipManager->GetCurrentEquipState() != EEquipState::Unarmed)
			// 	{
			// 		if (ANoWorkEquipmentBase* EquippedActor = EquipManager->GetFirstEquippedActor())
			// 		{
			// 			if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("EquipMontage"), EquippedActor->GetEquipMontage(), 1.f, NAME_None, false, 1.f, 0.f, false))
			// 			{
			// 				PlayMontageTask->ReadyForActivation();
			// 			}
			// 		}
			// 	}
			// }
		}

		if (ANoWorkWorldInteractable* WorldInteractable = Cast<ANoWorkWorldInteractable>(InteractableActor))
		{
			WorldInteractable->OnInteractActiveEnded(LyraCharacter);
		}
	
		FNoWorkInteractionMessage Message;
		Message.Instigator = LyraCharacter;
		Message.bShouldRefresh = false;
		Message.bSwitchActive = true;
			
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.BroadcastMessage(LyraGameplayTags::Message_Interaction_Notice, Message);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UNoWorkGameplayAbility_Interact_Active::OnInvalidInteraction()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UNoWorkGameplayAbility_Interact_Active::OnInputReleased(float TimeHeld)
{
	if (InteractionInfo.Duration > 0.f && TimeHeld < InteractionInfo.Duration && InteractionInfo.TapAbilityToGrant)
	{
		bInteractionTriggered = TriggerInteractionWithAbility(InteractionInfo.TapAbilityToGrant);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UNoWorkGameplayAbility_Interact_Active::OnDurationEnded()
{
	if (bInteractionTriggered)
	{
		return;
	}
	
	if (UAbilityTask_NetworkSyncPoint* NetSyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait))
	{
		NetSyncTask->OnSync.AddDynamic(this, &ThisClass::OnNetSync);
		NetSyncTask->ReadyForActivation();
	}
}

void UNoWorkGameplayAbility_Interact_Active::OnNetSync()
{
	if (TriggerInteractionWithAbility(InteractionInfo.AbilityToGrant))
	{
		bInteractionTriggered = true;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

// 실제 상호작용 능력(InteractionInfo.AbilityToGrant)을 Ability.Interact 이벤트로 트리거
bool UNoWorkGameplayAbility_Interact_Active::TriggerInteraction()
{
	return TriggerInteractionWithAbility(InteractionInfo.AbilityToGrant);
}

bool UNoWorkGameplayAbility_Interact_Active::TriggerInteractionWithAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return false;
	}

	bool bTriggerSuccessful = false;
	bool bCanActivate = false;

	FGameplayEventData Payload;
	Payload.EventTag = LyraGameplayTags::Ability_Interact;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.Target = InteractableActor;

	Interactable->CustomizeInteractionEventData(LyraGameplayTags::Ability_Interact, Payload);

	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(AbilityClass))
		{
			bCanActivate = AbilitySpec->Ability->CanActivateAbility(AbilitySpec->Handle, AbilitySystem->AbilityActorInfo.Get());
			bTriggerSuccessful = AbilitySystem->TriggerAbilityFromGameplayEvent(
				AbilitySpec->Handle,
				AbilitySystem->AbilityActorInfo.Get(),
				LyraGameplayTags::Ability_Interact,
				&Payload,
				*AbilitySystem
			);
		}
	}

	return bCanActivate || bTriggerSuccessful;

}
