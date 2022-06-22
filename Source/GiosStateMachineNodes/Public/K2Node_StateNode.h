// Copyright MiddleMast. All rights reserved

#pragma once

#include "K2Node_GiosStateMachineNode.h"
#include "K2Node_StateNode.generated.h"

class UState;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_StateNode : public UK2Node_GiosStateMachineNode
{
	GENERATED_BODY()
	
protected:
	UState* GetDefaultStateObject() const;
};
