// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "UObject/NoExportTypes.h"
#include "NoWorkItemInstance.generated.h"

// UItemInstance: 아이템의 런타임 인스턴스
// - 템플릿 ID/희귀도/스탯 태그 스택을 보관하며 네트워크 복제
// - LyraAbilitySourceInterface로 각종 감쇠 로직 제공
UCLASS()
class LYRAGAME_API UNoWorkItemInstance : public UObject, public ILyraAbilitySourceInterface
{
	GENERATED_BODY()
	
public:
	UNoWorkItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
};
