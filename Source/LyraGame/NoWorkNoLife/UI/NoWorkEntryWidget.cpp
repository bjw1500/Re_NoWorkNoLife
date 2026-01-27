// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/NoWorkEntryWidget.h"

#include "NoWorkItemHoverWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"

UNoWorkEntryWidget::UNoWorkEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNoWorkEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Text_Count->SetText(FText::GetEmpty());
}

void UNoWorkEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Image_Hover->SetVisibility(ESlateVisibility::Hidden);
}

void UNoWorkEntryWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		// 호버 위젯 정리
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
    
	Super::NativeDestruct();
}

void UNoWorkEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// 호버 프레임 표시
	Image_Hover->SetVisibility(ESlateVisibility::Visible);

	if (HoverWidget == nullptr)
	{
		TSubclassOf<UNoWorkItemHoverWidget> HoverWidgetClass = UNoWorkUIData::Get().ItemHoverWidgetClass;
		HoverWidget = CreateWidget<UNoWorkItemHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
	}

	if (HoverWidget)
	{
		// 호버 내용 갱신 후 Viewport에 추가
		HoverWidget->RefreshUI(ItemInstance);
		HoverWidget->AddToViewport();
	}
}

FReply UNoWorkEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		// 마우스 따라 호버의 화면 위치 갱신
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
    
	return Reply;
}

void UNoWorkEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// 호버 프레임 숨김 및 호버 위젯 제거
	Image_Hover->SetVisibility(ESlateVisibility::Hidden);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}

FReply UNoWorkEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// 좌클릭 드래그 검출 시작
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UNoWorkEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// 드래그 중에는 반투명 처리
	RefreshWidgetOpacity(false);
}

void UNoWorkEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	// 취소 시 불투명 복구
	RefreshWidgetOpacity(true);
}

void UNoWorkEntryWidget::RefreshWidgetOpacity(bool bClearlyVisible)
{
	// 간단한 시각 피드백(0.5: 드래그 중, 1.0: 평상시)
	SetRenderOpacity(bClearlyVisible ? 1.f : 0.5f);
}

void UNoWorkEntryWidget::RefreshUI(UNoWorkItemInstance* NewItemInstance, int32 NewItemCount)
{
	if (NewItemInstance == nullptr || NewItemCount < 1)
		return;
    
	ItemInstance = NewItemInstance;
	ItemCount = NewItemCount;

	const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	// 아이콘/수량/희귀도 배경 갱신
	Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));

	UTexture2D* RarityTexture = UNoWorkUIData::Get().GetEntryRarityTexture(ItemInstance->GetItemRarity());
	Image_RarityCover->SetBrushFromTexture(RarityTexture, true);

	//아이템 아이콘이 없으면 임시 방편으로 이름만 표시한다.
	if (ItemTemplate.IconTexture == nullptr)
	{
		Text_TempName->SetText(ItemTemplate.DisplayName);
	}
}

void UNoWorkEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	if (NewItemCount < 1)
		return;

	ItemCount = NewItemCount;
	Text_Count->SetText(ItemCount <= 1 ? FText::GetEmpty() : FText::AsNumber(ItemCount));
}
