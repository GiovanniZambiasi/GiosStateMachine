// Fill out your copyright notice in the Description page of Project Settings.

#include "K2Node_EnterState.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "State.h"
#include "EdGraphSchema_K2.h"
#include "GiosStateMachines.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_SwitchName.h"
#include "KismetCompiler.h"
#include "StateMachine.h"

#define LOCTEXT_NAMESPACE "K2Node_EnterState"

namespace
{
	const FName ClassPinName = TEXT("State Class");
	const FName ExitNodeOutputPinName = TEXT("Output");
}

void UK2Node_EnterState::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	auto ClassPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, UState::StaticClass(), ClassPinName);
	ClassPin->bNotConnectable = true;
}

void UK2Node_EnterState::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	LOG_GIOS_STATEMACHINES(Display, TEXT("Reallocating during reconstruction for %s..."), *GetName())
	
	const auto* OldClassPin = *OldPins.FindByPredicate([](UEdGraphPin* Pin){ return Pin->GetFName() == ClassPinName; });

	if(OldClassPin)
	{
		auto* ClassPin = GetClassPin();
		ClassPin->DefaultObject = OldClassPin->DefaultObject;
	}
	
	RefreshInputAndOutputPins();
}

void UK2Node_EnterState::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin->GetFName() == ClassPinName)
	{
		LOG_GIOS_STATEMACHINES(Display, TEXT("Reconstructing from DefaultValueChanged..."))
		ReconstructNode();
	}
}

void UK2Node_EnterState::RefreshInputAndOutputPins()
{
	const auto* Class = Cast<UClass>(GetClassPin()->DefaultObject);

	LOG_GIOS_STATEMACHINES(Display, TEXT("Refreshing inputs and outputs for %s"), Class != nullptr? *Class->GetName() : TEXT("NONE"))
	
	if (Class != nullptr && Class->ClassDefaultObject != nullptr)
	{
		const auto* DefaultObject = Cast<UState>(Class->ClassDefaultObject);

		auto Outputs = DefaultObject->GetOutputs();

		for (const auto Output : Outputs)
		{
			CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Output);
		}

		auto Inputs = DefaultObject->GetInputs();

		for (const auto Input : Inputs)
		{
			CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, Input);
		}
	}
}

void UK2Node_EnterState::FindInputAndOutputPins()
{
	InputPins.Empty();
	OutputPins.Empty();
	
	for (auto* Pin : Pins)
	{
		auto PinName = Pin->GetFName();
		if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec && PinName != UEdGraphSchema_K2::PN_Execute && PinName != UEdGraphSchema_K2::PN_Then)
		{
			switch (Pin->Direction.GetValue())
			{
			case EGPD_Input:
				InputPins.Add(Pin);
				break;
			case EGPD_Output:
				OutputPins.Add(Pin);
				break;
			default: break;
			}
		}
	}
}

void UK2Node_EnterState::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	auto* ClassPin = GetClassPin();
	if(ClassPin->DefaultObject == nullptr)
	{
		CompilerContext.MessageLog.Error(TEXT("No state class selected"));
		return;
	}
	
	auto* Schema = Cast<UEdGraphSchema_K2>(GetSchema());

	auto* ExitEventNode = CreateExitEventNode(CompilerContext, SourceGraph, ClassPin->DefaultObject->GetFName());
	auto* OutputDelegatePin = ExitEventNode->FindPinChecked(UK2Node_CustomEvent::DelegateOutputName);
	
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(ExitEventNode, this);
	
	FindInputAndOutputPins();

	ExpandInputPins(CompilerContext, SourceGraph, Schema, OutputDelegatePin);
	
	ExpandOutputPins(CompilerContext, SourceGraph, Schema, ExitEventNode);

	BreakAllNodeLinks();
}

