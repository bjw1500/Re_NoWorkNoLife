#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionQuery.h"
#include "NoWorkAbilityTask_WaitForInteractableTraceHit.generated.h"

// UNoWorkAbilityTask_WaitForInteractableTraceHit
// - 카메라/조준선 기반 라인 트레이스를 주기적으로 수행하여
//   INoWorkInteractable 대상들을 수집하고 상호작용 후보 목록을 방송한다.

struct FNoWorkInteractionInfo;
class INoWorkInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const TArray<FNoWorkInteractionInfo>&, InteractableInfos);

UCLASS()
class UNoWorkAbilityTask_WaitForInteractableTraceHit : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UNoWorkAbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UNoWorkAbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FNoWorkInteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange = 100.f, float InteractionTraceRate = 0.1f, bool bShowDebug = false);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
    // 카메라 기준 트레이스 → 명중 대상에서 Interactable 수집
    void PerformTrace();
	
	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch = false) const;
	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const;
	void LineTrace(const FVector& Start, const FVector& End, const FCollisionQueryParams& Params, FHitResult& OutHitResult) const;

    // 수집된 대상을 최종 상호작용 정보로 변환/필터링
    void UpdateInteractionInfos(const FNoWorkInteractionQuery& InteractQuery, const TArray<TScriptInterface<INoWorkInteractable>>& Interactables);
    // 후보들의 메쉬 외곽선 하이라이트 토글
    void HighlightInteractables(const TArray<FNoWorkInteractionInfo>& InteractionInfos, bool bShouldHighlight);

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged InteractableChanged;

private:
	UPROPERTY()
	FNoWorkInteractionQuery InteractionQuery;

	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;

	ECollisionChannel TraceChannel = ECC_Visibility;
	float InteractionTraceRange = 100.f;
	float InteractionTraceRate = 0.1f;
	bool bShowDebug = false;
	
	FTimerHandle TraceTimerHandle;
	TArray<FNoWorkInteractionInfo> CurrentInteractionInfos;
};
