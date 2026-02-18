// WorldOfEmpires.Build.cs
// Этот файл написан на C# (да, именно C#, не C++).
// Он нужен только для настройки сборки, игровой код пишется на C++.

using UnrealBuildTool;

public class WorldOfEmpires : ModuleRules
{
    public WorldOfEmpires(ReadOnlyTargetRules Target) : base(Target)
    {
        // PCHUsage — настройка предкомпилированных заголовков.
        // UseExplicitOrSharedPCHs — самый надёжный вариант.
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Список модулей UE, от которых зависит наша игра.
        // Каждый модуль — это "библиотека" внутри UE.
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",              // Базовые типы (строки, массивы, математика)
            "CoreUObject",       // Система объектов UE (UObject, рефлексия)
            "Engine",            // Движок (AActor, ACharacter, компоненты)
            "InputCore",         // Базовая обработка ввода (клавиши, мышь)
            "EnhancedInput",     // Новая система ввода UE5 (используем её)
            "GameplayTags",      // Система тегов (удобно для типов предметов)
            "UMG",               // Виджеты UI (меню, инвентарь)
            "NetCore",           // Сетевая подсистема
        });

        // Приватные зависимости — модули, которые нужны только
        // в .cpp файлах (не видны другим модулям, если наш модуль
        // будет кем-то подключен).
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",             // Низкоуровневый UI
            "SlateCore",         // Ядро Slate
        });
    }
}