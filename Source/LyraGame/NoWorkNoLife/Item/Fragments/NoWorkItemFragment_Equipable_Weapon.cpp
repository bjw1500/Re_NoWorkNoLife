// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/Fragments/NoWorkItemFragment_Equipable_Weapon.h"

#include "UObject/ObjectSaveContext.h"

UNoWorkItemFragment_Equipable_Weapon::UNoWorkItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Weapon;
}

void UNoWorkItemFragment_Equipable_Weapon::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
}

void UNoWorkItemFragment_Equipable_Weapon::OnInstanceCreated(UNoWorkItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
}
