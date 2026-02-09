#pragma once

#include "EnhancedPlayerInput.h"
#include "NoWorkEnhancedPlayerInput.generated.h"

UCLASS()
class UNoWorkEnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()
	
public:
	UNoWorkEnhancedPlayerInput();

public:
	void FlushPressedInput(UInputAction* InputAction);
	bool IsActionKeyDown(const UInputAction* InputAction);
};
