#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionInfo.h"
// 대상 액터가 INoWorkInteractable을 구현했는지 확인하고,
// 상호작용 쿼리로부터 최종 FNoWorkInteractionInfo 하나를 준비/캐시하는 능력 클래스의 베이스.
// - InitializeAbility에서 인터페이스/액터/정보를 수집해 멤버에 저장
// - 블루프린트에서 읽기 전용 멤버로 접근 가능
#include "NoWorkGameplayAbility_Interact_Info.generated.h"

UCLASS()
class UNoWorkGameplayAbility_Interact_Info : public ULyraGameplayAbility
{
    GENERATED_BODY()
    
public:
    UNoWorkGameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

    /**
     * 상호작용 준비 단계.
     * - TargetActor가 INoWorkInteractable을 구현하는지 확인
     * - 요청자(Avatar/Controller) 정보를 담은 쿼리를 구성
     * - Interactable/InteractableActor/InteractionInfo 멤버를 채움
     * @return 성공 시 true, 대상이 Interactable이 아니면 false
     */
    UFUNCTION(BlueprintCallable)
    bool InitializeAbility(AActor* TargetActor);

protected:

    /** 대상이 구현한 상호작용 인터페이스 포인터(유효 시 InitializeAbility에서 설정) */
    UPROPERTY(BlueprintReadOnly)
    TScriptInterface<INoWorkInteractable> Interactable;

    /** 인터페이스의 실제 소유 액터(편의상 함께 캐시) */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> InteractableActor;

    /** 쿼리/후처리를 거쳐 선택된 최종 상호작용 정보(보통 첫 번째 엔트리) */
    UPROPERTY(BlueprintReadOnly)
    FNoWorkInteractionInfo InteractionInfo;
};
