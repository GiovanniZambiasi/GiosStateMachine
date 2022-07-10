#include "EngineUtils.h"
#include "GiosStateMachineTests.h"
#include "State.h"
#include "StateMachine.h"
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

		for (auto WorldContext : GEngine->GetWorldContexts())
		{
			auto* World = WorldContext.World();

			if ((WorldContext.WorldType == EWorldType::PIE || WorldContext.WorldType == EWorldType::Game) && World &&
				World->GetName() == MapName)
			{
				Map = World;
				break;
			}
		}

		TestNotNull(TEXT("Map is not null"), Map);
	}

	UStateMachine* LoadStateMachineResource(const FString& Path) const
	{
		const FSoftObjectPath ResourceRef(Path);
		auto Resource = ResourceRef.TryLoad();

		return FGioTestUtils::CastResource<UStateMachine>(Resource);
	}

END_DEFINE_SPEC(FStateMachineTests)

const FString TransitionTestStateMachinePath = TEXT("Blueprint'/GiosStateMachines/Tests/Transition/BP_TransitionTestStateMachine.BP_TransitionTestStateMachine'");

void FStateMachineTests::Define()
{
	BeforeEach([this]
	{
		LoadMap(FGioTestUtils::EmptyTestSceneName);
	});

	It(TEXT("GivenStateMachine_WhenExitThroughA_EntersStateA"), [this]
	{
		auto* StateMachineResource = LoadStateMachineResource(TransitionTestStateMachinePath);

		if (!TestNotNull(TEXT("State machine resource found"), StateMachineResource))
			return;

		auto* StateMachine = NewObject<UStateMachine>(StateMachineResource);

		if (TestNotNull(TEXT("State machine is not null"), StateMachine))
			return;

		StateMachine->Run();

		auto* InitialState = StateMachine->GetCurrentState();
		InitialState->RequestExit(TEXT("A"));

		auto* CurrentState = StateMachine->GetCurrentState();

		TestEqual(TEXT("State is A"), CurrentState->GetName(), FString{TEXT("BP_StateA")});
	});

	It(TEXT("GivenStateMachine_WhenExitThroughB_EntersStateB"), [this]
	{
		auto* StateMachineResource = LoadStateMachineResource(TransitionTestStateMachinePath);

		if (!TestNotNull(TEXT("State machine resource found"), StateMachineResource))
			return;

		auto* StateMachine = NewObject<UStateMachine>(StateMachineResource);

		if (TestNotNull(TEXT("State machine is not null"), StateMachine))
			return;

		StateMachine->Run();

		auto* InitialState = StateMachine->GetCurrentState();
		InitialState->RequestExit(TEXT("B"));

		auto* CurrentState = StateMachine->GetCurrentState();

		TestEqual(TEXT("State is B"), CurrentState->GetName(), FString{TEXT("BP_StateB")});
	});

	AfterEach([this]
	{
		FGioTestUtils::Exit(Map);
		Map = nullptr;
	});
}
