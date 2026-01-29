#pragma once

#include "NoWorkNoLife/NoWorkDefine.h"
#include "Blueprint/DragDropOperation.h"
#include "NoWorkItemDragDrop.generated.h"

// 아이템 드래그 시 전달되는 페이로드(From/To 위치, 원본 위젯, 인스턴스, 보정값 등).
// - Inventory/Equipment 간 이동, 병합, 스왑의 근거 데이터를 담는다.
class UNoWorkItemInstance;
class UNoWorkItemEntryWidget;
class UNoWorkInventoryManagerComponent;

UCLASS()
class UNoWorkItemDragDrop : public UDragDropOperation
{
    GENERATED_BODY()
    
public:
    UNoWorkItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    UPROPERTY()
    // 드래그 시작 지점의 인벤토리 매니저(인벤토리에서 끌어올 때 유효)
    TObjectPtr<UNoWorkInventoryManagerComponent> FromInventoryManager;
    
    // 시작 슬롯 좌표(인벤토리 그리드 인덱스)
    FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

public:
    UPROPERTY()
    // 드롭 대상 인벤토리 매니저
    TObjectPtr<UNoWorkInventoryManagerComponent> ToInventoryManager;
    
    // 드롭 대상 슬롯 좌표(인벤토리 그리드 인덱스)
    FIntPoint ToItemSlotPos = FIntPoint::ZeroValue;

public:
    UPROPERTY()
    // 드래그 원본 엔트리 위젯(투명도 복구 등 UI 처리에 사용)
    TObjectPtr<UNoWorkItemEntryWidget> FromEntryWidget;
    
    UPROPERTY()
    // 드래그 중인 아이템 인스턴스
    TObjectPtr<UNoWorkItemInstance> FromItemInstance;
    
    // 마우스와 엔트리 위젯 좌상단 사이 거리(드래그 비주얼 보정)
    FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
};
