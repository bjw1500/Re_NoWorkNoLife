// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/NoWorkActivatableWidget.h"

UNoWorkActivatableWidget::UNoWorkActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

FReply UNoWorkActivatableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (DeactivateKey.IsValid() && InKeyEvent.GetKey() == DeactivateKey && InKeyEvent.IsRepeat() == false)
	{
		DeactivateWidget();
		return FReply::Handled();
	}
	
	return Reply;
}
