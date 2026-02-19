// WorldOfEmpires.Build.cs
// This file is written in C# (yes, C#, not C++).
// It's only needed for build configuration, game code is written in C++.

using UnrealBuildTool;

public class WorldOfEmpires : ModuleRules
{
    public WorldOfEmpires(ReadOnlyTargetRules Target) : base(Target)
    {
        // PCHUsage - precompiled headers setting.
        // UseExplicitOrSharedPCHs - most reliable option.
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // List of UE modules our game depends on.
        // Each module is a "library" inside UE.
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",              // Basic types (strings, arrays, math)
            "CoreUObject",       // UE object system (UObject, reflection)
            "Engine",            // Engine (AActor, ACharacter, components)
            "InputCore",         // Basic input handling (keys, mouse)
            "EnhancedInput",     // New UE5 input system (we use it)
            "GameplayTags",      // Tag system (useful for item types)
            "UMG",               // UI widgets (menu, inventory)
            "NetCore",           // Network subsystem
        });

        // Private dependencies - modules needed only
        // in .cpp files (not visible to other modules if our module
        // is used elsewhere).
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",             // Low-level UI
            "SlateCore",         // Slate core
        });
    }
}
