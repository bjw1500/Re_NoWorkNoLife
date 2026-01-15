#pragma once

#include "NoWorkNoLife/UI/NoWorkItemEntryWidget.h"
#include "NoWorkInventoryEntryWidget.generated.h"

// 인벤토리 내 실제 아이템을 표현하는 엔트리 위젯.
// - SlotsWidget(Canvas)에 절대 좌표로 배치된다.
// - 좌클릭 드래그로 드래그를 시작하고, 우클릭으로 빠른 이동(Quick) 요청을 보낸다.
// - 크기는 아이템 템플릿의 슬롯 크기(SlotCount) × 단위 슬롯 크기로 결정된다.
class USizeBox;
class UNoWorkItemInstance;
class UNoWorkInventorySlotsWidget;

UCLASS()
class UNoWorkInventoryEntryWidget : public UNoWorkItemEntryWidget
{
	GENERATED_BODY()
	
public:
    UNoWorkInventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    // 소유 슬롯 위젯/아이템/수량으로 엔트리 초기화. 크기와 UI를 즉시 갱신한다.
    void Init(UNoWorkInventorySlotsWidget* InSlotsWidget, UNoWorkItemInstance* InItemInstance, int32 InItemCount);

private:
    UPROPERTY()
    // 소유하고 있는 슬롯 컨테이너 위젯(좌표 변환/매니저 접근에 사용)
    TObjectPtr<UNoWorkInventorySlotsWidget> SlotsWidget;

    // 드래그 시작 시 계산해 두는 출발 슬롯 좌표(격자 인덱스)
    FIntPoint CachedFromSlotPos = FIntPoint::ZeroValue;
    // 마우스 위치와 아이템 위젯의 좌상단 사이 거리(드래그 보정용)
    FVector2D CachedDeltaWidgetPos = FVector2D::ZeroVector;
	
private:
    UPROPERTY(meta=(BindWidget))
    // 슬롯 크기에 맞게 엔트리 전체 크기를 제어하는 루트 SizeBox
    TObjectPtr<USizeBox> SizeBox_Root;
};
