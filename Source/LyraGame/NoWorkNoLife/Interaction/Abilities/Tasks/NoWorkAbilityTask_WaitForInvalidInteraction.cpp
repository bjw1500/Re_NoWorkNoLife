#include "NoWorkAbilityTask_WaitForInvalidInteraction.h"

#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NoWorkAbilityTask_WaitForInvalidInteraction)

// 허용 각도/거리 기준으로 상호작용의 유효 상태를 감시하는 태스크
UNoWorkAbilityTask_WaitForInvalidInteraction::UNoWorkAbilityTask_WaitForInvalidInteraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UNoWorkAbilityTask_WaitForInvalidInteraction* UNoWorkAbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance)
{
	UNoWorkAbilityTask_WaitForInvalidInteraction* Task = NewAbilityTask<UNoWorkAbilityTask_WaitForInvalidInteraction>(OwningAbility);
	Task->AcceptanceAngle = AcceptanceAngle;
	Task->AcceptanceDistance = AcceptanceDistance;
	return Task;
}

// 아바타 준비 및 기준(전방/위치) 캐싱 → 주기 검사 시작
void UNoWorkAbilityTask_WaitForInvalidInteraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	CachedCharacterForward2D = GetAvatarActor() ? GetAvatarActor()->GetActorForwardVector().GetSafeNormal2D() : FVector::ZeroVector;
	CachedCharacterLocation = GetAvatarActor() ? GetAvatarActor()->GetActorLocation() : FVector::ZeroVector;

	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::PerformCheck, 0.05f, true);
}

// 검사 타이머 정리
void UNoWorkAbilityTask_WaitForInvalidInteraction::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

// 각도/거리 허용 범위를 벗어나면 OnInvalidInteraction 브로드캐스트 후 종료
void UNoWorkAbilityTask_WaitForInvalidInteraction::PerformCheck()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(Ability->GetCurrentActorInfo()->AvatarActor.Get());
	UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement();

	if (LyraCharacter && CharacterMovement)
	{
		bool bValidAngle2D = CalculateAngle2D() <= AcceptanceAngle;
		bool bValidDistanceXY = FVector::DistSquared2D(CachedCharacterLocation, LyraCharacter->GetActorLocation()) <= (AcceptanceDistance * AcceptanceDistance);
		bool bValidDistanceZ = FMath::Abs(CachedCharacterLocation.Z - LyraCharacter->GetActorLocation().Z) <= (AcceptanceDistance + FMath::Abs(CharacterMovement->GetCrouchedHalfHeight() - LyraCharacter->BaseUnscaledCapsuleHalfHeight));

		if (bValidAngle2D && bValidDistanceXY && bValidDistanceZ)
			return;
	}

	OnInvalidInteraction.Broadcast();
	EndTask();
}

// 초기 전방과 현재 전방의 2D 각도(도) 계산
float UNoWorkAbilityTask_WaitForInvalidInteraction::CalculateAngle2D() const
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	if (AvatarActor && PlayerController)
	{
		FVector CharacterForward2D = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
		return UKismetMathLibrary::DegAcos(CachedCharacterForward2D.Dot(CharacterForward2D));
	}
	
	return 0.f;
}
