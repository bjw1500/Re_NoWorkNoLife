// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoWorkNoLife/UI/NoWorkUserWidget.h"
#include "NoWorkItemHoverEntryWidget.generated.h"

// 아이템 호버(툴팁) 항목 위젯.
// - 표시 이름/희귀도/타입/설명/스탯/사용 가능 클래스 등을 갱신한다.
class UImage;
class UTextBlock;
class UHorizontalBox;
class URichTextBlock;
class UNoWorkItemInstance;

UCLASS()
class LYRAGAME_API UNoWorkItemHoverEntryWidget : public UNoWorkUserWidget
{
	GENERATED_BODY()

public:
	UNoWorkItemHoverEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void NativePreConstruct() override;

public:

	void RefreshUI(const UNoWorkItemInstance* ItemInstance);

protected:

	UPROPERTY(meta=(BindWidget))
	// 아이템 이름 텍스트(희귀도 색 적용)
	TObjectPtr<UTextBlock> Text_DisplayName;
	
	UPROPERTY(meta=(BindWidget))
	// 희귀도 텍스트
	TObjectPtr<UTextBlock> Text_ItemRarity;

	UPROPERTY(meta=(BindWidget))
	// 타입 텍스트(무기/방어구/유틸 세부 타입)
	TObjectPtr<UTextBlock> Text_ItemType;
};
