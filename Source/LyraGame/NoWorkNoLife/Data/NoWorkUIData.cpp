// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Data/NoWorkUIData.h"

#include "NoWorkNoLife/NoWorkDefine.h"
#include "System/LyraAssetManager.h"

const UNoWorkUIData& UNoWorkUIData::Get()
{
	return ULyraAssetManager::Get().GetUIData();
}

UTexture2D* UNoWorkUIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	// 방어 코드: Enum 유효성 검사 (Count 미만인지 확인)
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;
	
	return RarityInfoEntries[ItemRarityIndex].EntryTexture;
}

FColor UNoWorkUIData::GetRarityColor(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return FColor::Black;
	
	return RarityInfoEntries[ItemRarityIndex].Color;
}
