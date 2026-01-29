#include "NoWorkItemDragWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkItemDragWidget)

UNoWorkItemDragWidget::UNoWorkItemDragWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkItemDragWidget::Init(const FVector2D& InWidgetSize, UTexture2D* InItemIcon, int32 InItemCount)
{
    // 프리뷰 위젯의 픽셀 크기 설정
    SizeBox_Root->SetWidthOverride(InWidgetSize.X);
    SizeBox_Root->SetHeightOverride(InWidgetSize.Y);
    
    // 아이콘과 수량 텍스트 갱신(1개면 숨김)
    Image_Icon->SetBrushFromTexture(InItemIcon, true);
    Text_Count->SetText((InItemCount >= 2) ? FText::AsNumber(InItemCount) : FText::GetEmpty());
}
