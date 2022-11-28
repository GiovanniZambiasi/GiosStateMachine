// Copyright MiddleMast. All rights reserved

#include "K2Node_NodeEntered.h"
#include "K2Node_Event.h"
#include "K2Node_SwitchName.h"
#include "KismetCompiler.h"

#include "GioNode.h"

#define LOCTEXT_NAMESPACE "UK2Node_StateEntered"

void UK2Node_NodeEntered::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	if(const auto* State = GetDefaultStateObject())
	{
	 	AllocateInputPins(State);
	}
}

void UK2Node_NodeEntered::AllocateInputPins(const UGioNode* State)
{
	auto Inputs = State->GetInputs();

	for (const auto& Input : Inputs)
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Input);
	}
}

TArray<UEdGraphPin*> UK2Node_NodeEntered::GetInputPins()
{
	TArray<UEdGraphPin*> InputPins{};

	for	(auto* Pin : this->Pins)
	{
		if(IsStateInputPin(Pin))
		{
			InputPins.Add(Pin);
		}
	}
	
	return InputPins;
}

bool UK2Node_NodeEntered::IsStateInputPin(const UEdGraphPin* Pin) const
{
	return Pin->Direction == EGPD_Output || Pin->GetFName() != UEdGraphSchema_K2::PN_Then || Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec;
}

void UK2Node_NodeEntered::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName InputPinName{ TEXT("Input") };

	const auto* Schema = Cast<UEdGraphSchema_K2>(GetSchema());
	
	auto* StateEnteredNode = CompilerContext.SpawnIntermediateNode<UK2Node_Event>(this, SourceGraph);
	StateEnteredNode->EventReference.SetExternalMember(TEXT("OnEntered"), UGioNode::StaticClass());
	StateEnteredNode->bOverrideFunction = true;
	StateEnteredNode->AllocateDefaultPins();

	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(StateEnteredNode, this);

	auto* SelectedInputPin = StateEnteredNode->FindPinChecked(InputPinName);
	
	auto InputPins = GetInputPins();

	TArray<FName> InputNames{};

	for (const auto* InputPin : InputPins)
	{
		InputNames.Add(InputPin->GetFName());
	}
	
	auto* Switcher = CompilerContext.SpawnIntermediateNode<UK2Node_SwitchName>(this, SourceGraph);
	Switcher->PinNames = InputNames;
	Switcher->bHasDefaultPin = false;
	Switcher->AllocateDefaultPins();

	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(Switcher, this);

	verify(Schema->TryCreateConnection(SelectedInputPin, Switcher->GetSelectionPin()));

	auto* StateEnteredThenPin = StateEnteredNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
	
	verify(Schema->TryCreateConnection(StateEnteredThenPin, Switcher->GetExecPin()))

	for (auto* CasePin: Switcher->Pins)
	{
		const auto CasePinName = CasePin->GetFName();
		
		if(CasePin->Direction != EGPD_Output || !InputNames.Contains(CasePinName))
		{
			continue;
		}

		auto* CorrespondingPin = *InputPins.FindByPredicate([&CasePinName](const UEdGraphPin* Element)
		{
			return Element->GetFName() == CasePinName;
		});

		check(CorrespondingPin);

		CompilerContext.MovePinLinksToIntermediate(*CorrespondingPin, *CasePin);
	}

	BreakAllNodeLinks();
}

void UK2Node_NodeEntered::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
	
	auto InputPins = Pins.FilterByPredicate([&](UEdGraphPin* Pin){ return IsStateInputPin(Pin); });
	auto StateDefault = GetDefaultStateObject();

	if(!StateDefault)
	{
		return;
	}

	auto InputNames = StateDefault->GetInputs();
	
	if(InputPins.Num() != InputNames.Num())
	{
		MessageLog.Error(*FString::Printf(TEXT("Invalid number of pins '%i'. Amount should be '%i'. Try refreshing node"), InputPins.Num(), InputNames.Num()));
		return;
	}

	for (auto InputName : InputNames)
	{
		if(!InputPins.ContainsByPredicate([&](UEdGraphPin* Pin){ return Pin->GetFName() == InputName; }))
		{
			MessageLog.Error(TEXT("Input pin not found with name '%s'. Try refreshing node"), *InputName.ToString());
			return;
		}
	}
}

#undef LOCTEXT_NAMESPACE
