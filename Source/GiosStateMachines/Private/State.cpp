// Copyright MiddleMast. All rights reserved

#include "State.h"

#include "GiosStateMachines.h"

void UState::Enter(const FName& Input)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entered via '%s'"), *GetName(), *Input.ToString());
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

void UState::RequestExit(FName Output)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s requesting exit from %s"), *GetName(), *Output.ToString())
	ExitRequestedEvent.Broadcast(Output);
}

void UState::Tick(const float& DeltaTime)
{
}
