#pragma once

#include "NoWorkGameplayAbility_Interact_Object.h"
#include "NoWorkGameplayAbility_Interact_Chest.generated.h"

class UCommonActivatableWidget;

UCLASS()
class UNoWorkGameplayAbility_Interact_Chest : public UNoWorkGameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UNoWorkGameplayAbility_Interact_Chest(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual  void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:

	UFUNCTION()
	void OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget);
	
protected:

	UPROPERTY(EditDefaultsOnly, Category="NoWork | Interaction")
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="NoWork | Interaction")
	FGameplayTag WidgetLayerTag;

private:

	UPROPERTY()
	TObjectPtr<UCommonActivatableWidget> PushedWidget;
	
};
