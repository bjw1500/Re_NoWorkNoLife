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
	void RefreshUI(UNoWorkItemInstance* NewItemInstance, int32 NewItemCount);
	// 수량만 갱신
	void RefreshItemCount(int32 NewItemCount);
	UNoWorkItemInstance* GetItemInstance() const { return ItemInstance; }

protected:
	UPROPERTY()
	// 표시 중인 아이템 인스턴스
	TObjectPtr<UNoWorkItemInstance> ItemInstance;

	UPROPERTY()
	// 스택 수량
	int32 ItemCount = 0;

protected:
	UPROPERTY(meta=(BindWidget))
	// 희귀도 배경(레이어 컬러)
	TObjectPtr<UImage> Image_RarityCover;
    
	UPROPERTY(meta=(BindWidget))
	// 아이템 아이콘 이미지
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	// 스택 수량 텍스트
	TObjectPtr<UTextBlock> Text_TempName;
	
	UPROPERTY(meta=(BindWidget))
	// 스택 수량 텍스트
	TObjectPtr<UTextBlock> Text_Count;
};
