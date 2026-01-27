// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/NoWorkItemHoverEntryWidget.h"

#include "Components/TextBlock.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

#include "Kismet/KismetStringLibrary.h"
#include "NoWorkNoLife/Item/Fragments/NoWorkItemFragment_Equipable.h"

#define LOCTEXT_NAMESPACE "ItemHoverWidget"

UNoWorkItemHoverEntryWidget::UNoWorkItemHoverEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNoWorkItemHoverEntryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UNoWorkItemHoverEntryWidget::RefreshUI(const UNoWorkItemInstance* ItemInstance)
{
	if (ItemInstance == nullptr)
	{
		// 대상이 없으면 숨김 처리
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	const EItemRarity ItemRarity = ItemInstance->GetItemRarity();

	FColor RarityColor = UNoWorkUIData::Get().GetRarityColor(ItemRarity);
	
	// Display Name
	Text_DisplayName->SetText(ItemTemplate.DisplayName);
	Text_DisplayName->SetColorAndOpacity(RarityColor);

	// Item Rarity(희귀도 텍스트/색)
	//#define LOCTEXT_NAMESPACE "ItemHoverWidget" ?? 이게 정확히 무슨 용도?
	FText ItemRarityText;
	switch (ItemRarity)
	{
	case EItemRarity::Poor:			ItemRarityText = LOCTEXT("ItemRarity-Poor",			"Poor");		break;
	case EItemRarity::Common:		ItemRarityText = LOCTEXT("ItemRarity-Common",		"Common");		break;
	case EItemRarity::Uncommon:		ItemRarityText = LOCTEXT("ItemRarity-Uncommon",		"Uncommon");	break;
	case EItemRarity::Rare:			ItemRarityText = LOCTEXT("ItemRarity-Rare",			"Rare");		break;
	case EItemRarity::Legendary:	ItemRarityText = LOCTEXT("ItemRarity-Legendary",	"Legendary");	break;
	}
	Text_ItemRarity->SetText(ItemRarityText);
	Text_ItemRarity->SetColorAndOpacity(RarityColor);

	EEquipmentType ItemType = EEquipmentType::Utility;
	if (const UNoWorkItemFragment_Equipable* EquipableFragment = ItemTemplate.FindFragmentByClass<UNoWorkItemFragment_Equipable>())
	{
		ItemType = EquipableFragment->EquipmentType;
	}

	FText ItemTypeText;
	switch (ItemType)
	{
	case EEquipmentType::Utility:		ItemTypeText = LOCTEXT("ItemType-Utility",				"Utility");		break;
	case EEquipmentType::Armor:			ItemTypeText = LOCTEXT("ItemType-Armor",				"Armor");		break;
	case EEquipmentType::Weapon:			ItemTypeText = LOCTEXT("ItemType-Armor",				"Weapon");		break;
	}
	Text_ItemType->SetText(ItemRarityText);
}
