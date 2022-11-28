// Copyright MiddleMast. All rights reserved

#pragma once

#include "K2Node_GioStateNode.h"
#include "K2Node_NodeEntered.generated.h"

class UGioNode;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_NodeEntered : public UK2Node_GioStateNode
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Node Entered")); }

	virtual bool DrawNodeAsExit() const override { return true; }
	
	virtual bool IsNodeSafeToIgnore() const override { return true; }

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;

private:
	void AllocateInputPins(const UGioNode* State);

	TArray<UEdGraphPin*> GetInputPins();

	bool IsStateInputPin(const UEdGraphPin* Pin) const;
};
