#include "EngineUtils.h"
#include "GiosStateMachineTests.h"
#include "State.h"
#include "StateMachine.h"
#include "StateMachineRunnerComponent.h"
#include "Engine/AssetManager.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

BEGIN_DEFINE_SPEC(FStateMachineTests, TEXT("State Machine Tests"),
                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

UWorld* Map;

void LoadMap(const FString& MapPath)
{
	auto Result = AutomationOpenMap(MapPath);

	TestTrue(TEXT("Map loaded"), Result);
	
	const auto MapName = FPaths::GetBaseFilename(MapPath);

	auto WorldContexts = GEngine->GetWorldContexts();
	
	for	(auto WorldContext : GEngine->GetWorldContexts())
	{
		auto* World = WorldContext.World();
		
		if((WorldContext.WorldType == EWorldType::PIE || WorldContext.WorldType == EWorldType::Game) && World && World->GetName() == MapName)
		{
			Map = World;
			break;
		}
	}

	TestNotNull(TEXT("Map is not null"), Map);
}

UStateMachineRunnerComponent* FindStateMachineComponentInMap(UWorld* Level)
{
	UStateMachineRunnerComponent* Component = nullptr;
		
	for	(TActorIterator<AActor> It(Level); It; ++It)
	{
		Component = It->FindComponentByClass<UStateMachineRunnerComponent>();

		if(Component)
		{
			break;
		}
	}

	return Component;
}

END_DEFINE_SPEC(FStateMachineTests)

void FStateMachineTests::Define()
{
	It(TEXT("GivenStateMachine_WhenExitThroughA_EntersStateA"), [this]
	{
		LoadMap(TEXT("/GiosStateMachines/Tests/Transition/StateMachineTransitionTest.StateMachineTransitionTest"));
		
		auto* Component = FindStateMachineComponentInMap(Map);
		Component->ReceiveBeginPlay();
		auto* StateMachine = Component->GetStateMachine();
		
		TestNotNull(TEXT("State machine is not null"), StateMachine);
		
		StateMachine->Run();
		auto* InitialState = StateMachine->GetCurrentState();
		InitialState->RequestExit(TEXT("A"));
		
		auto* CurrentState = StateMachine->GetCurrentState();
		
		TestEqual(TEXT("State is A"), CurrentState->GetName(), FString{TEXT("BP_StateA")});
	});

	AfterEach([this]
	{
		FGioTestUtils::Exit(Map);
		Map = nullptr;
	});
}
