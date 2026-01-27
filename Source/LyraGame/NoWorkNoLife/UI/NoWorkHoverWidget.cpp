#include "NoWorkHoverWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkHoverWidget)

UNoWorkHoverWidget::UNoWorkHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UNoWorkHoverWidget::SetPosition(const FVector2D& AbsolutePosition)
{
	/*
	설명
	- 목적: 마우스의 화면 좌표(AbsolutePosition) 기준으로 호버 위젯을 살짝 띄운 위치에 배치하되,
	        루트 캔버스 영역 밖으로 나가지 않도록 자동으로 보정합니다.
	- 입력: AbsolutePosition = 화면(screen/absolute) 좌표계의 마우스 위치.
	- 좌표계:
	    * Absolute(화면 좌표) -> Canvas Local(루트 캔버스의 로컬 좌표)로 변환 후 사용합니다.
	- 처리 순서:
	    1) 호버 컨테이너(HorizontalBox)의 레이아웃을 강제로 계산해 실제 표시될 크기를 얻습니다.
	    2) 루트 캔버스 패널의 크기를 가져옵니다. (0이면 아직 레이아웃이 준비되지 않은 상태이므로 리턴)
	    3) 마우스 절대좌표를 루트 캔버스 로컬 좌표로 변환하고, 마진을 더해 시작 배치 위치를 잡습니다.
	    4) 시작 위치 + 위젯 크기로 끝 점을 계산해 캔버스 밖으로 넘치는지(오버플로우) 확인합니다.
	    5) 넘친 축(X/Y)이 있으면 그 만큼 시작 위치를 되돌려 화면 안에 들어오도록 보정합니다.
	    6) 최종적으로 캔버스 패널 슬롯에 위치를 적용합니다.
	주의
	- 이 함수는 HorizontalBox_Hovers가 캔버스 패널의 직접 자식(슬롯 타입: UCanvasPanelSlot)일 것을 가정합니다.
	- 레이아웃/지오메트리 정보는 프레임별로 달라질 수 있으므로, 틱 혹은 입력 이벤트 시점 호출을 가정합니다.
	*/

	// 1) 먼저 자식 위젯들의 레이아웃 계산을 강제 수행해 원하는 크기(DesiredSize)를 최신화합니다.
	HorizontalBox_Hovers->ForceLayoutPrepass();

	// 호버 컨테이너의 실제 그려질 예상 크기(DesiredSize)를 얻습니다.
	FVector2D HoversWidgetSize = HorizontalBox_Hovers->GetDesiredSize();

	// 2) 루트 캔버스의 로컬 사이즈(가시 영역 크기)를 가져옵니다.
	FVector2D CanvasWidgetSize = CanvasPanel_Root->GetCachedGeometry().GetLocalSize();
	if (CanvasWidgetSize.IsZero())
		return; // 아직 레이아웃이 계산되지 않은 초기 프레임 등에서는 안전하게 종료합니다.
	
	// 3) 마우스 위치 주변에 여백을 두고 띄워 보이게 할 마진 값입니다.
	FVector2D Margin = FVector2D(10.f, 15.f);

	// Absolute(화면) 좌표를 루트 캔버스의 로컬 좌표로 변환합니다.
	FVector2D MouseWidgetPos = CanvasPanel_Root->GetCachedGeometry().AbsoluteToLocal(AbsolutePosition);

	// 시작 배치 위치 = 마우스 위치 + 마진
	FVector2D HoverWidgetStartPos = MouseWidgetPos + Margin;

	// 끝 점(우하단) = 시작 위치 + 위젯 크기
	FVector2D HoverWidgetEndPos = HoverWidgetStartPos + HoversWidgetSize;

	// 4) 캔버스 사이즈를 넘어서는 정도(오버플로우)를 계산합니다. 양수면 넘친 것입니다.
	FVector2D OutSize = (HoverWidgetEndPos - CanvasWidgetSize);

	// 5) X축으로 넘치면 넘친 만큼 왼쪽으로 이동시켜 보정합니다.
	if (OutSize.X > 0.f)
	{
		HoverWidgetStartPos.X -= OutSize.X;
	}
	// Y축으로 넘치면 넘친 만큼 위로 이동시켜 보정합니다.
	if (OutSize.Y > 0.f)
	{
		HoverWidgetStartPos.Y -= OutSize.Y;
	}
		
	// 6) 실제 캔버스 패널 슬롯(UCanvasPanelSlot)에 최종 위치를 적용합니다.
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(HorizontalBox_Hovers->Slot))
	{
		CanvasPanelSlot->SetPosition(HoverWidgetStartPos);
	}
}
