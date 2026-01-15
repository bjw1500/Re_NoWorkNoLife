#pragma once

#include "Blueprint/UserWidget.h"
#include "NoWorkInventorySlotWidget.generated.h"

// 인벤토리 그리드의 "한 칸"을 표현하는 배경 위젯.
// - 단위 슬롯 크기(UnitInventorySlotSize)에 맞춰 고정 크기로 초기화된다.
// - 실제 아이템은 Canvas 위에 엔트리 위젯으로 따로 그려지며,
//   이 위젯은 시각적인 그리드 배경 역할만 담당한다.
class UImage;
class USizeBox;

UCLASS()
class UNoWorkInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UNoWorkInventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// 위젯 초기화 시점에 단위 슬롯 크기를 적용해 고정 크기로 설정한다.
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(meta=(BindWidget))
	// 슬롯 배경의 실제 크기를 제어하는 루트 SizeBox
	TObjectPtr<USizeBox> SizeBox_Root;
};
