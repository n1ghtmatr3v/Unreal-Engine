// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdPersonGame : ModuleRules
{
	public ThirdPersonGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ThirdPersonGame",
			"ThirdPersonGame/Variant_Platforming",
			"ThirdPersonGame/Variant_Platforming/Animation",
			"ThirdPersonGame/Variant_Combat",
			"ThirdPersonGame/Variant_Combat/AI",
			"ThirdPersonGame/Variant_Combat/Animation",
			"ThirdPersonGame/Variant_Combat/Gameplay",
			"ThirdPersonGame/Variant_Combat/Interfaces",
			"ThirdPersonGame/Variant_Combat/UI",
			"ThirdPersonGame/Variant_SideScrolling",
			"ThirdPersonGame/Variant_SideScrolling/AI",
			"ThirdPersonGame/Variant_SideScrolling/Gameplay",
			"ThirdPersonGame/Variant_SideScrolling/Interfaces",
			"ThirdPersonGame/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
