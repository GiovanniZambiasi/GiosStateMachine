// Copyright MiddleMast. All rights reserved

#include "GioStateMachine.h"
#include "GioNode.h"
#include "GiosStateMachines.h"
#include "Kismet/KismetGuidLibrary.h"

void UGioStateMachine::EnterViaFirstInput()
{
	const TArray<FName>& AllInputs = GetInputs();
	if(ensureAlwaysMsgf(AllInputs.Num() > 0, TEXT("'%s' has no valid inputs"), *GetName()))
	{
		Enter(AllInputs[0]);
	}
}

void UGioStateMachine::Tick(const float& DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(CurrentActivation.IsValid())
	{
		CurrentActivation.Node->Tick(DeltaTime);
	}
}

void UGioStateMachine::OnEntered(const FName& Input)
{
	Super::OnEntered(Input);
	
	if(GetData() == nullptr)
	{
		SetData(CreateData());
	}
}

void UGioStateMachine::EnterNewNode(UClass* NodeClass, FName Input, FString NodeGuid, const FNodeExitHandler& ExitHandler)
{
	LOG_GIOS_STATEMACHINES(Display, TEXT("%s entering state %s [GUID:%s]"), *GetName(), *NodeClass->GetName(), *NodeGuid);
	
	if(!ensureMsgf(NodeClass->IsChildOf(UGioNode::StaticClass()), TEXT("'%s' is not a subclass of '%s'. Cannot enter state"), *NodeClass->GetName(), *UGioNode::StaticClass()->GetName()))
	{
		return;
	}

	FGuid ParsedGuid{};
	bool bResult{};
	UKismetGuidLibrary::Parse_StringToGuid(NodeGuid, ParsedGuid, bResult);
	check(bResult)
	UGioNode* Node = FindOrMakeNode(NodeClass);
	auto Activation = FGioStateActivation{Node, ParsedGuid, MakeShared<FNodeExitHandler>(ExitHandler)};
	SetCurrentActivation(Activation);
	StateHistory.Add(Activation);
	ensure(Node->GetInputs().Contains(Input));
	Node->Enter(Input);
}

UGioStateMachineData* UGioStateMachine::CreateData()
{
	return NewObject<UGioStateMachineData>(this, DataType);
}

void UGioStateMachine::SetCurrentActivation(const FGioStateActivation& Activation)
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

	UGioNode* PreviousState = CurrentActivation.Node;

	CurrentActivation.ExitHandler->ExecuteIfBound(Output);

	PreviousState->OnExited(Output);
	
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
		CurrentActivation = FGioStateActivation{};
		return;
	}
	
	FGioStateActivation PreviousState = StateHistory.Last();
	check(PreviousState.IsValid())
	SetCurrentActivation(PreviousState);
	PreviousState.Node->OnReturned();
}

UGioNode* UGioStateMachine::FindOrMakeNode(const UClass* NodeClass)
{
	if(UGioNode** Node = NodePool.Find(NodeClass))
	{
		return *Node;
	}

	UGioNode* Node = NewObject<UGioNode>(this, NodeClass);
	Node->OnExitRequested().AddUObject(this, &ThisClass::HandleNodeExitRequest);
	Node->OnReturnRequested().AddUObject(this, &ThisClass::HandleNodeReturnRequest);
	Node->SetData(GetDataForNewNode());
	NodePool.Add(NodeClass, Node);
	return Node;
}
