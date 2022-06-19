﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "State.h"

#include "GiosStateMachines.h"

void UState::Enter(const FName& Input)
{
	LOG_GAME_STATES(Display, TEXT("%s entered via '%s'"), *GetName(), *Input.ToString());
	OnEntered(Input);
}

void UState::SetInputs(const TArray<FName>& InputNames)
{
	Inputs.Empty();
	Inputs.Append(InputNames);
}

void UState::SetOutputs(const TArray<FName>& OutputNames)
{
	Outputs.Empty();
	Outputs.Append(OutputNames);
}

void UState::RequestExit(const FName& Output)
{
	LOG_GAME_STATES(Display, TEXT("%s requesting exit from %s"), *GetName(), *Output.ToString())
	ExitRequestedEvent.Broadcast(Output);
}
