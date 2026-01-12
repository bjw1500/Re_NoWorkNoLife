// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/Fragments/NoWorkItemFragment_Equipable_Armor.h"

#include "UObject/ObjectSaveContext.h"

UNoWorkItemFragment_Equipable_Armor::UNoWorkItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Armor;
}

void UNoWorkItemFragment_Equipable_Armor::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
}

void UNoWorkItemFragment_Equipable_Armor::OnInstanceCreated(UNoWorkItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
}
