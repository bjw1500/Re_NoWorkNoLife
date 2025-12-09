#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionInfo.h"
#include "NoWorkGameplayAbility_Interact.generated.h"

// UNoWorkGameplayAbility_Interact
// - 상시 활성화되어 상호작용 후보를 탐색/하이라이트/알림하고 입력을 받아 진행 능력을 트리거한다.

USTRUCT(BlueprintType)
struct FNoWorkInteractionMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRefresh = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSwitchActive = false;
	
	UPROPERTY(BlueprintReadWrite)
	FNoWorkInteractionInfo InteractionInfo = FNoWorkInteractionInfo();
};

UCLASS()
class UNoWorkGameplayAbility_Interact : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UNoWorkGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
    // 트레이스 태스크가 브로드캐스트한 후보 목록을 UI/상태에 반영
    UFUNCTION(BlueprintCallable)
    void UpdateInteractions(const TArray<FNoWorkInteractionInfo>& InteractionInfos);

    // 현재 첫 후보를 대상으로 진행 능력(Ability_Interact_Active) 이벤트 송신
    UFUNCTION(BlueprintCallable)
    void TriggerInteraction();

private:
	void WaitInputStart();
	
	UFUNCTION()
	void OnInputStart();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FNoWorkInteractionInfo> CurrentInteractionInfos;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float InteractionTraceRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float InteractionTraceRate = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float InteractionScanRange = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	bool bShowTraceDebug = false;
	
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
