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
