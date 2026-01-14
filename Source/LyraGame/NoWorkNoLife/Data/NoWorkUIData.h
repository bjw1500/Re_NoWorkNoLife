// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NoWorkUIData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Const, meta=(DisplayName="NoWork UI Data"))
class LYRAGAME_API UNoWorkUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	static const UNoWorkUIData& Get();
	
};
