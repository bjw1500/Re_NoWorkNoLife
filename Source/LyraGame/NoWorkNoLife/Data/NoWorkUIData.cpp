// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/Data/NoWorkUIData.h"

#include "System/LyraAssetManager.h"

const UNoWorkUIData& UNoWorkUIData::Get()
{
	return ULyraAssetManager::Get().GetUIData();
}
