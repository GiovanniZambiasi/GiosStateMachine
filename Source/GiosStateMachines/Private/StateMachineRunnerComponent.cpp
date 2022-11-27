// Copyright MiddleMast. All rights reserved

#include "StateMachineRunnerComponent.h"

#include "GiosStateMachines.h"
#include "State.h"
#include "StateMachine.h"

UStateMachineRunnerComponent::UStateMachineRunnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateMachineClass = UStateMachine::StaticClass();
}

void UStateMachineRunnerComponent::BeginPlay()
{
	Super::BeginPlay();

	LOG_GIOS_STATEMACHINES(Display, TEXT("Component beginplay"))
	
	if(StateMachineClass != nullptr)
	{
		RunStateMachine(StateMachineClass);
	}
}

void UStateMachineRunnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(StateMachine)
	{
		StateMachine->Tick(DeltaTime);
	}
}

void UStateMachineRunnerComponent::RunStateMachine(TSubclassOf<UStateMachine> Class)
{
	StateMachine = NewObject<UStateMachine>(this, Class);
	checkf(StateMachine, TEXT("%s's 'CreateStateMachine' function did not create a valid state machine. Make sure to set the StateMachineClass"), *GetNameSafe(GetOwner()))
	StateMachine->EnterViaFirstInput();
}
