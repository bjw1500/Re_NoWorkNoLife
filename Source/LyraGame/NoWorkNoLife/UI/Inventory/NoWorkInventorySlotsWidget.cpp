// Fill out your copyright notice in the Description page of Project Settings.

#include "NoWorkNoLife/UI/Inventory/NoWorkInventorySlotsWidget.h"

#include "NoWorkInventoryEntryWidget.h"
#include "NoWorkInventoryValidWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"

#include "NoWorkNoLife/UI/Inventory/NoWorkInventorySlotWidget.h"

#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "NoWorkNoLife/Data/NoWorkUIData.h"

#include "NoWorkNoLife/Item/Managers/NoWorkInventoryManagerComponent.h"

UNoWorkInventorySlotsWidget::UNoWorkInventorySlotsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 인벤토리 기본 제목 초기값 설정
	TitleText = FText::FromString(TEXT("Title"));
}

void UNoWorkInventorySlotsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터/런타임 공통: 제목 텍스트 반영
	Text_Title->SetText(TitleText);
}

void UNoWorkInventorySlotsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UNoWorkInventorySlotsWidget::NativeDestruct()
{
	// 구독 해제/자식 위젯 정리
	DestructUI();
    
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	// 메시지 리스너 해제
	MessageSubsystem.UnregisterListener(MessageListenerHandle);
    
	Super::NativeDestruct();
}

void UNoWorkInventorySlotsWidget::ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message)
{
	// 유효한 매니저가 없으면 무시
    if (Message.InventoryManager == nullptr)
        return;
    
    // 대상 인벤토리 매니저 연결
    InventoryManager = Message.InventoryManager;
    
    // 슬롯 개수/배열 크기 준비
    const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
    const int32 InventorySlotNum = InventorySlotCount.X * InventorySlotCount.Y;
    SlotWidgets.SetNum(InventorySlotNum);
    ValidWidgets.SetNum(InventorySlotNum);
    EntryWidgets.SetNum(InventorySlotNum);
    
    // 슬롯/유효표시 그리드를 생성하여 각 위치에 배치
    for (int32 y = 0; y < InventorySlotCount.Y; y++)
    {
        for (int32 x = 0; x < InventorySlotCount.X; x++)
        {
            // 슬롯 배경 위젯 생성 및 그리드 추가
            TSubclassOf<UNoWorkInventorySlotWidget> SlotWidgetClass = UNoWorkUIData::Get().InventorySlotWidgetClass;
            UNoWorkInventorySlotWidget* SlotWidget = CreateWidget<UNoWorkInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
            SlotWidgets[y * InventorySlotCount.X + x] = SlotWidget;
            GridPanel_Slots->AddChildToUniformGrid(SlotWidget, y, x);

            // 유효/무효 하이라이트 위젯 생성 및 그리드 추가
            TSubclassOf<UNoWorkInventoryValidWidget> ValidWidgetClass = UNoWorkUIData::Get().InventoryValidWidgetClass;
            UNoWorkInventoryValidWidget* ValidWidget = CreateWidget<UNoWorkInventoryValidWidget>(GetOwningPlayer(), ValidWidgetClass);
            ValidWidgets[y * InventorySlotCount.X + x] = ValidWidget;
            GridPanel_ValidSlots->AddChildToUniformGrid(ValidWidget, y, x);
        }
    }
    
     // 초기 인벤토리 엔트리를 순회하여 엔트리 위젯 배치
     const TArray<FNoWorkInventoryEntry>& Entries = InventoryManager->GetAllEntries();
     for (int32 i = 0; i < Entries.Num(); i++)
     {
         const FNoWorkInventoryEntry& Entry = Entries[i];
         if (UNoWorkItemInstance* ItemInstance = Entry.GetItemInstance())
         {
         	//선형으로 되어 있는 데이터 배열을 2차원 형태의 좌표로 변환한다.
         	//나머지 값 -> X 좌표
         	//나눗셈 값 -> Y 좌표
             FIntPoint ItemSlotPos = FIntPoint(i % InventorySlotCount.X, i / InventorySlotCount.X);
             OnInventoryEntryChanged(ItemSlotPos, ItemInstance, Entry.GetItemCount());
         }
     }
     //변경 이벤트 구독: 이후 변경 사항을 즉시 UI에 반영
    EntryChangedDelegateHandle = InventoryManager->OnInventoryEntryChanged.AddUObject(this, &ThisClass::OnInventoryEntryChanged);
}

