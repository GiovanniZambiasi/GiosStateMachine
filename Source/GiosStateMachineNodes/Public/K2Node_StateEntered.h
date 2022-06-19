#pragma once

#include "K2Node_GameStatesNode.h"
#include "K2Node_StateEntered.generated.h"

class UState;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_StateEntered : public UK2Node_GameStatesNode
{
	GENERATED_BODY()
	
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("State Entered")); }

	virtual bool DrawNodeAsExit() const override { return true; }
	
	virtual bool IsNodeSafeToIgnore() const override { return true; }

	virtual void AllocateDefaultPins() override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	void AllocatePinsForState(const UState* State);

	UState* GetDefaultStateObject() const;

	TArray<UEdGraphPin*> GetInputPins();
};
