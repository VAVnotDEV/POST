// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class POST : ModuleRules
{
    public POST(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
