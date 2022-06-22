// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node_StateNode.h"
#include "State.h"
#include "K2Node_StateExit.generated.h"

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_StateExit : public UK2Node_StateNode
{
	GENERATED_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Request Exit")); }

	virtual bool IsNodeSafeToIgnore() const override { return true; }
	
	virtual bool DrawNodeAsExit() const override { return true; }

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	void AllocateOutputPins(const UState* State);

	void ExpandOutputPin(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* Pin);

	TArray<UEdGraphPin*> GetOutputPins();
};
