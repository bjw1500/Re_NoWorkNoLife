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

void UNoWorkItemEntryWidget::RefreshItemCount(int32 NewItemCount)
{
}
