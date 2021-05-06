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
#include "ContentBrowserExtensions.h"
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
	
		FPackerContentBrowserExtensions::InstallHooks();

	
	
	FGlobalTabmanager::Get()->RegisterTabSpawner(TextureChannelPackerToolTabName, FOnSpawnTab::CreateRaw(this, &FTextureChannelPackerToolModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTextureChannelPackerToolTabTitle", "Texture Channel Packer Tool"))
		.SetMenuType(ETabSpawnerMenuType::Disabled);
	
	;
}

void FTextureChannelPackerToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FTextureChannelPackerToolStyle::Shutdown();

	FTextureChannelPackerToolCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TextureChannelPackerToolTabName);
	FPackerContentBrowserExtensions::RemoveHooks();
}

TSharedRef<SDockTab> FTextureChannelPackerToolModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	

	// Construct toolbar widget
	TSharedRef<STextureChannelPackerWidget> TextureChannelPackerWidget =
			SNew(STextureChannelPackerWidget).Textures(Texture2Ds);
			Texture2Ds.Empty();
	
	TSharedRef<SDockTab>DockTab= SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.ContentPadding(FMargin(2))
	
	[
      
       		SNew(SScrollBox)
            			+SScrollBox::Slot()
            			[
            				// Put your tab content here!
            				TextureChannelPackerWidget
            			]
   ];

	return  DockTab;
}

void FTextureChannelPackerToolModule::PluginButtonClicked()
{

#if ENGINE_MINOR_VERSION >25
	FGlobalTabmanager::Get()->TryInvokeTab(TextureChannelPackerToolTabName);
#else
	FGlobalTabmanager::Get()->InvokeTab(TextureChannelPackerToolTabName);
#endif
	
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