// Copyright MiddleMast. All rights reserved

#include "StateMachineRunnerComponent.h"

#include "GiosStateMachines.h"
#include "GioNode.h"
#include "GioStateMachine.h"

UStateMachineRunnerComponent::UStateMachineRunnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateMachineClass = UGioStateMachine::StaticClass();
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

void UStateMachineRunnerComponent::RunStateMachine(TSubclassOf<UGioStateMachine> Class)
{
	StateMachine = NewObject<UGioStateMachine>(this, Class);
	checkf(StateMachine, TEXT("%s's 'CreateStateMachine' function did not create a valid state machine. Make sure to set the StateMachineClass"), *GetNameSafe(GetOwner()))
	StateMachine->EnterViaFirstInput();
}
