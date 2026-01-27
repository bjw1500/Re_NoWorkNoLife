// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoWorkNoLife/UI/NoWorkEntryWidget.h"
#include "NoWorkItemEntryWidget.generated.h"

class UTextBlock;
class UImage;
class UNoWorkItemInstance;
/**
 * 
 */
UCLASS()
class LYRAGAME_API UNoWorkItemEntryWidget : public UNoWorkEntryWidget
{
	GENERATED_BODY()

public:

	UNoWorkItemEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// 아이템/수량을 수신해 아이콘/희귀도 배경/수량 텍스트를 갱신한다.
	virtual void RefreshUI(UNoWorkItemInstance* NewItemInstance, int32 NewItemCount) override;
	// 수량만 갱신
	void RefreshItemCount(int32 NewItemCount);
	UNoWorkItemInstance* GetItemInstance() const { return ItemInstance; }
	
};
