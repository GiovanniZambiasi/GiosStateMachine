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

void UStateMachine::EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entering state %s"), *GetName(), *StateClass->GetName());
	
	if(!ensureMsgf(StateClass->IsChildOf(UState::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *StateClass->GetName(), *UState::StaticClass()->GetName()))
	{
		return;
	}

	StateExitHandler = ExitHandler;
	
	CurrentState = NewObject<UState>(this, StateClass);
	CurrentState->OnExitRequested().AddUObject(this, &ThisClass::HandleStateExitRequest);
	CurrentState->SetData(StateMachineData);
	
	ensure(CurrentState->GetInputs().Contains(Input));
	CurrentState->Enter(Input);
}

void UStateMachine::Tick(const float& DeltaTime)
{
	if(CurrentState)
	{
		CurrentState->Tick(DeltaTime);
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

void UStateMachine::HandleStateExitRequest(const FName& Output)
{
	if(!CurrentState->GetOutputs().Contains(Output))
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("Output '%s' not present in state '%s'"), *Output.ToString(), *GetNameSafe(CurrentState))
		return;
	}

	LOG_GIOS_STATEMACHINES(Display, TEXT("StateMachine received exit request through %s"), *Output.ToString());
	
	auto PreviousState = CurrentState;
	
	StateExitHandler.Execute(Output);

	if(CurrentState == PreviousState)
	{
		LOG_GIOS_STATEMACHINES(Warning, TEXT("State '%s' requested exit '%s', but no transition was made.\n\nIs there a transition setup for the requested output?"),
			*GetNameSafe(CurrentState), *Output.ToString())
	}
}