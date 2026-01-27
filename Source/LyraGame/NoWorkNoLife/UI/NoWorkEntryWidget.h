// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoWorkNoLife/UI/NoWorkUserWidget.h"
#include "NoWorkEntryWidget.generated.h"


class UTextBlock;
class UImage;
class UNoWorkItemHoverWidget;
class UNoWorkItemInstance;

UCLASS()
class LYRAGAME_API UNoWorkEntryWidget : public UNoWorkUserWidget
{
	GENERATED_BODY()

public:
	UNoWorkEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	// 드래그 중 반투명/일반 상태 전환(시각 피드백)
	void RefreshWidgetOpacity(bool bClearlyVisible);

public:
	// 아이템/수량을 수신해 아이콘/희귀도 배경/수량 텍스트를 갱신한다.
	virtual  void RefreshUI(UNoWorkItemInstance* NewItemInstance, int32 NewItemCount);
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
    
	UPROPERTY()
	// 호버(툴팁) 위젯 인스턴스
	TObjectPtr<UNoWorkItemHoverWidget> HoverWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	// 희귀도 배경(레이어 컬러)
	TObjectPtr<UImage> Image_RarityCover;
    
	UPROPERTY(meta=(BindWidget))
	// 아이템 아이콘 이미지
	TObjectPtr<UImage> Image_Icon;
    
	UPROPERTY(meta=(BindWidget))
	// 호버 시 표시되는 오버레이 이미지
	TObjectPtr<UImage> Image_Hover;

	UPROPERTY(meta=(BindWidget))
	// 스택 수량 텍스트
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(meta=(BindWidget))
	// 스택 수량 텍스트
	TObjectPtr<UTextBlock> Text_TempName;
	
};
