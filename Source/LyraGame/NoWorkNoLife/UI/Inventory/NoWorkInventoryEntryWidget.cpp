#include "NoWorkInventoryEntryWidget.h"

#include "NoWorkInventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"
#include "NoWorkNoLife/Item/Managers/NoWorkItemManagerComponent.h"
#include "NoWorkNoLife/UI/NoWorkItemDragDrop.h"
#include "NoWorkNoLife/UI/NoWorkItemDragWidget.h"
#include "NoWorkNoLife/UI/NoWorkItemHoverWidget.h"

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

FReply UNoWorkInventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
    
    // 마우스/아이템 중심 좌표를 슬롯 컨테이너 기준 로컬 좌표로 변환
    FVector2D MouseWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    FVector2D ItemWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InGeometry.LocalToAbsolute(UnitInventorySlotSize / 2.f));
    // 로컬 픽셀 좌표 → 그리드 인덱스
    FIntPoint ItemSlotPos = FIntPoint(ItemWidgetPos.X / UnitInventorySlotSize.X, ItemWidgetPos.Y / UnitInventorySlotSize.Y);
    
    CachedFromSlotPos = ItemSlotPos;
    CachedDeltaWidgetPos = MouseWidgetPos - ItemWidgetPos;
    
    // 우클릭: 퀵 무브(인벤토리 → 장비/바닥 등 서버 로직에 위임)
    if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        UNoWorkItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UNoWorkItemManagerComponent>();
        UNoWorkInventoryManagerComponent* FromInventoryManager = SlotsWidget->GetInventoryManager();

        if (ItemManager && FromInventoryManager)
        {
            ItemManager->Server_QuickFromInventory(FromInventoryManager, ItemSlotPos);
            return FReply::Handled();
        }
    }
    
    return Reply;
}

void UNoWorkInventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
    const UNoWorkItemTemplate& ItemTemplate = UNoWorkItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

    // 드래그 비주얼 생성 및 사이즈/아이콘/수량 초기화
    TSubclassOf<UNoWorkItemDragWidget> DragWidgetClass = UNoWorkUIData::Get().ItemDragWidgetClass;
    UNoWorkItemDragWidget* DragWidget = CreateWidget<UNoWorkItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
    FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
    DragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
    
    // 드래그 페이로드 생성 및 필수 필드 채움
    UNoWorkItemDragDrop* DragDrop = NewObject<UNoWorkItemDragDrop>();
    DragDrop->DefaultDragVisual = DragWidget;
    DragDrop->Pivot = EDragPivot::TopLeft;
    DragDrop->Offset = -((CachedDeltaWidgetPos + UnitInventorySlotSize / 2.f) / DragWidgetSize);
    DragDrop->FromEntryWidget = this;
    DragDrop->FromInventoryManager = SlotsWidget->GetInventoryManager();
    DragDrop->FromItemSlotPos = CachedFromSlotPos;
    DragDrop->FromItemInstance = ItemInstance;
    DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
    OutOperation = DragDrop;
}
