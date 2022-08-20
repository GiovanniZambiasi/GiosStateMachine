// Copyright MiddleMast. All rights reserved

#include "State.h"

#include "GiosStateMachines.h"

namespace
{
	TSet<FName> ProtectedInputNames
	{
		TEXT("exec"),
		UState::GetReturnName(),
	};

	TSet<FName> ProtectedOutputNames
	{
		TEXT("then"),
		UState::GetReturnName(),
	};
}

void UState::SetData(UStateMachineData* Data)
{
	StateMachineData = Data;
}

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

void UState::Return()
{
	RequestExit(TEXT("Return"));
}

void UState::Tick(const float& DeltaTime)
{
}

void UState::RemoveProtectedNames(TArray<FName>& Names, const TSet<FName>& ProtectedNames)
{
	TArray<FName> RemovedNames{};
	
	Names.RemoveAll([&](FName Entry)
	{
		bool bProtected = ProtectedNames.Contains(Entry);
		
		if(bProtected)
			RemovedNames.Add(Entry);

		return bProtected;
	});

	for (int i = 0; i < RemovedNames.Num(); ++i)
	{
		auto Name = RemovedNames[i];
		LOG_GIOS_STATEMACHINES(Warning, TEXT("Protected name '%s' removed from array. Consider using a different name"), *Name.ToString())
	}
}

#if WITH_EDITOR

void UState::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const auto PropertyName = PropertyChangedEvent.GetPropertyName();
	
	if(PropertyName == TEXT("Inputs"))
	{
		RemoveProtectedNames(Inputs, ProtectedInputNames);
	}
	else if(PropertyName == TEXT("Outputs"))
	{
		RemoveProtectedNames(Outputs, ProtectedOutputNames);
	}
}

#endif
