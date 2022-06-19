// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GiosStateMachineNodes : ModuleRules
{
	public GiosStateMachineNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"GiosStateMachines",
				"BlueprintGraph",
				"KismetCompiler",
				"UnrealEd"
			}
			);
	}
}
