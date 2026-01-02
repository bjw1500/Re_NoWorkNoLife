#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "NoWorkAbilityTask_WaitForInvalidInteraction.generated.h"

// UNoWorkAbilityTask_WaitForInvalidInteraction
// - 상호작용 진행 중 플레이어가 허용 각도/거리 범위를 벗어나는지 주기적으로 검사
// - 벗어나면 OnInvalidInteraction을 브로드캐스트하고 태스크 종료

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvalidInteraction);

UCLASS()
class UNoWorkAbilityTask_WaitForInvalidInteraction : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UNoWorkAbilityTask_WaitForInvalidInteraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UNoWorkAbilityTask_WaitForInvalidInteraction* WaitForInvalidInteraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	// 캐시된 기준 대비 현재 각도/거리 허용 여부 검사(실패 시 무효 처리)
	void PerformCheck();
	// 초기 전방(2D)과 현재 전방(2D) 사이 각도 계산(degree)
	float CalculateAngle2D() const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnInvalidInteraction OnInvalidInteraction;

private:
	float AcceptanceAngle = 0.f;
	float AcceptanceDistance = 0.f;
	
private:
	FTimerHandle CheckTimerHandle;
	FVector CachedCharacterForward2D;
	FVector CachedCharacterLocation;
};
