#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "GiosStateMachineNodes.h"
#include "K2Node_GameStatesNode.generated.h"

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_GameStatesNode : public UK2Node
{
	GENERATED_BODY()
	
public:
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	
	virtual FText GetMenuCategory() const override { return GameStates::CategoryName; }

	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(.8f, .2f, .0f); }
};
