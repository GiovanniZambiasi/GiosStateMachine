// Copyright MiddleMast. All rights reserved

#include "StateMachine.h"

#include "State.h"
#include "GiosStateMachines.h"

void UStateMachine::Run()
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("State machine running"))
	OnRun();
}

void UStateMachine::EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler)
{
	if(!ensureMsgf(StateClass->IsChildOf(UState::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *StateClass->GetName(), *UState::StaticClass()->GetName()))
	{
		return;
	}

	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entering state %s"), *GetName(), *StateClass->GetName());
	
	StateExitHandler = ExitHandler;
	
	CurrentState = NewObject<UState>(this, StateClass);
	CurrentState->OnExitRequested().AddUObject(this, &ThisClass::HandleStateExitRequest);
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

void UStateMachine::HandleStateExitRequest(const FName& Output)
{
	if(!ensure(CurrentState->GetOutputs().Contains(Output)))
	{
		return;
	}

	LOG_GIOS_STATEMACHINES(Display, TEXT("StateMachine received exit request through %s"), *Output.ToString());
	
	auto* PreviousState = CurrentState;
	
	StateExitHandler.Execute(Output);

	if(CurrentState == PreviousState)
	{
		LOG_GIOS_STATEMACHINES(Warning, TEXT("State '%s' requested exit '%s', but no transition was made.\n\nIs there a transition setup for the requested output?"),
			*GetNameSafe(CurrentState), *Output.ToString())
	}
}