UK2Node_CustomEvent* UK2Node_EnterState::CreateExitEventNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const FName& StateClassName )
{
	auto* ExitEventNode = CompilerContext.SpawnIntermediateNode<UK2Node_CustomEvent>(this, SourceGraph);
	ExitEventNode->CustomFunctionName = FName{ FString::Printf(TEXT("On_%s_Exit_%s"), *StateClassName.ToString(), *NodeGuid.ToString()) };
	ExitEventNode->AllocateDefaultPins();
	FEdGraphPinType OutputPinType;
	OutputPinType.PinCategory = UEdGraphSchema_K2::PC_Name;
	ExitEventNode->CreateUserDefinedPin(ExitNodeOutputPinName, OutputPinType, EGPD_Output);

	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(this, ExitEventNode);
	
	return ExitEventNode;
}

void UK2Node_EnterState::ExpandInputPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const UEdGraphSchema_K2* Schema, UEdGraphPin* OutputDelegatePin)
{
	for (auto* Pin : InputPins)
	{
		ExpandInputPin(CompilerContext, SourceGraph, Pin, Schema, OutputDelegatePin);
	}
}

void UK2Node_EnterState::ExpandInputPin(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* InputPin, const UEdGraphSchema_K2* Schema, UEdGraphPin* OutputDelegatePin)
{
	static const FName StateClassParamName = TEXT("StateClass");
	static const FName InputParamName = TEXT("Input");
	static const FName StateExitRequestHandlerParamName = TEXT("ExitHandler");

	auto* CallNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	const auto FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachine, EnterState);
	const auto* Function = CompilerContext.NewClass->FindFunctionByName(FunctionName);
	
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallNode, this);
	
	if(Function == nullptr)
	{
		CompilerContext.MessageLog.Error(TEXT("No function named '%s' found in class %s. This node can only be added to %s objects"),
			*FunctionName.ToString(), *CompilerContext.NewClass->GetName(), *UStateMachine::StaticClass()->GetName());

		return;
	}
	
	CallNode->SetFromFunction(Function);
	CallNode->AllocateDefaultPins();

	auto* ClassParamPin = CallNode->FindPinChecked(StateClassParamName);
	ClassParamPin->DefaultObject = GetClassPin()->DefaultObject;
	
	auto* InputParamPin = CallNode->FindPinChecked(InputParamName);
	InputParamPin->DefaultValue = InputPin->GetName();

	auto* StateExitHandlerPin = CallNode->FindPinChecked(StateExitRequestHandlerParamName);
	verify(Schema->TryCreateConnection(StateExitHandlerPin, OutputDelegatePin));
	
	auto* ExecutePin = CallNode->GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*InputPin, *ExecutePin);
}

void UK2Node_EnterState::ExpandOutputPins(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, const UEdGraphSchema_K2* Schema, UK2Node_CustomEvent* ExitEventNode)
{
	TArray<FName> OutputPinNames{};

	for (const auto* OutputPin : OutputPins)
	{
		OutputPinNames.Add(OutputPin->GetFName());
	}
	
	auto* SwitchNode = CompilerContext.SpawnIntermediateNode<UK2Node_SwitchName>(this, SourceGraph);
	SwitchNode->bHasDefaultPin = false;
	SwitchNode->PinNames = OutputPinNames;
	SwitchNode->AllocateDefaultPins();

	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(this,SwitchNode);

	for (auto* OutputPin : OutputPins)
	{
		auto* SwitchCasePin = SwitchNode->FindPinChecked(OutputPin->GetFName());

		CompilerContext.MovePinLinksToIntermediate(*OutputPin, *SwitchCasePin);
	}

	auto* ExitNodeOutputPin = ExitEventNode->FindPinChecked(ExitNodeOutputPinName);
	auto* SwitchSelectionPin = SwitchNode->GetSelectionPin();
	verify(Schema->TryCreateConnection(ExitNodeOutputPin, SwitchSelectionPin))

	auto* ExitThenPin = ExitEventNode->FindPin(UEdGraphSchema_K2::PN_Then);
	auto* SwitchExecutePin = SwitchNode->GetExecPin();
	verify(Schema->TryCreateConnection(ExitThenPin, SwitchExecutePin))
}

UEdGraphPin* UK2Node_EnterState::GetClassPin() const
{
	return FindPinChecked(ClassPinName);
}

#undef LOCTEXT_NAMESPACE
