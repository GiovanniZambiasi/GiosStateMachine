// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "StateActivation.generated.h"

class UGioNode;

USTRUCT()
struct FStateActivation
{
	GENERATED_BODY()

	UPROPERTY()
	UGioNode* Node;

	TSharedPtr<class FNodeExitHandler> ExitHandler;

	bool IsValid() const { return Node != nullptr; };
};
