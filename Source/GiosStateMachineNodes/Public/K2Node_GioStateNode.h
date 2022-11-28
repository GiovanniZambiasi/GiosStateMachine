// Copyright MiddleMast. All rights reserved

#pragma once

#include "K2Node_GiosStateMachineNode.h"
#include "K2Node_GioStateNode.generated.h"

class UGioNode;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_GioStateNode : public UK2Node_GiosStateMachineNode
{
	GENERATED_BODY()
	
protected:
	UGioNode* GetDefaultStateObject() const;
};
