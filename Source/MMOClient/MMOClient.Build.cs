// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MMOClient : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    public MMOClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "SlateCore", 
            "Sockets", "Networking", "Libprotobuf",
            "AIModule",
            "Json", "JsonUtilities", "HTTP", 
            "MyFirebasePlug", 
            /*"OnlineSubsystem", "OnlineSubsystemEOS"*/ });


        PublicIncludePaths.AddRange(new string[] {
			// ... add public include paths required here ...

			Path.Combine(ModuleDirectory, "proto")
        });

        System.Console.WriteLine("-------------" + ModuleDirectory);
        bEnableUndefinedIdentifierWarnings = false;
    }

}
