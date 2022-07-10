// Copyright MiddleMast. All rights reserved

#include "GiosStateMachineTests.h"

#include "Kismet/GameplayStatics.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, GiosStateMachineTests)void FGioTestUtils::Exit(UWorld* World)
{
	if (APlayerController* TargetPC = UGameplayStatics::GetPlayerController(World, 0))
	{
		TargetPC->ConsoleCommand(TEXT("Exit"), true);
	}
}

const FString FGioTestUtils::EmptyTestSceneName{TEXT("/GiosStateMachines/Tests/Transition/StateMachinetest")};