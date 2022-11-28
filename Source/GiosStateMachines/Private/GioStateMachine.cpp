// Copyright MiddleMast. All rights reserved

#include "GioStateMachine.h"
#include "GioNode.h"
#include "GiosStateMachines.h"

void UGioStateMachine::Enter(const FName& Input)
{
	if(GetData() == nullptr)
	{
		SetData(CreateData());
	}
	
	Super::Enter(Input);
}

void UGioStateMachine::EnterViaFirstInput()
{
	const TArray<FName>& AllInputs = GetInputs();
	if(ensureAlwaysMsgf(AllInputs.Num() > 0, TEXT("'%s' has no valid inputs"), *GetName()))
	{
		Enter(AllInputs[0]);
	}
}

void UGioStateMachine::EnterNewNode(UClass* NodeClass, FName Input, const FNodeExitHandler& ExitHandler)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entering state %s"), *GetName(), *NodeClass->GetName());
	
	if(!ensureMsgf(NodeClass->IsChildOf(UGioNode::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *NodeClass->GetName(), *UGioNode::StaticClass()->GetName()))
	{
		return;
	}

	auto* State = NewObject<UGioNode>(this, NodeClass);
	State->OnExitRequested().AddUObject(this, &ThisClass::HandleNodeExitRequest);
	State->OnReturnRequested().AddUObject(this, &ThisClass::HandleNodeReturnRequest);
	State->SetData(GetDataForNewNode());
	auto Activation = FStateActivation{State, MakeShared<FNodeExitHandler>(ExitHandler)};
	SetCurrentActivation(Activation);
	StateHistory.Add(Activation);
	ensure(State->GetInputs().Contains(Input));
	State->Enter(Input);
}

void UGioStateMachine::Tick(const float& DeltaTime)
{
	if(CurrentActivation.IsValid())
	{
		CurrentActivation.Node->Tick(DeltaTime);
	}
}

UGioStateMachineData* UGioStateMachine::CreateData()
{
	return NewObject<UGioStateMachineData>(this, DataType);
}

void UGioStateMachine::SetCurrentActivation(const FStateActivation& Activation)
{
	CurrentActivation = Activation;
}

void UGioStateMachine::HandleNodeExitRequest(UGioNode* Context, const FName& Output)
{
	if(Context != CurrentActivation.Node)
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("State exit request received from invalid context '%s'. Current state is '%s'"),
			*GetNameSafe(Context), *GetNameSafe(CurrentActivation.Node))
		return;
	}

	if(!CurrentActivation.Node->GetOutputs().Contains(Output))
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("Output '%s' not present in state '%s'"), *Output.ToString(), *GetNameSafe(CurrentActivation.Node))
		return;
	}

	LOG_GIOS_STATEMACHINES(Display, TEXT("StateMachine received exit request through %s"), *Output.ToString());
	
	auto PreviousState = CurrentActivation.Node;

	CurrentActivation.ExitHandler->Execute(Output);

	if(CurrentActivation.Node == PreviousState)
	{
		LOG_GIOS_STATEMACHINES(Warning, TEXT("State '%s' requested exit '%s', but no transition was made.\n\nIs there a transition setup for the requested output?"),
			*GetNameSafe(PreviousState), *Output.ToString())
	}
}

void UGioStateMachine::HandleNodeReturnRequest(UGioNode* Context)
{
	if(Context != CurrentActivation.Node)
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("State return request received invalid context '%s'. Current state is '%s'"),
			*GetNameSafe(Context), *GetNameSafe(CurrentActivation.Node))
		return;
	}

	check(StateHistory.Num() > 0)
	StateHistory.Pop();

	if(StateHistory.Num() == 0)
	{
		CurrentActivation = FStateActivation{};
		return;
	}
	
	FStateActivation PreviousState = StateHistory.Last();
	check(PreviousState.IsValid())
	SetCurrentActivation(PreviousState);
	PreviousState.Node->Returned();
}