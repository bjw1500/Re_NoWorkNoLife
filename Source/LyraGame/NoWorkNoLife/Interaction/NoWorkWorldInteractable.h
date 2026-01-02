#pragma once

#include "NoWorkInteractable.h"
#include "NoWorkWorldInteractable.generated.h"

// ANoWorkWorldInteractable: 월드 배치 상호작용 베이스
// - INoWorkInteractable 구현 + 사용 여부 복제 + 진행/성공 이벤트 훅 제공

UCLASS(Abstract, BlueprintType, Blueprintable)
class ANoWorkWorldInteractable : public AActor, public INoWorkInteractable
{
	GENERATED_BODY()
	
public:
	ANoWorkWorldInteractable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// bCanUsed 설정 시 1회성 사용 제한(bIsUsed 체크)
	virtual bool CanInteraction(const FNoWorkInteractionQuery& InteractionQuery) const override;
		
public:
	// 진행 시작/종료/성공 이벤트(블루프린트 연동용)
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractActiveStarted(AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractActiveStarted")
	void K2_OnInteractActiveStarted(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractActiveEnded(AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractActiveEnded")
	void K2_OnInteractActiveEnded(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionSuccess(AActor* Interactor);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInteractionSuccess")
	void K2_OnInteractionSuccess(AActor* Interactor);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bCanUsed = false;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsUsed = false;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedInteractors;
};
