// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "GiosStateMachineNodes.h"
#include "GiosStateMachines.h"
#include "K2Node_GiosStateMachineNode.generated.h"

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_GiosStateMachineNode : public UK2Node
{
	GENERATED_BODY()
	
public:
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	
	virtual FText GetMenuCategory() const override { return FText::FromString(GIOS_STATEMACHINES_CATEGORY); }

	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(.8f, .2f, .0f); }
};
