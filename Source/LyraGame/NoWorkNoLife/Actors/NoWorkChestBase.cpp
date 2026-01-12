#include "NoWorkNoLife/Actors/NoWorkChestBase.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "NoWorkNoLife/Data/NoWorkItemData.h"
#include "NoWorkNoLife/Item/NoWorkItemTemplate.h"

#include "NoWorkNoLife/Item//Managers/NoWorkInventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkChestBase)

// 메쉬/인벤토리 컴포넌트 구성 및 충돌 프로파일 설정
ANoWorkChestBase::ANoWorkChestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 구성요소 생성 및 기본 설정: 틱 비활성, 루트/메쉬/인벤토리 컴포넌트 구성
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);
	
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComponent->SetCanEverAffectNavigation(true);
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;

	InventoryManager = CreateDefaultSubobject<UNoWorkInventoryManagerComponent>(TEXT("Inventory Manager"));
}

// 서버에서 시작 시 전리품 규칙에 따라 인벤토리를 초기화
void ANoWorkChestBase::BeginPlay()
{
	// 서버에서 초기 전리품 규칙(FItemAddRule)을 평가해 인벤토리를 채운다.
	// 규칙 확률에 따라 무기/방어구/커스텀 템플릿과 희귀도 1개를 선택, 실패 시 Fallback 처리.
	Super::BeginPlay();

	if (HasAuthority() == false)
		return;

	const TArray<TSubclassOf<UNoWorkItemTemplate>>& WeaponItemTemplateClasses = UNoWorkItemData::Get().GetWeaponItemTemplateClasses();
	const TArray<TSubclassOf<UNoWorkItemTemplate>>& ArmorItemTemplateClasses = UNoWorkItemData::Get().GetArmorItemTemplateClasses();

	bool bItemAdded = false;

	for (const FItemAddRule& ItemAddRule : ItemAddRules)
	{
		if (ItemAddRule.ItemAddType == EItemAddType::None)
			continue;
	
		if (FMath::RandRange(0.f, 100.f) > ItemAddRule.ItemAddTypeRate)
			continue;
		
		const TArray<TSubclassOf<UNoWorkItemTemplate>>* SelectedItemTemplateClasses = nullptr;
		
		switch (ItemAddRule.ItemAddType)
		{
		case EItemAddType::Weapon:	SelectedItemTemplateClasses = &WeaponItemTemplateClasses;				break;
		case EItemAddType::Armor:	SelectedItemTemplateClasses = &ArmorItemTemplateClasses;				break;
		case EItemAddType::Custom:	SelectedItemTemplateClasses = &ItemAddRule.CustomItemTemplateClasses;	break;
		}
	
		if (SelectedItemTemplateClasses)
		{
			int32 SelectedItemTemplateIndex = FMath::RandRange(0, SelectedItemTemplateClasses->Num() - 1);
			TSubclassOf<UNoWorkItemTemplate> SelectedItemTemplateClass = (*SelectedItemTemplateClasses)[SelectedItemTemplateIndex];
			
			int32 SelectedItemRarityIndex = FMath::RandRange(0, ItemAddRule.ItemRarities.Num() - 1);
			EItemRarity SelectedItemRarity = ItemAddRule.ItemRarities[SelectedItemRarityIndex];
			
			InventoryManager->TryAddItemByRarity(SelectedItemTemplateClass, SelectedItemRarity, 1);
			bItemAdded = true;
		}
	}
}

void ANoWorkChestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ChestState(열림/닫힘)을 복제하여 모든 클라이언트에서 상태를 동기화
	DOREPLIFETIME(ThisClass, ChestState);
}

// 상태에 따른 상호작용 정보 선택(열림/닫힘)
FNoWorkInteractionInfo ANoWorkChestBase::GetPreInteractionInfo(const FNoWorkInteractionQuery& InteractionQuery) const
{
	// 현재 ChestState에 맞는 상호작용 안내(열림/닫힘)를 반환(클라이언트 UI 표시용)
	switch (ChestState)
	{
	case EChestState::Open:		return OpenedInteractionInfo;
	case EChestState::Close:	return ClosedInteractionInfo;
	default:					return FNoWorkInteractionInfo();
	}
}

void ANoWorkChestBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	OutMeshComponents.Add(MeshComponent);
}

void ANoWorkChestBase::SetChestState(EChestState NewChestState)
{
	// 서버 전용: 중복 호출/동일 상태 전환 방지 후 상태 변경하고 OnRep로 비주얼 갱신
	if (HasAuthority() == false || NewChestState == ChestState)
		return;

	ChestState = NewChestState;
	OnRep_ChestState();
}

// 상태 변경 시 대응 몽타주 재생
void ANoWorkChestBase::OnRep_ChestState()
{
	// 상태 변화에 따라 열기/닫기 몽타주 재생(서버/클라 공통)
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		UAnimMontage* SelectedMontage = nullptr;
		
		switch (ChestState)
		{
		case EChestState::Open:		SelectedMontage = OpenMontage;	break;
		case EChestState::Close:	SelectedMontage = CloseMontage; break;
		}

		if (SelectedMontage)
		{
			AnimInstance->Montage_Play(SelectedMontage);
		}
	}
}
