// Copyright MiddleMast. All rights reserved

#include "GioNode.h"
#include "GiosStateMachines.h"

namespace
{
	TSet<FName> ProtectedInputNames
	{
		TEXT("exec"),
	};

	TSet<FName> ProtectedOutputNames
	{
		TEXT("then"),
	};
}

void UGioNode::SetData(UGioStateMachineData* Data)
{
	StateMachineData = Data;
}

void UGioNode::Enter(const FName& Input)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entered via '%s'"), *GetName(), *Input.ToString());
	OnEntered(Input);
	K2_OnEntered(Input);
}

void UGioNode::SetInputs(const TArray<FName>& InputNames)
{
	Inputs.Empty();
	Inputs.Append(InputNames);
}

void UGioNode::SetOutputs(const TArray<FName>& OutputNames)
{
	Outputs.Empty();
	Outputs.Append(OutputNames);
}

void UGioNode::RequestExit(FName Output)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s requesting exit from %s"), *GetName(), *Output.ToString())
	ExitRequestedEvent.Broadcast(this, Output);
}

void UGioNode::RequestReturn()
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s requesting return"), *GetName())
	ReturnRequestedEvent.Broadcast(this);
}

void UGioNode::Tick(const float& DeltaTime)
{
	K2_OnTick(DeltaTime);
}

void UGioNode::OnReturned()
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s has been returned to"), *GetName())
	K2_OnReturned();
}

void UGioNode::RemoveProtectedNames(TArray<FName>& Names, const TSet<FName>& ProtectedNames)
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

void UGioNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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
