#include "NoWorkGameplayAbility_Interact_Object.h"

#include "LyraGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractable.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionQuery.h"
#include "NoWorkNoLife/Interaction/NoWorkWorldInteractable.h"
#include "Tasks/NoWorkAbilityTask_WaitForInteractableTraceHit.h"
#include "Tasks/NoWorkAbilityTask_WaitForInvalidInteraction.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkGameplayAbility_Interact_Object)

UNoWorkGameplayAbility_Interact_Object::UNoWorkGameplayAbility_Interact_Object(
	const FObjectInitializer& ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(LyraGameplayTags::Ability_Interact_Object);
	ActivationOwnedTags.AddTag(LyraGameplayTags::Status_Interact);
}

void UNoWorkGameplayAbility_Interact_Object::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (InitializeAbility(TargetActor) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	bInitialized = false;
	FNoWorkInteractionQuery Query;
	Query.RequestingAvatar = GetAvatarActorFromActorInfo();
	Query.RequestingController = GetControllerFromActorInfo();

	if (Interactable->CanInteraction(Query) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bInitialized = true;

	if (ANoWorkWorldInteractable* WorldInteractable = Cast<ANoWorkWorldInteractable>(TargetActor))
	{
		WorldInteractable->OnInteractionSuccess(GetAvatarActorFromActorInfo());
	}

	if (UNoWorkAbilityTask_WaitForInvalidInteraction* InvalidInteractionTask = UNoWorkAbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidInteractionTask->OnInvalidInteraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidInteractionTask->ReadyForActivation();
	}

	if (UAnimMontage* ActiveEndMontage = InteractionInfo.ActiveEndMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveEndMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}
}

void UNoWorkGameplayAbility_Interact_Object::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UNoWorkGameplayAbility_Interact_Object::OnInvalidInteraction()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
