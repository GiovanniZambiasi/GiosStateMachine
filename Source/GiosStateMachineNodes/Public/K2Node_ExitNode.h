// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node_GioStateNode.h"
#include "GioNode.h"
#include "K2Node_ExitNode.generated.h"

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_ExitNode : public UK2Node_GioStateNode
{
	GENERATED_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Exit Node")); }

	virtual bool IsNodeSafeToIgnore() const override { return true; }
	
	virtual bool DrawNodeAsExit() const override { return true; }

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;
	
private:
	void AllocateOutputPins(const UGioNode* State);

	void ExpandOutputPin(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* Pin);

	TArray<UEdGraphPin*> GetOutputPins();

	bool IsStateOutputPin(const UEdGraphPin* Pin) const;
};
