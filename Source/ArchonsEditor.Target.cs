// Copyright Sergei Shavrin 2024. All Rights Reserved.

using UnrealBuildTool;

public class ArchonsEditorTarget : TargetRules
{
    public ArchonsEditorTarget( TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Archons");
    }
}