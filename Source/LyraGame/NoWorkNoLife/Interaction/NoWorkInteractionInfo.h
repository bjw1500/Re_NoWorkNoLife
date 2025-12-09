#pragma once

#include "Abilities/GameplayAbility.h"
#include "NoWorkInteractionInfo.generated.h"

class INoWorkInteractable;

// FNoWorkInteractionInfo: 인터랙션 후보 한 건의 메타데이터(출처/텍스트/Duration/Ability/연출/UI)를 담는다.

USTRUCT(BlueprintType)
struct FNoWorkInteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<INoWorkInteractable> Interactable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Content;

    // 유지형 상호작용 시간(0이면 즉시형)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.f;

public:
    // 실제 트리거할 능력 클래스(필터링 기준)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayAbility> AbilityToGrant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ActiveStartMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ActiveEndMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="GameplayCue"))
	FGameplayTag ActiveLoopGameplayCueTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator==(const FNoWorkInteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			Title.IdenticalTo(Other.Title) &&
			Content.IdenticalTo(Other.Content) &&
			Duration == Other.Duration &&
			AbilityToGrant == Other.AbilityToGrant &&
			ActiveStartMontage == Other.ActiveStartMontage &&
			ActiveEndMontage == Other.ActiveEndMontage &&
			ActiveLoopGameplayCueTag == Other.ActiveLoopGameplayCueTag &&
			InteractionWidgetClass == Other.InteractionWidgetClass;
	}

	FORCEINLINE bool operator!=(const FNoWorkInteractionInfo& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FNoWorkInteractionInfo& Other) const
	{
		return Interactable.GetInterface() < Other.Interactable.GetInterface();
	}
};
