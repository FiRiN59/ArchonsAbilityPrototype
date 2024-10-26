// Copyright Sergei Shavrin 2024. All Rights Reserved.

using UnrealBuildTool;

public class ArchonsTarget : TargetRules
{
    public ArchonsTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Archons");
    }
}