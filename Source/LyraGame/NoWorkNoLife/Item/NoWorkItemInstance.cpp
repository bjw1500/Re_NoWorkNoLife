// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/NoWorkItemInstance.h"

#include "NoWorkItemTemplate.h"

// Ensure generated code is compiled in the same TU
#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkItemInstance)

UNoWorkItemInstance::UNoWorkItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNoWorkItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UNoWorkItemInstance::IsSupportedForNetworking() const
{ return true; }

float UNoWorkItemInstance::GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags) const
{
	return 0;
}

float UNoWorkItemInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	return 0;
}

void UNoWorkItemInstance::Init(int32 InItemTemplateID, EItemRarity InItemRarity)
{
	if (InItemTemplateID <= INDEX_NONE || InItemRarity == EItemRarity::Count)
		return;

	ItemTemplateID = InItemTemplateID;
	ItemRarity = InItemRarity;

	// const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemTemplateID);
	// for (const UD1ItemFragment* Fragment : ItemTemplate.Fragments)
	// {
	// 	if (Fragment)
	// 	{
	// 		Fragment->OnInstanceCreated(this);
	// 	}
	// }
}
