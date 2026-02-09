#pragma once

#include "NoWorkGameplayAbility_Interact_Info.h"
#include "NoWorkGameplayAbility_Interact_Active.generated.h"

// UNoWorkGameplayAbility_Interact_Active
// - 입력 유지/시간 경과/연출을 관리하고 마지막에 실제 Ability를 트리거하는 진행형 능력
// - UNoWorkGameplayAbility_Interact_Info에서 초기화된 대상/정보를 사용

UCLASS()
class UNoWorkGameplayAbility_Interact_Active : public UNoWorkGameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UNoWorkGameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	UFUNCTION(BlueprintCallable)
	void FlushPressedInput(UInputAction* InputAction);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void OnInvalidInteraction();

	UFUNCTION()
	void OnInputReleased(float TimeHeld);
	
	UFUNCTION()
	void OnDurationEnded();

	UFUNCTION()
	void OnNetSync();
	
	UFUNCTION()
	bool TriggerInteraction();

protected:
	
	bool TriggerInteractionWithAbility(TSubclassOf<UGameplayAbility> AbilityClass);
	
	FTimerHandle DurationTimerHandle;
	
	bool bInteractionTriggered = false;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	TObjectPtr<UInputAction> MoveInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float AcceptanceAngle = 65.f;

	UPROPERTY(EditDefaultsOnly, Category="NoWork|Interaction")
	float AcceptanceDistance = 10.f;
};
