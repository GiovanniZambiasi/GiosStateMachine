// Copyright MiddleMast. All rights reserved

#pragma once

#include "K2Node_StateNode.h"
#include "K2Node_StateEntered.generated.h"

class UState;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_StateEntered : public UK2Node_StateNode
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("State Entered")); }

	virtual bool DrawNodeAsExit() const override { return true; }
	
	virtual bool IsNodeSafeToIgnore() const override { return true; }

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	void AllocateInputPins(const UState* State);

	TArray<UEdGraphPin*> GetInputPins();
};
