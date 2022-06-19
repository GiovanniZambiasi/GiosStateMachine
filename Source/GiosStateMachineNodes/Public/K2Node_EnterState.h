// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_GameStatesNode.h"
#include "K2Node_EnterState.generated.h"

class UEdGraphSchema_K2;
class UK2Node_CustomEvent;

UCLASS()
class GIOSSTATEMACHINENODES_API UK2Node_EnterState : public UK2Node_GameStatesNode
{
	GENERATED_BODY()

	TArray<UEdGraphPin*> InputPins;
	
	TArray<UEdGraphPin*> OutputPins;
	
public:
	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("Enter State")); }

	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual bool IsNodeSafeToIgnore() const override { return true; }
	
private:
	void RefreshInputAndOutputPins();

	void FindInputAndOutputPins();

	void ExpandInputPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const UEdGraphSchema_K2* Schema, UEdGraphPin* OutputDelegatePin);
	
	void ExpandInputPin(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* InputPin, const UEdGraphSchema_K2* Schema, UEdGraphPin* OutputDelegatePin);
	
	void ExpandOutputPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const UEdGraphSchema_K2* Schema, UK2Node_CustomEvent* ExitEventNode);
	
	UK2Node_CustomEvent* CreateExitEventNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const FName& StateClassName );
	
	UEdGraphPin* GetClassPin() const;
};
