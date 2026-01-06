// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Item/NoWorkItemInstance.h"

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
