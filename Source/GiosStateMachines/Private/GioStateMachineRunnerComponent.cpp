// Copyright MiddleMast. All rights reserved

#include "GioStateMachineRunnerComponent.h"
#include "GiosStateMachines.h"
#include "GioStateMachine.h"

UGioStateMachineRunnerComponent::UGioStateMachineRunnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateMachineClass = UGioStateMachine::StaticClass();
}

void UGioStateMachineRunnerComponent::BeginPlay()
{
	Super::BeginPlay();

	LOG_GIOS_STATEMACHINES(Display, TEXT("Component beginplay"))
	
	if(StateMachineClass != nullptr)
	{
		RunStateMachine(StateMachineClass);
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

void UGioStateMachineRunnerComponent::RunStateMachine(TSubclassOf<UGioStateMachine> Class)
{
	StateMachine = NewObject<UGioStateMachine>(this, Class);
	checkf(StateMachine, TEXT("%s's 'CreateStateMachine' function did not create a valid state machine. Make sure to set the StateMachineClass"), *GetNameSafe(GetOwner()))
	StateMachine->EnterViaFirstInput();
}
