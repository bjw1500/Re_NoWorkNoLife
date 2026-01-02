#include "NoWorkGameplayAbility_Interact_Info.h"

#include "NoWorkNoLife/Interaction/NoWorkInteractable.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractionQuery.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkGameplayAbility_Interact_Info)

UNoWorkGameplayAbility_Interact_Info::UNoWorkGameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UNoWorkGameplayAbility_Interact_Info::InitializeAbility(AActor* TargetActor)
{
    // 대상 액터가 INoWorkInteractable을 구현하는지 확인(실패 시 조기 종료)
    TScriptInterface<INoWorkInteractable> TargetInteractable(TargetActor);
    if (TargetInteractable == nullptr)
        return false;

    // 상호작용 쿼리 구성: 요청자(아바타/컨트롤러) 정보 주입
    FNoWorkInteractionQuery InteractionQuery;
    InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
    InteractionQuery.RequestingController = GetControllerFromActorInfo();

    // 인터페이스/액터 캐시
    Interactable = TargetInteractable;
    InteractableActor = TargetActor;

    // 빌더를 통해 최종 상호작용 정보 집계
    // 주의: GatherPostInteractionInfos는 최소 1개 이상의 항목을 추가한다는 계약을 가정한다.
    //       (비어있을 가능성에 대한 방어는 호출 측 정책에 따라 추가 고려 필요)
    TArray<FNoWorkInteractionInfo> InteractionInfos;
    FNoWorkInteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
    Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
    InteractionInfo = InteractionInfos[0];
    
    return true;
}
