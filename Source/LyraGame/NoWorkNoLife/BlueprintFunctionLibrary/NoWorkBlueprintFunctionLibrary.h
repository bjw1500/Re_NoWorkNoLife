// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NoWorkBlueprintFunctionLibrary.generated.h"


using Util = UNoWorkBlueprintFunctionLibrary;

#define DEBUG_LOG(Message)	UNoWorkBlueprintFunctionLibrary::DebugLog(TEXT(Message))
#define DEBUG_MESSAGE		UNoWorkBlueprintFunctionLibrary::DebugMessage(__FUNCTION__, __FILE__, __LINE__)
#define CHECK_VALID(Object) UNoWorkBlueprintFunctionLibrary::CheckValid(Object,TEXT(#Object),__FUNCTION__, __FILE__, __LINE__)
#define DEBUG_INCOMPLETE	UNoWorkBlueprintFunctionLibrary::NotifyFeatureIncomplete()


UCLASS()
class LYRAGAME_API UNoWorkBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NoWorkBlueprintLibrary | Utils")
	static void DebugLog(FString string);
	static void DebugMessage(const char* FunctionName, const char* FileName, int32 LineNumber);
	
	UFUNCTION(BlueprintCallable, Category = "NoWorkBlueprintLibrary | Utils")
	static void NotifyFeatureIncomplete();

	UFUNCTION(BlueprintCallable, Category = "NoWorkBlueprintLibrary | Utils")
	static void Checkf(UObject* Object, FString Message);

	static bool CheckValid(UObject* Object, FString Message, const char* FunctionName, const char* FileName, int32 LineNumber);
};
