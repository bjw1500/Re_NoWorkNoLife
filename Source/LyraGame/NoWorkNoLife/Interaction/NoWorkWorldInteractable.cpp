#include "NoWorkNoLife/Interaction/NoWorkWorldInteractable.h"

#include "LyraGameplayTags.h"
#include "NoWorkNoLife/Character/NoWorkCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkWorldInteractable)

// 복제 활성 + 진행자 캐시를 관리
ANoWorkWorldInteractable::ANoWorkWorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void ANoWorkWorldInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsUsed);
}

// 진행 시작 시 캐시 저장(서버) 후 BP 이벤트 호출
void ANoWorkWorldInteractable::OnInteractActiveStarted(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		CachedInteractors.Add(Interactor);
	}

	K2_OnInteractActiveStarted(Interactor);
}

// 진행 종료 시 캐시 해제(서버) 후 BP 이벤트 호출
void ANoWorkWorldInteractable::OnInteractActiveEnded(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		CachedInteractors.Remove(Interactor);
	}

	K2_OnInteractActiveEnded(Interactor);
}

// 성공 시 1회성 사용 상태 갱신, 동시 진행 중인 다른 캐릭터의 진행 능력 취소
void ANoWorkWorldInteractable::OnInteractionSuccess(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (HasAuthority())
	{
		if (bCanUsed)
		{
			bIsUsed = true;
		}

		for (TWeakObjectPtr<AActor> CachedInteractor : CachedInteractors)
		{
			if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CachedInteractor.Get()))
			{
				if (Interactor == LyraCharacter)
					continue;
				
				if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
				{
					FGameplayTagContainer CancelAbilitiesTag;
					CancelAbilitiesTag.AddTag(LyraGameplayTags::Ability_Interact_Active);
					ASC->CancelAbilities(&CancelAbilitiesTag);
				}
			}
		}
		
		CachedInteractors.Empty();
	}
	
	K2_OnInteractionSuccess(Interactor);
}

bool ANoWorkWorldInteractable::CanInteraction(const FNoWorkInteractionQuery& InteractionQuery) const
{
	return bCanUsed ? (bIsUsed == false) : true;
}
