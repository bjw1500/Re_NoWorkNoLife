// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NoWorkNoLife/UI/NoWorkUserWidget.h"
#include "NoWorkNoLife/UI/Inventory/NoWorkInventoryWidget.h"
#include "NoWorkInventorySlotsWidget.generated.h"

class UNoWorkInventoryEntryWidget;
class UNoWorkInventoryValidWidget;
class UNoWorkItemInstance;
class UNoWorkInventorySlotWidget;
class UOverlay;
class UTextBlock;
class UCanvasPanel;
class UUniformGridPanel;
class UNoWorkInventoryManagerComponent;

UCLASS()
class LYRAGAME_API UNoWorkInventorySlotsWidget : public UNoWorkUserWidget
{
	GENERATED_BODY()

public:

	UNoWorkInventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:

	// 위젯 생성 전(에디터/런타임 공통). 미리보기 텍스트 등 세팅.
	virtual void NativePreConstruct() override;
	// 위젯 생성 시(런타임). 메시지 리스너 등록 등 초기화.
	virtual void NativeConstruct() override;
	// 위젯 파괴 직전. 구독 해제 및 리소스 정리.
	virtual void NativeDestruct() override;

public:

	// 메시지 리스너에서 호출되는 UI 구성 진입점.
	void ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message);
	// UI 정리.
	void DestructUI();

	// 모든 유효 표시를 기본 상태로 초기화.
	void ResetValidSlots();

	// 특정 슬롯의 아이템/수량 변경을 UI에 반영.
	void OnInventoryEntryChanged(const FIntPoint& InItemSlotPos, UNoWorkItemInstance* InItemInstance, int32 InItemCount);

public:
	// 현재 연결된 InventoryManager 반환.
	UNoWorkInventoryManagerComponent* GetInventoryManager() const { return InventoryManager; }
	// 슬롯 컨테이너(Overlay_Slots)의 지오메트리 반환(좌표 변환에 사용).
	const FGeometry& GetSlotContainerGeometry() const;
	
public:
	
	// 인벤토리 제목 텍스트.
	UPROPERTY(EditAnywhere)
	FText TitleText;

private:
	UPROPERTY()
	// 그리드에 배치되는 슬롯(배경) 위젯들.
	TArray<TObjectPtr<UNoWorkInventorySlotWidget>> SlotWidgets;

	UPROPERTY()
	// 실제 아이템을 표시하는 엔트리 위젯들(캔버스 절대 좌표 배치).
	TArray<TObjectPtr<UNoWorkInventoryEntryWidget>> EntryWidgets;

	UPROPERTY()
	// 드래그 중 배치 가능/불가 시각화용 유효 표시(Valid) 위젯들.
	TArray<TObjectPtr<UNoWorkInventoryValidWidget>> ValidWidgets;

	UPROPERTY()
	// 이 위젯이 시각화하는 대상 인벤토리 매니저.
	TObjectPtr<UNoWorkInventoryManagerComponent> InventoryManager;
	
private:
	UPROPERTY(meta=(BindWidget))
	// 제목 텍스트 바인딩 대상.
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(meta=(BindWidget))
	// 슬롯/유효표시 그리드를 함께 감싸는 컨테이너(좌표 기준).
	TObjectPtr<UOverlay> Overlay_Slots;
	
	UPROPERTY(meta=(BindWidget))
	// 슬롯 배경 그리드.
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta=(BindWidget))
	// 아이템 엔트리를 절대 좌표로 배치하는 캔버스.
	TObjectPtr<UCanvasPanel> CanvasPanel_Entries;

	UPROPERTY(meta=(BindWidget))
	// 유효/무효 하이라이트 그리드.
	TObjectPtr<UUniformGridPanel> GridPanel_ValidSlots;
	
private:
	// 엔트리 변경 델리게이트 바인딩 핸들.
	FDelegateHandle EntryChangedDelegateHandle;
	// 직전 드래그 오버 슬롯 좌표(중복 계산 방지용).
	FIntPoint PrevDragOverSlotPos = FIntPoint(-1, -1);
	// GameplayMessageSubsystem 리스너 핸들.
	FGameplayMessageListenerHandle MessageListenerHandle;
	
};
