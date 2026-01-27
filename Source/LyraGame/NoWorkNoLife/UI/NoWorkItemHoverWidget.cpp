#include "NoWorkItemHoverWidget.h"

#include "NoWorkItemHoverEntryWidget.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
//#include "NoWorkNoLife/Item/Managers/NoWorkEquipmentManagerComponent.h"

#include "NoWorkItemHoverEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkItemHoverWidget)

UNoWorkItemHoverWidget::UNoWorkItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkItemHoverWidget::RefreshUI(const UNoWorkItemInstance* HoveredItemInstance)
{
	if (HoveredItemInstance == nullptr)
		return;

	HoverWidget->RefreshUI(HoveredItemInstance);
	
	
    // const UNoWorkItemInstance* PairItemInstance = nullptr;
    // if (UNoWorkEquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UNoWorkEquipmentManagerComponent>())
    // {
    //     EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
    //     // 호버 대상과 비교할 짝 아이템(착용 중)을 매니저에서 조회
    //     PairItemInstance = EquipmentManager->FindPairItemInstance(HoveredItemInstance, EquipmentSlotType);
    // }
    //
    // // 좌우 패널 UI 갱신
    // HoverWidget_Left->RefreshUI(HoveredItemInstance);
    // HoverWidget_Right->RefreshUI(PairItemInstance);
}
