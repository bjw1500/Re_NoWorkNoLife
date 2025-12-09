#pragma once

#include "NoWorkInteractionQuery.generated.h"

// FNoWorkInteractionQuery: 상호작용 요청자(Avatar/Controller)와 선택적 부가 데이터 전달 컨테이너

USTRUCT(BlueprintType)
struct FNoWorkInteractionQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};
