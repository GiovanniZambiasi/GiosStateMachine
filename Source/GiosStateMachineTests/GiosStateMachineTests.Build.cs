// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GiosStateMachineTests : ModuleRules
{
	public GiosStateMachineTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GiosStateMachines",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",	
			}
		);
	}
}