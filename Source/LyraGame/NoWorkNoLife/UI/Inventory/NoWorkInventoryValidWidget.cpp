#include "NoWorkInventoryValidWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"

#include "NoWorkNoLife/Data/NoWorkUIData.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkInventoryValidWidget)

UNoWorkInventoryValidWidget::UNoWorkInventoryValidWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkInventoryValidWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 데이터 에셋의 단위 슬롯 크기로 오버레이 픽셀 크기를 통일한다.
    FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
    SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
    SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}

void UNoWorkInventoryValidWidget::ChangeSlotState(ESlotState SlotState)
{
    switch (SlotState)
    {
    case ESlotState::Default:
        // 기본 상태: 모두 숨김
        Image_Red->SetVisibility(ESlateVisibility::Hidden);
        Image_Green->SetVisibility(ESlateVisibility::Hidden);
        break;
    case ESlotState::Invalid:
        // 배치 불가: 레드만 표시
        Image_Red->SetVisibility(ESlateVisibility::Visible);
        Image_Green->SetVisibility(ESlateVisibility::Hidden);
        break;
    case ESlotState::Valid:
        // 배치 가능: 그린만 표시
        Image_Red->SetVisibility(ESlateVisibility::Hidden);
        Image_Green->SetVisibility(ESlateVisibility::Visible);
        break;
    }
}
