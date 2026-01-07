// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Data/NoWorkItemData.h"

#include "System/LyraAssetManager.h"

#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"
#include "UObject/ObjectSaveContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

const UNoWorkItemData& UNoWorkItemData::Get()
{
	return ULyraAssetManager::Get().GetItemData();
}

void UNoWorkItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B)
{
	return A < B;
});

	ItemTemplateClassToID.Empty();

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);
	}
}

EDataValidationResult UNoWorkItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	TSet<int32> ItemTemplateIDSet;
	TSet<TSubclassOf<UNoWorkItemTemplate>> ItemTemplateClassSet;

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check
		const int32 ItemTemplateID = Pair.Key;
		
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateIDSet.Add(ItemTemplateID);

		// Class Check
		const TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass = Pair.Value;
		
		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}
		
		ItemTemplateClassSet.Add(ItemTemplateClass);
	}
	return Result;
}

const UNoWorkItemTemplate& UNoWorkItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<UNoWorkItemTemplate>* ItemTemplateClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemTemplateClass, TEXT("Can't find ItemTemplateClass from ID [%d]"), ItemTemplateID);
	return *(ItemTemplateClass->GetDefaultObject());
}

int32 UNoWorkItemData::FindItemTemplateIDByClass(TSubclassOf<UNoWorkItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}

void UNoWorkItemData::GetAllItemTemplateClasses(TArray<TSubclassOf<UNoWorkItemTemplate>>& OutItemTemplateClasses) const
{
	OutItemTemplateClasses.Reset();
	OutItemTemplateClasses.Reserve(ItemTemplateIDToClass.Num());
	
	for (auto& Pair : ItemTemplateIDToClass)
	{
		OutItemTemplateClasses.Add(Pair.Value);
	}
}
