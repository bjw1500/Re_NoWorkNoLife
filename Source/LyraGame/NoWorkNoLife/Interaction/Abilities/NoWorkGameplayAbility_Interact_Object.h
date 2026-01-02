#pragma once

#include "NoWorkGameplayAbility_Interact_Info.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionInfo.h"
#include "NoWorkGameplayAbility_Interact_Object.generated.h"

UCLASS()
class UNoWorkGameplayAbility_Interact_Object : public UNoWorkGameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UNoWorkGameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual  void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnInvalidInteraction();

protected:

	UPROPERTY(EditDefaultsOnly, Category="NoWork | Interaction")
	float AcceptanceAngle = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="NoWork | Interaction")
	float AcceptanceDistance = 90.f;
	
protected:

	bool bInitialized = false;
	
};
