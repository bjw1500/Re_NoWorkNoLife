// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "NoWorkItemManagerComponent.generated.h"

// UItemManagerComponent
// - 서버 RPC로 인벤토리/장비 간 이동, 퀵 이동, 드롭/줍기 처리
// - AllowedComponents로 상호작용 허용 대상(다른 인벤토리/장비 UI 등) 제한

class UNoWorkItemInstance;
class UNoWorkInventoryManagerComponent;

UCLASS()
class LYRAGAME_API UNoWorkItemManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UNoWorkItemManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

    // 인벤토리 간 이동/병합(동일 인벤 포함)
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_InventoryToInventory(UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UNoWorkInventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);

public:
    // 인벤토리에서 빠른 이동(장비로 또는 내 인벤의 빈칸/병합 위치로 자동 분배)
    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_QuickFromInventory(UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

public:
    
    // // 인벤토리에서 드롭(월드로 배출)
    // UFUNCTION(Server, Reliable, BlueprintCallable)
    // void Server_DropItemFromInventory(UNoWorkInventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

public:

    // 아이템 인스턴스와 수량으로 드롭 시도(성공 시 스폰 + 원천 제거)
    // UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    // bool TryDropItem(UNoWorkItemInstance* FromItemInstance, int32 FromItemCount);

public:
    // 조작 허용 대상(소유자 인벤/장비 컴포넌트 등) 등록
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddAllowedComponent(UActorComponent* ActorComponent);

    // 조작 허용 대상 해제
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void RemoveAllowedComponent(UActorComponent* ActorComponent);
	
    // 현재 화이트리스트에 포함되는지 확인
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure)
    bool IsAllowedComponent(UActorComponent* ActorComponent) const;

protected:
    // 소유자 기준의 InventoryManager 조회(편의 함수)
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UNoWorkInventoryManagerComponent* GetMyInventoryManager() const;

private:
    // 허용된 컴포넌트 목록(서버 권한에서만 의미 있음)
    UPROPERTY()
    TArray<TWeakObjectPtr<UActorComponent>> AllowedComponents;
	
};
