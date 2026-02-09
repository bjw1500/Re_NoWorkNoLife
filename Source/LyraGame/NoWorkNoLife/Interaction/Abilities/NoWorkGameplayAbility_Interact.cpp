#include "NoWorkGameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "LyraGameplayTags.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PawnMovementComponent.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionInfo.h"
#include "Physics/LyraCollisionChannels.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"

#include "NoWorkNoLife/Abilities/Tasks/NoWorkAbilityTask_WaitInputStart.h"
#include "Tasks/NoWorkAbilityTask_GrantNearbyInteraction.h"
#include "Tasks/NoWorkAbilityTask_WaitForInteractableTraceHit.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkGameplayAbility_Interact)

// 상시 활성화되어 상호작용 후보 탐색/표시 및 입력 관리를 담당
UNoWorkGameplayAbility_Interact::UNoWorkGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

// 트레이스 스캔 태스크 시작 + 서버에서 주변 능력 부여 태스크 시작 + 입력 대기 시작
void UNoWorkGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FNoWorkInteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = GetControllerFromActorInfo();
	
    // 카메라 조준선 트레이스로 상호작용 후보를 주기적으로 갱신
    if (UNoWorkAbilityTask_WaitForInteractableTraceHit* TraceHitTask = UNoWorkAbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(this, InteractionQuery, NoWork_TraceChannel_Interaction, MakeTargetLocationInfoFromOwnerActor(), InteractionTraceRange, InteractionTraceRate, bShowTraceDebug))
	{
		TraceHitTask->InteractableChanged.AddDynamic(this, &ThisClass::UpdateInteractions);
		TraceHitTask->ReadyForActivation();
	}

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
    // 서버 권한일 때 근처 인터랙션 능력들을 동적 부여/회수
    if (AbilitySystem && AbilitySystem->GetOwnerRole() == ROLE_Authority)
	{
		UNoWorkAbilityTask_GrantNearbyInteraction* GrantAbilityTask = UNoWorkAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractables(this, InteractionScanRange, InteractionScanRate);
		GrantAbilityTask->ReadyForActivation();
	}
	
	WaitInputStart();
}

// 후보가 바뀌면 UI에 알리고 하이라이트 상태를 최신화
void UNoWorkGameplayAbility_Interact::UpdateInteractions(const TArray<FNoWorkInteractionInfo>& InteractionInfos)
{
	FNoWorkInteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(LyraGameplayTags::Status_Interact) == false);
	Message.InteractionInfo = InteractionInfos.Num() > 0 ? InteractionInfos[0] : FNoWorkInteractionInfo();

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetAvatarActorFromActorInfo());
	MessageSystem.BroadcastMessage(LyraGameplayTags::Message_Interaction_Notice, Message);

	CurrentInteractionInfos = InteractionInfos;
}

// 현재 첫 후보 대상으로 진행 능력(Ability_Interact_Active) 이벤트 송신
void UNoWorkGameplayAbility_Interact::TriggerInteraction()
{
	if (CurrentInteractionInfos.Num() == 0)
		return;
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetAvatarActorFromActorInfo());
	if (LyraCharacter && LyraCharacter->GetMovementComponent()->IsFalling())
		return;
	
	if (GetAbilitySystemComponentFromActorInfo())
	{
		const FNoWorkInteractionInfo& InteractionInfo = CurrentInteractionInfos[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = nullptr;

		if (UObject* Object = InteractionInfo.Interactable.GetObject())
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				InteractableActor = Actor;
			}
			else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				InteractableActor = ActorComponent->GetOwner();
			}
		}
		
		FGameplayEventData Payload;
		Payload.EventTag = LyraGameplayTags::Ability_Interact_Active;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;
		
		SendGameplayEvent(LyraGameplayTags::Ability_Interact_Active, Payload);
	}
}

void UNoWorkGameplayAbility_Interact::WaitInputStart()
{
	if (UNoWorkAbilityTask_WaitInputStart* InputStartTask = UNoWorkAbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UNoWorkGameplayAbility_Interact::OnInputStart()
{
	if (bInteractionInputHeld)
	{
		return;
	}

	bInteractionInputHeld = true;
	TriggerInteraction();
	WaitInputRelease();
}

void UNoWorkGameplayAbility_Interact::WaitInputRelease()
{
	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
}

void UNoWorkGameplayAbility_Interact::OnInputReleased(float TimeHeld)
{
	bInteractionInputHeld = false;
	WaitInputStart();
}
