#pragma once

#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkNoLife/Interaction/NoWorkWorldInteractable.h"
#include "NoWorkChestBase.generated.h"

// ANoWorkChestBase: 상자 인터랙터 베이스
// - 열림/닫힘 상태 복제, 상태별 상호작용 정보/몽타주, 인벤토리 초기 채움 로직 보유

class UArrowComponent;

UENUM(BlueprintType)
enum class EChestState : uint8
{
	Open,
	Close
};

UCLASS()
class ANoWorkChestBase : public ANoWorkWorldInteractable
{
	GENERATED_BODY()
	
public:
	ANoWorkChestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 현재 상태(열림/닫힘)에 맞는 상호작용 정보 반환
	virtual FNoWorkInteractionInfo GetPreInteractionInfo(const FNoWorkInteractionQuery& InteractionQuery) const override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetChestState(EChestState NewChestState);

	EChestState GetChestState() {return ChestState;}
	
private:
	UFUNCTION()
	void OnRep_ChestState();

protected:
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_ChestState)
	EChestState ChestState = EChestState::Close;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	FNoWorkInteractionInfo OpenedInteractionInfo;
	
	UPROPERTY(EditDefaultsOnly, Category="Info")
	FNoWorkInteractionInfo ClosedInteractionInfo;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	TObjectPtr<UAnimMontage> OpenMontage;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	TObjectPtr<UAnimMontage> CloseMontage;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
};
