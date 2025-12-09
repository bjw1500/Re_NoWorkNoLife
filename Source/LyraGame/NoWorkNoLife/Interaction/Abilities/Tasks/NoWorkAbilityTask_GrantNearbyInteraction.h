#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "NoWorkAbilityTask_GrantNearbyInteraction.generated.h"

UCLASS()
class UNoWorkAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UNoWorkAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractables(UGameplayAbility* OwningAbility, float InteractionAbilityScanRange, float InteractionAbilityScanRate);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	virtual bool IsSupportedForNetworking() const override { return true; }
	
private:
	void QueryInteractables();
	
private:
	float InteractionAbilityScanRange = 100.f;
	float InteractionAbilityScanRate = 0.1f;

	FTimerHandle QueryTimerHandle;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> GrantedInteractionAbilities;
};
