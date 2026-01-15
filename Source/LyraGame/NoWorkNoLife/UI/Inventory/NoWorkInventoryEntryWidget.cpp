#include "NoWorkInventoryEntryWidget.h"

#include "NoWorkInventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkInventoryEntryWidget)

UNoWorkInventoryEntryWidget::UNoWorkInventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkInventoryEntryWidget::Init(UNoWorkInventorySlotsWidget* InSlotsWidget, UNoWorkItemInstance* InItemInstance, int32 InItemCount)
{
    if (InSlotsWidget == nullptr || InItemInstance == nullptr)
        return;
    
    SlotsWidget = InSlotsWidget;

    FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
    const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());

    // 아이템이 차지하는 슬롯 수 × 단위 슬롯 픽셀 크기 = 실제 위젯 픽셀 크기
    FVector2D WidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
    SizeBox_Root->SetWidthOverride(WidgetSize.X);
    SizeBox_Root->SetHeightOverride(WidgetSize.Y);
    
    RefreshUI(InItemInstance, InItemCount);
}
