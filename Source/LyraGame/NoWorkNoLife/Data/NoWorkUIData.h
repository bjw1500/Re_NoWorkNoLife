// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkUIData.generated.h"


class UNoWorkInventoryValidWidget;
class UNoWorkInventoryEntryWidget;
class UNoWorkInventorySlotWidget;
class UNoWorkItemHoverWidget;
class UNoWorkItemDragWidget;

// 아이템 희귀도별 UI 표시 정보를 정의하는 구조체
// - 배열(RarityInfoEntries)로 관리되며, 인덱스는 EItemRarity를 그대로 사용
// - Color: 희귀도 색상, EntryTexture: 일반 표시, HoverTexture: 호버 표시
USTRUCT(BlueprintType)
struct FNoWorkItemRarityInfoEntry
{
	GENERATED_BODY()

public:
	// 희귀도 색상 (알파 채널 노출 숨김)
	UPROPERTY(EditDefaultsOnly, meta=(HideAlphaChannel))
	FColor Color = FColor::Black;
    
	// 일반 표시(엔트리)용 텍스처
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> EntryTexture;
};

UCLASS(BlueprintType, Const, meta=(DisplayName="NoWork UI Data"))
class LYRAGAME_API UNoWorkUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	static const UNoWorkUIData& Get();

public:

	// 희귀도 → 일반 표시 텍스처 (없으면 nullptr)
	UTexture2D* GetEntryRarityTexture(EItemRarity ItemRarity) const;
	// 희귀도 → 색상 (잘못된 인덱스면 Black)
	FColor GetRarityColor(EItemRarity ItemRarity) const;

public:
	// 인벤토리 한 칸(단위) 픽셀 크기 설정
	UPROPERTY(EditDefaultsOnly)
	// 인벤토리 단위 슬롯의 픽셀 크기(가로/세로). 모든 슬롯/엔트리/유효표시의 기준.
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;
	
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNoWorkInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNoWorkInventoryEntryWidget> InventoryEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNoWorkInventoryValidWidget> InventoryValidWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNoWorkItemHoverWidget> ItemHoverWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNoWorkItemDragWidget> ItemDragWidgetClass;

private:

	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeEnum="EItemRarity"))
	FNoWorkItemRarityInfoEntry RarityInfoEntries[(int32)EItemRarity::Count];
	
};
