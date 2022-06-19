// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine.h"

#include "State.h"
#include "GiosStateMachines.h"

void UStateMachine::Begin()
{
	LOG_GAME_STATES(Display, TEXT("State machine begin"))
	OnBegin();
}

void UStateMachine::EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler)
{
	if(!ensureMsgf(StateClass->IsChildOf(UState::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *StateClass->GetName(), *UState::StaticClass()->GetName()))
	{
		return;
	}

	LOG_GAME_STATES(Display, TEXT("%s entering state %s"), *GetName(), *StateClass->GetName());
	
	StateExitHandler = ExitHandler;
	
	CurrentState = NewObject<UState>(this, StateClass);
	CurrentState->OnExitRequested().AddUObject(this, &ThisClass::HandleStateExitRequest);
	CurrentState->Enter(Input);
}

void UStateMachine::HandleStateExitRequest(const FName& Output)
{
	LOG_GAME_STATES(Display, TEXT("StateMachine received exit request through %s"), *Output.ToString());
	StateExitHandler.Execute(Output);
}
