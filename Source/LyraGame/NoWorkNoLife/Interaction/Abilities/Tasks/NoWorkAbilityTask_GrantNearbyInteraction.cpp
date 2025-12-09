#include "NoWorkAbilityTask_GrantNearbyInteraction.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractable.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionQuery.h"
#include "Physics/LyraCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkAbilityTask_GrantNearbyInteraction)

// 일정 주기로 주변 오버랩을 검사하여 Interactable들의 AbilityToGrant를 소유자 ASC에 부여/회수
UNoWorkAbilityTask_GrantNearbyInteraction* UNoWorkAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractables(UGameplayAbility* OwningAbility, float InteractionAbilityScanRange, float InteractionAbilityScanRate)
{
	UNoWorkAbilityTask_GrantNearbyInteraction* Task = NewAbilityTask<UNoWorkAbilityTask_GrantNearbyInteraction>(OwningAbility);
	Task->InteractionAbilityScanRange = InteractionAbilityScanRange;
	Task->InteractionAbilityScanRate = InteractionAbilityScanRate;
	return Task;
}

// 타이머로 주기적 오버랩 스캔 시작
void UNoWorkAbilityTask_GrantNearbyInteraction::Activate()
{
	Super::Activate();
	
	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &UNoWorkAbilityTask_GrantNearbyInteraction::QueryInteractables, InteractionAbilityScanRate, true);
	}
}

void UNoWorkAbilityTask_GrantNearbyInteraction::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

// 주변 Interactable 수집 → InteractionInfo 빌드 → Ability 부여/회수 동기화
void UNoWorkAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	UWorld* World = GetWorld();
	AActor* AvatarActor = GetAvatarActor();
	
	if (World && AvatarActor)
	{
		TSet<FObjectKey> RemoveKeys;
		GrantedInteractionAbilities.GetKeys(RemoveKeys);
		
		FCollisionQueryParams Params(SCENE_QUERY_STAT(UNoWorkAbilityTask_GrantNearbyInteraction), false);

		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByChannel(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, NoWork_TraceChannel_Interaction, FCollisionShape::MakeSphere(InteractionAbilityScanRange), Params);
		
		if (OverlapResults.Num() > 0)
		{
			TArray<TScriptInterface<INoWorkInteractable>> Interactables;
			for (const FOverlapResult& OverlapResult : OverlapResults)
			{
				TScriptInterface<INoWorkInteractable> InteractableActor(OverlapResult.GetActor());
				if (InteractableActor)
				{
					Interactables.AddUnique(InteractableActor);
				}
		
				TScriptInterface<INoWorkInteractable> InteractableComponent(OverlapResult.GetComponent());
				if (InteractableComponent)
				{
					Interactables.AddUnique(InteractableComponent);
				}
			}
			
			FNoWorkInteractionQuery InteractionQuery;
			InteractionQuery.RequestingAvatar = AvatarActor;
			InteractionQuery.RequestingController = Cast<AController>(AvatarActor->GetOwner());
		
			TArray<FNoWorkInteractionInfo> InteractionInfos;
			for (TScriptInterface<INoWorkInteractable>& Interactable : Interactables)
			{
				FNoWorkInteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
				Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
			}
		
			for (FNoWorkInteractionInfo& InteractionInfo : InteractionInfos)
			{
				if (InteractionInfo.AbilityToGrant)
				{
					FObjectKey ObjectKey(InteractionInfo.AbilityToGrant);
					if (GrantedInteractionAbilities.Find(ObjectKey))
					{
						RemoveKeys.Remove(ObjectKey);
					}
					else
					{
						FGameplayAbilitySpec Spec(InteractionInfo.AbilityToGrant, 1, INDEX_NONE, this);
						FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(Spec);
						GrantedInteractionAbilities.Add(ObjectKey, SpecHandle);
					}
				}
			}
		}
		
		for (const FObjectKey& RemoveKey : RemoveKeys)
		{
			AbilitySystemComponent->ClearAbility(GrantedInteractionAbilities[RemoveKey]);
			GrantedInteractionAbilities.Remove(RemoveKey);
		}
	}
}
