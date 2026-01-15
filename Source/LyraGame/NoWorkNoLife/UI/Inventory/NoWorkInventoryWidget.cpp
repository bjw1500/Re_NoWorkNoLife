// Fill out your copyright notice in the Description page of Project Settings.


#include "NoWorkNoLife/UI/Inventory/NoWorkInventoryWidget.h"

#include "NoWorkInventorySlotsWidget.h"

UNoWorkInventoryWidget::UNoWorkInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNoWorkInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UNoWorkInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화 메시지를 받을 리스너 등록
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UNoWorkInventoryWidget::NativeDestruct()
{
	DestructUI();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	// 메시지 리스너 해제
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
	
	Super::NativeDestruct();
}

void UNoWorkInventoryWidget::ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message)
{
	if (InventorySlotsWidget == nullptr)
		return;

	InventorySlotsWidget->ConstructUI(Channel, Message);
}


void UNoWorkInventoryWidget::DestructUI()
{
	if (InventorySlotsWidget == nullptr)
		return;

	InventorySlotsWidget->DestructUI();
}
