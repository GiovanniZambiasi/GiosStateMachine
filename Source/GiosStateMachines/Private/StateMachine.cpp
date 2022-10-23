// Copyright MiddleMast. All rights reserved

#include "StateMachine.h"
#include "State.h"
#include "GiosStateMachines.h"

void UStateMachine::Run()
{
	if(StateMachineData == nullptr)
	{
		SetData(CreateData());
	}
	
	LOG_GIOS_STATEMACHINES(Display, TEXT("State machine running"))
	
	OnRun();
}

void UStateMachine::EnterNewState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entering state %s"), *GetName(), *StateClass->GetName());
	
	if(!ensureMsgf(StateClass->IsChildOf(UState::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *StateClass->GetName(), *UState::StaticClass()->GetName()))
	{
		return;
	}

	auto* State = NewObject<UState>(this, StateClass);
	State->OnExitRequested().AddUObject(this, &ThisClass::HandleStateExitRequest);
	State->OnReturnRequested().AddUObject(this, &ThisClass::HandleStateReturnRequest);
	State->SetData(StateMachineData);
	auto Activation = FStateActivation{State, MakeShared<FStateExitHandler>(ExitHandler)};
	SetCurrentActivation(Activation);
	StateHistory.Add(Activation);
	ensure(State->GetInputs().Contains(Input));
	State->Enter(Input);
}

void UStateMachine::Tick(const float& DeltaTime)
{
	if(CurrentActivation.IsValid())
	{
		CurrentActivation.State->Tick(DeltaTime);
	}
}

void UStateMachine::SetData(UStateMachineData* Data)
{
	StateMachineData = Data;
}

UStateMachineData* UStateMachine::CreateData()
{
	return NewObject<UStateMachineData>(this, DataType);
}

void UStateMachine::SetCurrentActivation(const FStateActivation& Activation)
{
	CurrentActivation = Activation;
}

void UStateMachine::HandleStateExitRequest(UState* Context, const FName& Output)
{
	if(Context != CurrentActivation.State)
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("State exit request received from invalid context '%s'. Current state is '%s'"),
			*GetNameSafe(Context), *GetNameSafe(CurrentActivation.State))
		return;
	}

	if(!CurrentActivation.State->GetOutputs().Contains(Output))
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("Output '%s' not present in state '%s'"), *Output.ToString(), *GetNameSafe(CurrentActivation.State))
		return;
	}

	LOG_GIOS_STATEMACHINES(Display, TEXT("StateMachine received exit request through %s"), *Output.ToString());
	
	auto PreviousState = CurrentActivation.State;

	CurrentActivation.ExitHandler->Execute(Output);

	if(CurrentActivation.State == PreviousState)
	{
		LOG_GIOS_STATEMACHINES(Warning, TEXT("State '%s' requested exit '%s', but no transition was made.\n\nIs there a transition setup for the requested output?"),
			*GetNameSafe(PreviousState), *Output.ToString())
	}
}

void UStateMachine::HandleStateReturnRequest(UState* Context)
{
	if(Context != CurrentActivation.State)
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("State return request received invalid context '%s'. Current state is '%s'"),
			*GetNameSafe(Context), *GetNameSafe(CurrentActivation.State))
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
	PreviousState.State->Returned();
}