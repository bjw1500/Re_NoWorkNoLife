// NoWork 상호작용 시스템의 핵심 인터페이스와 헬퍼.
// - INoWorkInteractable: 상호작용 가능한 대상(액터/컴포넌트)이 구현하는 공용 계약
// - FNoWorkInteractionInfoBuilder: 상호작용 정보 수집 시 Interactable 출처를 자동 태깅하는 빌더
// - 기본 GatherPostInteractionInfos: 아바타 능력치(Resourcefulness)로 Duration 보정 후 정보 추가
#pragma once

#include "AbilitySystemBlueprintLibrary.h"
#include "NoWorkInteractionInfo.h"
#include "NoWorkInteractionQuery.h"
#include "UObject/Interface.h"
#include "NoWorkInteractable.generated.h"

class INoWorkInteractable;
struct FNoWorkInteractionInfo;
struct FNoWorkInteractionQuery;

// 한 Interactable로부터 생성된 상호작용 정보를 모으는 수집기.
// AddInteractionInfo 호출 시 자동으로 Interactable 포인터를 채워 일관성을 보장한다.
class FNoWorkInteractionInfoBuilder
{
public:
	FNoWorkInteractionInfoBuilder(TScriptInterface<INoWorkInteractable> InInteractable, TArray<FNoWorkInteractionInfo>& InInteractionInfos)
		: Interactable(InInteractable)
		, InteractionInfos(InInteractionInfos) { }

public:
    // 상호작용 정보 추가(Interactable 필드 자동 설정)
    void AddInteractionInfo(const FNoWorkInteractionInfo& InteractionInfo)
	{
		FNoWorkInteractionInfo& Entry = InteractionInfos.Add_GetRef(InteractionInfo);
		Entry.Interactable = Interactable;
	}
	
private:
	TScriptInterface<INoWorkInteractable> Interactable;
	TArray<FNoWorkInteractionInfo>& InteractionInfos;
};

// 상호작용 공용 인터페이스의 UINTERFACE 래퍼(블루프린트에서 구현 불가).
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UNoWorkInteractable : public UInterface
{
	GENERATED_BODY()
};

// 상호작용 대상이 구현해야 하는 최소 계약.
// - GetPreInteractionInfo: 기본 정보(Title/Content/Ability 등) 제공
// - GatherPostInteractionInfos: 쿼리/능력치 등을 반영해 최종 정보 집합 구성
// - CanInteraction: 현재 상호작용 가능 여부
// - GetMeshComponents: 하이라이트용 메쉬 수집
class INoWorkInteractable
{
	GENERATED_BODY()

public:
    // 기본 상호작용 정보 제공(파생 클래스에서 상황별로 채우기)
    virtual FNoWorkInteractionInfo GetPreInteractionInfo(const FNoWorkInteractionQuery& InteractionQuery) const { return FNoWorkInteractionInfo(); }
	
    // 최종 상호작용 정보 모으기: 능력치 보정 등 사후 처리 후 빌더에 추가
    virtual void GatherPostInteractionInfos(const FNoWorkInteractionQuery& InteractionQuery, FNoWorkInteractionInfoBuilder& InteractionInfoBuilder) const
	{
		FNoWorkInteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);
	
		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractionQuery.RequestingAvatar.Get()))
		{
			//float Resourcefulness = AbilitySystem->GetNumericAttribute(UNoWorkCombatSet::GetResourcefulnessAttribute());
			float Resourcefulness = 0;
			InteractionInfo.Duration = FMath::Max<float>(0.f, InteractionInfo.Duration - Resourcefulness * 0.01f);
		}
	
		InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
	}
	
    // 실제 트리거될 Gameplay Event 데이터 커스터마이즈 훅
    virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const { }
	
	UFUNCTION(BlueprintCallable)
    // 외곽선 하이라이트 대상으로 사용할 메쉬 수집
    virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const { }
	
	UFUNCTION(BlueprintCallable)
    // 현재 상호작용 가능 여부(기본 true)
    virtual bool CanInteraction(const FNoWorkInteractionQuery& InteractionQuery) const { return true; }
};
