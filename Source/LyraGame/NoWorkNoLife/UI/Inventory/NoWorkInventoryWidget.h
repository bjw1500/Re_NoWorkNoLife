// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NoWorkNoLife/UI/NoWorkUserWidget.h"
#include "NoWorkInventoryWidget.generated.h"

class UNoWorkInventoryManagerComponent;
class UNoWorkInventorySlotsWidget;

// UI 초기화 메시지(메시지 서브시스템으로 전달)
USTRUCT(BlueprintType)
struct FInventoryInitializeMessage
{
	GENERATED_BODY()

public:
	// UI가 시각화할 대상 인벤토리 매니저
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNoWorkInventoryManagerComponent> InventoryManager;
};

UCLASS()
class LYRAGAME_API UNoWorkInventoryWidget : public UNoWorkUserWidget
{
	GENERATED_BODY()

	UNoWorkInventoryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	virtual  void NativePreConstruct() override;
	virtual  void NativeConstruct() override;
	virtual  void NativeDestruct() override;
	void ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message);
	void DestructUI();


private:

	UPROPERTY(meta=(BindWidget))
	// 슬롯/유효표시 그리드를 함께 감싸는 컨테이너(좌표 기준).
	TObjectPtr<UNoWorkInventorySlotsWidget> InventorySlotsWidget;
	
private:

	// 초기화 메시지를 수신할 채널 태그.
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
	// GameplayMessageSubsystem 리스너 핸들.
	FGameplayMessageListenerHandle MessageListenerHandle;
};
