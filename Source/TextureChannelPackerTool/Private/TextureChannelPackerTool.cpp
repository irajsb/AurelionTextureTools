// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TextureChannelPackerTool.h"
#include "TextureChannelPackerToolStyle.h"
#include "TextureChannelPackerToolCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SScrollBox.h"

#include "STextureChannelPackerWidget.h"

static const FName TextureChannelPackerToolTabName("TextureChannelPackerTool");

#define LOCTEXT_NAMESPACE "FTextureChannelPackerToolModule"

void FTextureChannelPackerToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTextureChannelPackerToolStyle::Initialize();
	FTextureChannelPackerToolStyle::ReloadTextures();

	FTextureChannelPackerToolCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTextureChannelPackerToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTextureChannelPackerToolModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FTextureChannelPackerToolModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	/*
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FTextureChannelPackerToolModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	*/
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TextureChannelPackerToolTabName, FOnSpawnTab::CreateRaw(this, &FTextureChannelPackerToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTextureChannelPackerToolTabTitle", "TextureChannelPackerTool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTextureChannelPackerToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FTextureChannelPackerToolStyle::Shutdown();

	FTextureChannelPackerToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TextureChannelPackerToolTabName);
}

TSharedRef<SDockTab> FTextureChannelPackerToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FTextureChannelPackerToolModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("TextureChannelPackerTool.cpp"))
		);

	// Construct toolbar widget
	TSharedRef<STextureChannelPackerWidget> TextureChannelPackerWidget =
			SNew(STextureChannelPackerWidget);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				// Put your tab content here!
				TextureChannelPackerWidget
			]
		];

/*
	SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]

*/

}

void FTextureChannelPackerToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(TextureChannelPackerToolTabName);
}

void FTextureChannelPackerToolModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTextureChannelPackerToolCommands::Get().OpenPluginWindow);
}

void FTextureChannelPackerToolModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTextureChannelPackerToolCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTextureChannelPackerToolModule, TextureChannelPackerTool)