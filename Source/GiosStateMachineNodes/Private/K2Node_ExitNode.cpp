// Copyright MiddleMast. All rights reserved


#include "K2Node_ExitNode.h"

#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "UK2Node_StateExit"

void UK2Node_ExitNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	if(const auto* State = GetDefaultStateObject())
	{
		AllocateOutputPins(State);
	}
}

void UK2Node_ExitNode::AllocateOutputPins(const UGioNode* State)
{
	auto Outputs = State->GetOutputs();

	for (const auto& Output : Outputs)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, Output);
	}
}

TArray<UEdGraphPin*> UK2Node_ExitNode::GetOutputPins()
{
	TArray<UEdGraphPin*> Outputs{};

	for	(auto* Pin : this->Pins)
	{
		if(IsStateOutputPin(Pin))
		{
			Outputs.Add(Pin);
		}
	}

	return Outputs;
}

bool UK2Node_ExitNode::IsStateOutputPin(const UEdGraphPin* Pin) const
{
	return Pin->Direction == EGPD_Input || Pin->GetFName() != UEdGraphSchema_K2::PN_Execute || Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec;
}

void UK2Node_ExitNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	auto OutputPins = GetOutputPins();

	for	(auto* Pin : OutputPins)
	{
		ExpandOutputPin(CompilerContext, SourceGraph, Pin);
	}

	BreakAllNodeLinks();
}

void UK2Node_ExitNode::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	auto OutputPins = Pins.FilterByPredicate([&](UEdGraphPin* Pin){ return IsStateOutputPin(Pin); });
	auto StateDefault = GetDefaultStateObject();

	if(!StateDefault)
	{
		return;
	}
	
	auto OutputNames = StateDefault->GetOutputs();
	
	if(OutputPins.Num() != OutputNames.Num())
	{
		MessageLog.Error(*FString::Printf(TEXT("Invalid number of pins '%d'. Amount should be '%d'. Try refreshing node"), OutputPins.Num(), OutputNames.Num()));
		return;
	}

	for (auto OutputName : OutputNames)
	{
		if(!OutputPins.ContainsByPredicate([&](UEdGraphPin* Pin){ return Pin->GetFName() == OutputName; }))
		{
			MessageLog.Error(TEXT("Output pin not found with name '%s'. Try refreshing node"), *OutputName.ToString());
			return;
		}
	}
}

void UK2Node_ExitNode::ExpandOutputPin(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* Pin)
{
	UK2Node_CallFunction* CallNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	const auto FunctionName = GET_FUNCTION_NAME_CHECKED(UGioNode, RequestExit);
	const auto* Function = CompilerContext.NewClass->FindFunctionByName(FunctionName);

	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallNode, this);

	if(Function == nullptr)
	{
		CompilerContext.MessageLog.Error(TEXT("No function named '%s' found in class %s. This node can only be added to %s objects"),
			*FunctionName.ToString(), *CompilerContext.NewClass->GetName(), *UGioNode::StaticClass()->GetName());

		return;
	}
	
	CallNode->SetFromFunction(Function);
	CallNode->AllocateDefaultPins();

	auto* ExecPin = CallNode->GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*Pin, *ExecPin);

	auto* OutputParameterPin = CallNode->FindPinChecked(TEXT("Output"));
	OutputParameterPin->DefaultValue = Pin->GetFName().ToString();
}

#undef LOCTEXT_NAMESPACE
