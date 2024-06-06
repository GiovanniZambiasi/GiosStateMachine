// Copyright MiddleMast. All rights reserved

#include "GioStateMachineRunnerComponent.h"
#include "GiosStateMachines.h"
#include "GioStateMachine.h"

UGioStateMachineRunnerComponent::UGioStateMachineRunnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateMachineClass = UGioStateMachine::StaticClass();
}

void UGioStateMachineRunnerComponent::CreateStateMachine(TSubclassOf<UGioStateMachine> Class)
{
	StateMachine = NewObject<UGioStateMachine>(this, Class);
	checkf(StateMachine, TEXT("%s's 'CreateStateMachine' function did not create a valid state machine. Make sure to set the StateMachineClass"), *GetNameSafe(GetOwner()))
}

void UGioStateMachineRunnerComponent::RunStateMachine()
{
	if(!StateMachine)
	{
		LOG_GIOS_STATEMACHINES(Error, TEXT("%s tried to run StateMachine, but it's invalid"), *GetName())
		return;
	}

	StateMachine->EnterViaFirstInput();
}

void UGioStateMachineRunnerComponent::BeginPlay()
{
	Super::BeginPlay();

	LOG_GIOS_STATEMACHINES(Display, TEXT("Component beginplay"))
	
	if(StateMachineClass == nullptr)
	{
		return;
	}

	if(bAutoRun)
	{
		CreateAndRunStateMachine(StateMachineClass);
	}
	else
	{
		CreateStateMachine(StateMachineClass);
	}
}

void UGioStateMachineRunnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(StateMachine)
	{
		StateMachine->Tick(DeltaTime);
	}
}

void UGioStateMachineRunnerComponent::CreateAndRunStateMachine(TSubclassOf<UGioStateMachine> Class)
{
	CreateStateMachine(Class);
	RunStateMachine();
}
