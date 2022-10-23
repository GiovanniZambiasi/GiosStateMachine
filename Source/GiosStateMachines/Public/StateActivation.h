// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "StateActivation.generated.h"

class UState;

USTRUCT()
struct FStateActivation
{
	GENERATED_BODY()

	UPROPERTY()
	UState* State;

	TSharedPtr<class FStateExitHandler> ExitHandler;

	bool IsValid() const { return State != nullptr; };
};
