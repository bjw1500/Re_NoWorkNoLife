// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

#include "Misc/DataValidation.h"

UNoWorkItemTemplate::UNoWorkItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EDataValidationResult UNoWorkItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (SlotCount.X < 1 || SlotCount.Y < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("SlotCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}
	
	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("MaxStackCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}
	return Result;
}

const UNoWorkItemFragment* UNoWorkItemTemplate::FindFragmentByClass(
	TSubclassOf<UNoWorkItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UNoWorkItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
