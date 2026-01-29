#pragma once

#include "NoWorkUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "NoWorkItemDragWidget.generated.h"

// 드래그 중 마우스에 따라다니는 프리뷰(아이콘/수량/크기) 위젯.
// - 아이콘 텍스처와 스택 수량을 표시하며, 드래그 시작 시점에 한 번 초기화된다.
class UTexture2D;
class USizeBox;
class UImage;
class UTextBlock;

UCLASS()
class UNoWorkItemDragWidget : public UNoWorkUserWidget
{
	GENERATED_BODY()
	
public:
    UNoWorkItemDragWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
    // 드래그 프리뷰의 크기/아이콘/수량 표시를 설정한다.
    void Init(const FVector2D& InWidgetSize, UTexture2D* InItemIcon, int32 InItemCount);

protected:
    UPROPERTY(meta=(BindWidget))
    // 전체 크기를 제어하는 루트 SizeBox
    TObjectPtr<USizeBox> SizeBox_Root;

    UPROPERTY(meta=(BindWidget))
    // 아이템 아이콘 이미지
    TObjectPtr<UImage> Image_Icon;

    UPROPERTY(meta=(BindWidget))
    // 아이템 수량 텍스트(1개일 때는 비표시)
    TObjectPtr<UTextBlock> Text_Count;
};
