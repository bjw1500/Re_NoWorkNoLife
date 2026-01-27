#pragma once

#include "NoWorkHoverWidget.h"
#include "NoWorkNoLife/Item/NoWorkItemInstance.h"
#include "NoWorkItemHoverWidget.generated.h"

// 아이템 호버(툴팁) 컨테이너 위젯.
// - 좌/우 패널에 대상 아이템과 비교 대상(장비 착용 중 아이템)을 표시한다.
// class UNoWorkEquipmentManagerComponent;
class UNoWorkItemInstance;
class UCanvasPanel;
class UHorizontalBox;
class UNoWorkItemHoverEntryWidget;

UCLASS()
class UNoWorkItemHoverWidget : public UNoWorkHoverWidget
{
	GENERATED_BODY()
	
public:
    UNoWorkItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    // 호버 대상 아이템과 비교 아이템을 찾아 양쪽 패널 UI를 갱신한다.
    void RefreshUI(const UNoWorkItemInstance* HoveredItemInstance);
	
protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNoWorkItemHoverEntryWidget> HoverWidget;
	
    // UPROPERTY(meta=(BindWidget))
    // // 왼쪽 패널(호버 대상)
    // TObjectPtr<UNoWorkItemHoverEntryWidget> HoverWidget_Left;
    //
    // UPROPERTY(meta=(BindWidget))
    // // 오른쪽 패널(비교 대상: 장착 중 아이템)
    // TObjectPtr<UNoWorkItemHoverEntryWidget> HoverWidget_Right;
};
