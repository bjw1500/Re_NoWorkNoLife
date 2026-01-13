// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/BlueprintFunctionLibrary/NoWorkBlueprintFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>


void UNoWorkBlueprintFunctionLibrary::DebugLog(FString string)
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *string);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
	}
}

void UNoWorkBlueprintFunctionLibrary::DebugMessage(const char* FunctionName, const char* FileName, int32 LineNumber)
{
	if (GEngine == nullptr)
		return;

	FString Result = FString::Printf(TEXT("%s is Error %s line - %d"), ANSI_TO_TCHAR(FunctionName), ANSI_TO_TCHAR(FileName), LineNumber);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *Result);
}

void UNoWorkBlueprintFunctionLibrary::NotifyFeatureIncomplete()
{
	DEBUG_LOG("아직 제작 중인 기능입니다.");
}

void UNoWorkBlueprintFunctionLibrary::Checkf(UObject* Object, FString Message)
{
	if (GEngine)
	{
		checkf(Object, TEXT("%s"), *Message);
	}
}

bool UNoWorkBlueprintFunctionLibrary::CheckValid(UObject* Object, FString Message, const char* FunctionName,
	const char* FileName, int32 LineNumber)
{
	if (IsValid(Object) == true)
		return true;
	if (GEngine == nullptr)
		return false;

	FString Result = FString::Printf(TEXT("%s Error %s %d, %s is not valid"), ANSI_TO_TCHAR(FunctionName), ANSI_TO_TCHAR(FileName), LineNumber, *Message);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Result);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *Result);

	return false;
}
