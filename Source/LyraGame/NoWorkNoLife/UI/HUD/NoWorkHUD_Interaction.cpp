// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/HUD/NoWorkHUD_Interaction.h"

#include "CommonVisibilitySwitcher.h"
#include "K2Node_FormatText.h"
#include "LyraGameplayTags.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "NoWorkNoLife/Interaction/NoWorkInteractable.h"
#include "NoWorkNoLife/Interaction/Abilities/NoWorkGameplayAbility_Interact.h"

#define LOCTEXT_NAMESPACE "NoWorkHUD_Interaction"

UNoWorkHUD_Interaction::UNoWorkHUD_Interaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UNoWorkHUD_Interaction::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Notice
	//Interact 표시 할 때
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	NoticeMessageListenerHandle = MessageSubsystem.RegisterListener(LyraGameplayTags::Message_Interaction_Notice, this, &ThisClass::ShowNoticeUI);
	
	//Progress
	//Interact 진행 중일 때
	ProgressMessageListenerHandle = MessageSubsystem.RegisterListener(LyraGameplayTags::Message_Interaction_Progress, this, &ThisClass::ShowProgressUI);
}

void UNoWorkHUD_Interaction::NativeConstruct()
{
	Super::NativeConstruct();

	HorizontalBox_Notice->SetVisibility(ESlateVisibility::Collapsed);
	Image_Progress->GetDynamicMaterial()->SetScalarParameterValue("Percentage", 0.0f);
	
}

void UNoWorkHUD_Interaction::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float EndTime = StartTime + Duration;
	float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
	
	if (EndTime > CurrentTime)
	{
		float ProgressTime = CurrentTime - StartTime;
		float RemainTime = Duration - ProgressTime;
		Ratio = ProgressTime/Duration;

		FText Format = FText::Format(LOCTEXT("Interaction RemainTime", "{0}초"), FText::AsNumber(RemainTime));
		Text_RemainTime_Progress->SetText(Format);
		Image_Progress->GetDynamicMaterial()->SetScalarParameterValue("Percentage", Ratio);
	}
}

void UNoWorkHUD_Interaction::ShowNoticeUI(FGameplayTag Channel, const FNoWorkInteractionMessage& Message)
{
	bool bShouldRefresh = Message.bShouldRefresh;
	if (bShouldRefresh == true)
	{
		if (Message.InteractionInfo.Interactable)
		{
			FText InteractionTitle = Message.InteractionInfo.Title;
			FText InteractionContent = Message.InteractionInfo.Content;

			//@usage FText::Format(LOCTEXT("PlayerNameFmt", "{0} is really cool"), FText::FromString(PlayerName));
			FText FormatText = FText::Format(LOCTEXT("Interaction Title", "{0} {1}"), InteractionTitle, InteractionContent);

			Text_Title_Notice->SetText(FormatText);
			Text_Title_Progress->SetText(FormatText);

			HorizontalBox_Notice->SetVisibility(ESlateVisibility::HitTestInvisible);
			
		}
		else
		{
			HorizontalBox_Notice->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	
	bool bSwitchActive = Message.bSwitchActive;
	if (bSwitchActive == true)
	{
		Switcher_Interaction->SetActiveWidget(HorizontalBox_Notice);
	}
	
}

void UNoWorkHUD_Interaction::ShowProgressUI(FGameplayTag Channel, const FNoWorkInteractionMessage& Message)
{
	bool bShouldRefresh = Message.bShouldRefresh;
	if (bShouldRefresh == true)
	{
		Duration = Message.InteractionInfo.Duration;
		StartTime = UGameplayStatics::GetTimeSeconds(this);
	}

	bool bSwitchActive = Message.bSwitchActive;
	if (bSwitchActive == true)
	{
		Switcher_Interaction->SetActiveWidget(HorizontalBox_Progress);
	}
}
