// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MyFirebasePlug : ModuleRules
{
	public MyFirebasePlug(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		

        PublicSystemLibraries.Add("ws2_32.lib");
		PublicSystemLibraries.Add("wldap32.lib");
		PublicSystemLibraries.Add("crypt32.lib");
		PublicSystemLibraries.Add("userenv.lib");
		PublicSystemLibraries.Add("iphlpapi.lib");
		PublicSystemLibraries.Add("kernel32.lib");
		PublicSystemLibraries.Add("psapi.lib");
		PublicSystemLibraries.Add("dbghelp.lib");
		PublicSystemLibraries.Add("bcrypt.lib");
		

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ModuleDirectory, "include"),
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_analytics.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_app.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_app_check.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_auth.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_database.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_dynamic_links.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_firestore.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_functions.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_gma.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_installations.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_messaging.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_remote_config.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "firebase_storage.lib"));
    }
}
