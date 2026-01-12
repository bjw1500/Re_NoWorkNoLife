// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"
#include "NoWorkNoLife/Item/Fragments/NoWorkItemFragment_Equipable.h"
#include "NoWorkItemFragment_Equipable_Armor.generated.h"

class UNoWorkItemInstance;

UCLASS()
class LYRAGAME_API UNoWorkItemFragment_Equipable_Armor : public UNoWorkItemFragment_Equipable
{
	GENERATED_BODY()

public:
	UNoWorkItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UNoWorkItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;
	
};
