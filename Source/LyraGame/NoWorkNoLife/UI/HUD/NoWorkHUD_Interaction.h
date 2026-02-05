// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NoWorkNoLife/UI/NoWorkUserWidget.h"
#include "NoWorkHUD_Interaction.generated.h"

class UImage;
class UHorizontalBox;
struct FNoWorkInteractionMessage;
class UCommonVisibilitySwitcher;
class UTextBlock;
/**
 * 
 */
UCLASS()
class LYRAGAME_API UNoWorkHUD_Interaction : public UNoWorkUserWidget
{
	GENERATED_BODY()

public:

	UNoWorkHUD_Interaction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	void ShowNoticeUI(FGameplayTag Channel, const FNoWorkInteractionMessage& Message);
	void ShowProgressUI(FGameplayTag Channel, const FNoWorkInteractionMessage& Message);

public:

	UPROPERTY()
	float Duration;

	UPROPERTY()
	float StartTime;

	UPROPERTY()
	float Ratio;
	
private:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Interaction;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Notice;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title_Notice;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Progress;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title_Progress;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RemainTime_Progress;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Progress;

private:

	// GameplayMessageSubsystem 리스너 핸들.
	FGameplayMessageListenerHandle NoticeMessageListenerHandle;
	FGameplayMessageListenerHandle ProgressMessageListenerHandle;
	
	
};
