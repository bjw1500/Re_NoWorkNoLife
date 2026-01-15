#include "NoWorkInventorySlotWidget.h"

#include "Components/SizeBox.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkInventorySlotWidget)

UNoWorkInventorySlotWidget::UNoWorkInventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkInventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 데이터 에셋에서 단위 슬롯 크기를 가져와 배경 칸의 픽셀 크기를 통일한다.
	FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}
