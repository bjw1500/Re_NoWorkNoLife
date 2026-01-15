#pragma once

#include "Blueprint/UserWidget.h"
#include "NoWorkNoLife/NoWorkDefine.h"
#include "NoWorkInventoryValidWidget.generated.h"

// 인벤토리 드래그 중 배치 가능 여부를 색상으로 표시하는 오버레이 위젯.
// - 슬롯 한 칸 크기에 맞춰 초기화된다.
// - ChangeSlotState로 Default/Invalid/Valid 상태를 전환하여 시각화한다.
class UImage;
class USizeBox;

UCLASS()
class UNoWorkInventoryValidWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UNoWorkInventoryValidWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void NativeOnInitialized() override;

public:
    // 유효 상태를 변경하여 레드/그린 이미지를 토글한다.
    void ChangeSlotState(ESlotState SlotState);
	
protected:
    UPROPERTY(meta=(BindWidget))
    // 오버레이의 실제 픽셀 크기를 제어하는 루트 SizeBox
    TObjectPtr<USizeBox> SizeBox_Root;
    
    UPROPERTY(meta=(BindWidget))
    // 배치 불가(Invalid)를 나타내는 레드 이미지
    TObjectPtr<UImage> Image_Red;

    UPROPERTY(meta=(BindWidget))
    // 배치 가능(Valid)을 나타내는 그린 이미지
    TObjectPtr<UImage> Image_Green;
};
