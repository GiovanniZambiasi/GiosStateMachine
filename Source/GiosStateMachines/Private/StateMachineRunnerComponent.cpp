// Copyright MiddleMast. All rights reserved

#include "StateMachineRunnerComponent.h"

#include "StateMachine.h"

UStateMachineRunnerComponent::UStateMachineRunnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	StateMachineClass = UStateMachine::StaticClass();
}

void UStateMachineRunnerComponent::BeginPlay()
{
	Super::BeginPlay();

	StateMachine = CreateStateMachine();
	checkf(StateMachine, TEXT("%s's 'CreateStateMachine' function did not create a valid state machine. Make sure to set the StateMachineClass"), *GetName())
	StateMachine->Run();

	FSoftObjectPath ResourceRef(TEXT("Blueprint'/GiosStateMachines/Tests/Transition/BP_TransitionTestStateMachine.BP_TransitionTestStateMachine'"));
	auto Resource = ResourceRef.TryLoad();
	//auto Resource = LoadObject<UStateMachine>(GetTransientPackage(), TEXT("/GiosStateMachines/Tests/Transition/BP_TransitionTestStateMachine.BP_TransitionTestStateMachine"));
	UE_LOG(LogTemp, Display, TEXT("Found resource via path?! %s"), Resource != nullptr ? TEXT("YES!") : TEXT("No :(") )
}

UStateMachine* UStateMachineRunnerComponent::CreateStateMachine()
{
	auto* Machine = NewObject<UStateMachine>(this, StateMachineClass);
	return Machine;
}

void UStateMachineRunnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(StateMachine)
	{
		StateMachine->Tick(DeltaTime);
	}
}

