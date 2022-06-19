#include "K2Node_StateEntered.h"
#include "K2Node_Event.h"
#include "K2Node_SwitchName.h"
#include "KismetCompiler.h"

#include "State.h"

#define LOCTEXT_NAMESPACE "UK2Node_StateEntered"

void UK2Node_StateEntered::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	if(const auto* State = GetDefaultStateObject())
	{
	 	AllocatePinsForState(State);
	}
}

void UK2Node_StateEntered::AllocatePinsForState(const UState* State)
{
	auto Inputs = State->GetInputs();

	for (const auto& Input : Inputs)
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Input);
	}
}

UState* UK2Node_StateEntered::GetDefaultStateObject() const
{
	auto Class = GetBlueprint()->GeneratedClass;
	
	if(!Class)
	{
		return nullptr;
	}
	
	return Cast<UState>(Class->ClassDefaultObject);
}

TArray<UEdGraphPin*> UK2Node_StateEntered::GetInputPins()
{
	TArray<UEdGraphPin*> InputPins{};

	for	(auto* Pin : this->Pins)
	{
		if(Pin->Direction != EGPD_Output || Pin->GetFName() == UEdGraphSchema_K2::PN_Then || Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec)
		{
			continue;
		}

		InputPins.Add(Pin);
	}
	
	return InputPins;
}

void UK2Node_StateEntered::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName InputPinName{ TEXT("Input") };

	const auto* Schema = Cast<UEdGraphSchema_K2>(GetSchema());
	
	auto* StateEnteredNode = CompilerContext.SpawnIntermediateNode<UK2Node_Event>(this, SourceGraph);
	StateEnteredNode->EventReference.SetExternalMember(TEXT("OnEntered"), UState::StaticClass());
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

#undef LOCTEXT_NAMESPACE