void UNoWorkInventorySlotsWidget::DestructUI()
{
	// 변경 이벤트 구독 해제
	InventoryManager->OnInventoryEntryChanged.Remove(EntryChangedDelegateHandle);
	EntryChangedDelegateHandle.Reset();
	
	// 아이템 엔트리 위젯 정리
	CanvasPanel_Entries->ClearChildren();
	EntryWidgets.Reset();
    
	// 슬롯/유효표시 그리드 정리
	GridPanel_Slots->ClearChildren();
	SlotWidgets.Reset();
}

void UNoWorkInventorySlotsWidget::ResetValidSlots()
{
	for (UNoWorkInventoryValidWidget* ValidWidget : ValidWidgets)
	{
		if (ValidWidget)
		{
			// 하이라이트 기본 상태로 되돌리기
			ValidWidget->ChangeSlotState(ESlotState::Default);
		}
	}
}

void UNoWorkInventorySlotsWidget::OnInventoryEntryChanged(const FIntPoint& InItemSlotPos,
	UNoWorkItemInstance* InItemInstance, int32 InItemCount)
{
	// 슬록 좌표 → 선형 인덱스로 변환
	const FIntPoint& InventorySlotCount = InventoryManager->GetInventorySlotCount();
	int32 SlotIndex = InItemSlotPos.Y * InventorySlotCount.X + InItemSlotPos.X;

	// 기존에 위젯이 있으면 동일 인스턴스인지 확인
	if (UNoWorkInventoryEntryWidget* EntryWidget = EntryWidgets[SlotIndex])
	{
		UNoWorkItemInstance* ItemInstance = EntryWidget->GetItemInstance();
		if (ItemInstance && ItemInstance == InItemInstance)
		{
			// 같은 인스턴스면 수량만 갱신
			EntryWidget->RefreshItemCount(InItemCount);
			return;
		}
        
		// 다른 인스턴스면 기존 위젯 제거 후 신규 배치
		CanvasPanel_Entries->RemoveChild(EntryWidget);
		EntryWidgets[SlotIndex] = nullptr;
	}
    
	if (InItemInstance)
	{
		// 슬롯 한 칸의 픽셀 크기(캔버스 배치에 사용)
		FIntPoint UnitInventorySlotSize = UNoWorkUIData::Get().UnitInventorySlotSize;
        
		// 아이템 엔트리 위젯 생성 및 보관
		TSubclassOf<UNoWorkInventoryEntryWidget> EntryWidgetClass = UNoWorkUIData::Get().InventoryEntryWidgetClass;
		UNoWorkInventoryEntryWidget* EntryWidget = CreateWidget<UNoWorkInventoryEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[SlotIndex] = EntryWidget;
        
		// 위젯 초기화(소유 슬롯/아이템/수량)
		EntryWidget->Init(this, InItemInstance, InItemCount);
        
		// 캔버스에 추가하고 슬롯 좌표 기준으로 위치 설정
		UCanvasPanelSlot* CanvasPanelSlot = CanvasPanel_Entries->AddChildToCanvas(EntryWidget);
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(FVector2D(InItemSlotPos.X * UnitInventorySlotSize.X, InItemSlotPos.Y * UnitInventorySlotSize.Y));
	}
}

const FGeometry& UNoWorkInventorySlotsWidget::GetSlotContainerGeometry() const
{
	// 드래그 좌표 변환 기준(Overlay의 지오메트리 사용)
	return Overlay_Slots->GetCachedGeometry();
}
