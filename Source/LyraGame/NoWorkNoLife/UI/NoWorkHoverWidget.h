#pragma once

#include "Blueprint/UserWidget.h"
#include "NoWorkHoverWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;

UCLASS()
class UNoWorkHoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UNoWorkHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetPosition(const FVector2D& AbsolutePosition);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Hovers;
};
