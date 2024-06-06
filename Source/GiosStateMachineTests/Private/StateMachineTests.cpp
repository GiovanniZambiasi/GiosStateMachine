#include "EngineUtils.h"
#include "GiosStateMachineTests.h"
#include "GioNode.h"
#include "GioStateMachine.h"
#include "GioStateMachineRunnerComponent.h"
#include "Engine/AssetManager.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

BEGIN_DEFINE_SPEC(FStateMachineTests, TEXT("GioStateMachine"),
                  EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

	UWorld* Map;
	UGioStateMachineRunnerComponent* StateMachineRunner;

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

	UGioStateMachine* LoadStateMachineResource(const FString& Path) const
	{
		const TSoftObjectPtr<UBlueprint> ResourceRef(Path);
		auto Resource = ResourceRef.LoadSynchronous();

		return FGioTestUtils::CastResource<UGioStateMachine>(Resource);
	}

	bool LoadAndRunStateMachine(const FString& ResourcePath)
	{
		auto* StateMachineResource = LoadStateMachineResource(ResourcePath);

		if (TestNotNull(TEXT("State machine resource is not null"), StateMachineResource))
		{
			StateMachineRunner->RunStateMachine(StateMachineResource->GetClass());
			return true;
		}

		return false;
	}

END_DEFINE_SPEC(FStateMachineTests)

const FString TransitionTestStateMachinePath = TEXT(
	"Blueprint'/GiosStateMachines/Tests/Transition/BP_TransitionTestStateMachine.BP_TransitionTestStateMachine'");

const FString NestedStateMachinePath = TEXT(
	"Blueprint'/GiosStateMachines/Tests/NestedSM/BP_NestedSMTest.BP_NestedSMTest'");

void FStateMachineTests::Define()
{
	BeforeEach([this]
	{
		LoadMap(FGioTestUtils::EmptyTestSceneName);

		auto* Actor = Map->SpawnActor<AActor>();
		StateMachineRunner = Cast<UGioStateMachineRunnerComponent>(
			Actor->AddComponentByClass(UGioStateMachineRunnerComponent::StaticClass(), false, FTransform{}, true));
	});

	It(TEXT("WhenExitThroughA_EntersStateA"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("A"));

		auto* CurrentState = StateMachine->GetCurrentNode();

		TestEqual(TEXT("State is A"), CurrentState->GetClass()->GetName(), FString{TEXT("BP_StateA_C")});
	});

	It(TEXT("WhenExitThroughB_EntersStateB"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("B"));

		auto* CurrentState = StateMachine->GetCurrentNode();

		TestEqual(TEXT("State is B"), CurrentState->GetClass()->GetName(), FString{TEXT("BP_StateB_C")});
	});

	It(TEXT("WhenExitThroughInvalidOutput_LogsWarning"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		AddExpectedError(TEXT("Output 'C' not present"), EAutomationExpectedErrorFlags::Contains);

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("C"));
	});

	It(TEXT("WhenRan_CreatesDataObject"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		TestNotNull(TEXT("StateMachineData is not null"), StateMachine->GetData());
	});

	It(TEXT("WhenRan_SetDataOnState"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();

		TestNotNull(TEXT("StateMachineData is not null"), InitialState->GetData());
	});

	It(TEXT("WhenTransitions_SetDataOnNewState"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("A"));

		auto* NewState = StateMachine->GetCurrentNode();

		TestNotNull(TEXT("StateMachineData is not null"), NewState->GetData());
	});

	It(TEXT("WhenInvalidContextRequestsExiit_ErrorIsLogged"), [this]
		{
			if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
			{
				return;
			}
		
			AddExpectedError(TEXT("invalid context"), EAutomationExpectedErrorFlags::Contains);
		
			auto* StateMachine = StateMachineRunner->GetStateMachine();

			auto* InitialState = StateMachine->GetCurrentNode();
			InitialState->RequestExit(TEXT("A"));
			InitialState->RequestExit(TEXT("A"));
		});
	
	It(TEXT("WhenStateReturned_ReturnsToPreviousState"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("A"));

		auto* NewState = StateMachine->GetCurrentNode();
		NewState->RequestReturn();

		TestEqual(TEXT("Current state is InitialState"), StateMachine->GetCurrentNode(), InitialState);
	});

	It(TEXT("WhenInitialStateReturns_StateMachineStops"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestReturn();

		TestFalse(TEXT("StateMachine is running"), StateMachine->IsRunning());
	});

	It(TEXT("WhenInvalidContextRequestsReturn_ErrorIsLogged"), [this]
	{
		if (!LoadAndRunStateMachine(TransitionTestStateMachinePath))
		{
			return;
		}

		AddExpectedError(TEXT("invalid context"), EAutomationExpectedErrorFlags::Contains);

		auto* StateMachine = StateMachineRunner->GetStateMachine();

		auto* InitialState = StateMachine->GetCurrentNode();
		InitialState->RequestExit(TEXT("A"));

		InitialState->RequestReturn();
	});

	It(TEXT("WhenNestedStateMachineEntered_StateMachineDataIsSet"), [this]
	{
		if (!LoadAndRunStateMachine(NestedStateMachinePath))
		{
			return;
		}

		UGioStateMachine* StateMachine = StateMachineRunner->GetStateMachine();
		UGioStateMachineData* OriginalData = StateMachine->GetData();
		UGioStateMachine* InnerStateMachine = Cast<UGioStateMachine>(StateMachine->GetCurrentNode());

		if(!TestNotNull(TEXT("Inner state machine entered"), InnerStateMachine))
			return;

		UGioStateMachineData* InnerData = InnerStateMachine->GetData();
		TestTrue(TEXT("Data is the same"), OriginalData != nullptr && OriginalData == InnerData);
	});
	
	AfterEach([this]
	{
		if(StateMachineRunner && Map)
		{
			Map->DestroyActor(StateMachineRunner->GetOwner());
			StateMachineRunner = nullptr;
		}
	});
}
