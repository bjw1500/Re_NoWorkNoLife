// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/NoWorkItemEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

UNoWorkItemEntryWidget::UNoWorkItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNoWorkItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 기본 수량 텍스트는 비표시
	Text_Count->SetText(FText::GetEmpty());
}

void UNoWorkItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNoWorkItemEntryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UNoWorkItemEntryWidget::RefreshUI(UNoWorkItemInstance* NewItemInstance, int32 NewItemCount)
{
	Super::RefreshUI(NewItemInstance, NewItemCount);
}

void UNoWorkItemEntryWidget::RefreshItemCount(int32 NewItemCount)
{
	Super::RefreshItemCount(NewItemCount);
}
