#pragma once

#include "Abilities/Tasks/AbilityTask.h"

// UNoWorkAbilityTask_WaitInputStart
// - AbilitySystem의 입력 시작을 기다려 OnStart 델리게이트를 호출
#include "NoWorkAbilityTask_WaitInputStart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputStartDelegate);

UCLASS()
class UNoWorkAbilityTask_WaitInputStart : public UAbilityTask
{
	GENERATED_BODY()

public:
	UNoWorkAbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UNoWorkAbilityTask_WaitInputStart* WaitInputStart(UGameplayAbility* OwningAbility);
	
public:
	virtual void Activate() override;
	
public:
	UFUNCTION()
	void OnStartCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FInputStartDelegate OnStart;
	
protected:
	FDelegateHandle DelegateHandle;
};
