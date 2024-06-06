// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GioStateActivation.generated.h"

class UGioNode;

USTRUCT()
struct FGioStateActivation
{
	GENERATED_BODY()

	UPROPERTY()
	UGioNode* Node;

	UPROPERTY()
	FGuid NodeGuid{};

	TSharedPtr<class FNodeExitHandler> ExitHandler;

	bool IsValid() const { return Node != nullptr; };
};
