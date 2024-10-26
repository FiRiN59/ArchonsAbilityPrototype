// Copyright Sergei Shavrin 2024. All Rights Reserved.

using UnrealBuildTool;

public class Archons : ModuleRules
{
    public Archons(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        // AI
        PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });

        // Navigation
        PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });
    }
}