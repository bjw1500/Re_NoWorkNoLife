// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NoWorkActivatableWidget.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API UNoWorkActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UNoWorkActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	FKey DeactivateKey;
	
};
