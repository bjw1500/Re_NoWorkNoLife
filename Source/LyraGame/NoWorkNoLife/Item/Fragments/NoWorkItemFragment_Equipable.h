// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"
#include "NoWorkItemFragment_Equipable.generated.h"

class ULyraAbilitySet;

UCLASS()
class LYRAGAME_API UNoWorkItemFragment_Equipable : public UNoWorkItemFragment
{
	GENERATED_BODY()

public:

	UNoWorkItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const ULyraAbilitySet> BaseAbilitySet;
	
};
