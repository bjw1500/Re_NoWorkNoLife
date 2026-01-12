// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NoWorkItemData.generated.h"

class UNoWorkItemTemplate;


UCLASS(BlueprintType, Const, meta=(DisplayName="NoWork Item Data"))
class LYRAGAME_API UNoWorkItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UNoWorkItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	const UNoWorkItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass) const;
	
	void GetAllItemTemplateClasses(TArray<TSubclassOf<UNoWorkItemTemplate>>& OutItemTemplateClasses) const;

	const TArray<TSubclassOf<UNoWorkItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	const TArray<TSubclassOf<UNoWorkItemTemplate>>& GetArmorItemTemplateClasses() const { return ArmorItemTemplateClasses; }
	
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UNoWorkItemTemplate>> ItemTemplateIDToClass;

	UPROPERTY()
	TMap<TSubclassOf<UNoWorkItemTemplate>, int32> ItemTemplateClassToID;
	
private:
	UPROPERTY()
	TArray<TSubclassOf<UNoWorkItemTemplate>> WeaponItemTemplateClasses;

	UPROPERTY()
	TArray<TSubclassOf<UNoWorkItemTemplate>> ArmorItemTemplateClasses;
	
};
