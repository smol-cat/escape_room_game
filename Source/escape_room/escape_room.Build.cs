// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class escape_room : ModuleRules
{
	public escape_room(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
