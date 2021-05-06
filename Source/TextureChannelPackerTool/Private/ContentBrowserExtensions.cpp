// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContentBrowserExtensions.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Texture2D.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "UMG/Public/UMGStyle.h"
#include "TextureChannelPackerTool.h"
#include "TextureChannelPackerToolCommands.h"


#define LOCTEXT_NAMESPACE "TexturePacker"

DECLARE_LOG_CATEGORY_EXTERN(LogPackerCBExtensions, Log, All);
DEFINE_LOG_CATEGORY(LogPackerCBExtensions);

//////////////////////////////////////////////////////////////////////////

static FContentBrowserMenuExtender_SelectedAssets ContentBrowserExtenderDelegate;
static FDelegateHandle ContentBrowserExtenderDelegateHandle;

//////////////////////////////////////////////////////////////////////////
// FContentBrowserSelectedAssetExtensionBase

struct FContentBrowserSelectedAssetExtensionBase
{
public:
	TArray<struct FAssetData> SelectedAssets;

public:
	virtual void Execute() {}
	virtual ~FContentBrowserSelectedAssetExtensionBase() {}
};

//////////////////////////////////////////////////////////////////////////
// FCreateSpriteFromTextureExtension

#include "IAssetTools.h"
#include "AssetToolsModule.h"


struct FPackTexturesExtension : public FContentBrowserSelectedAssetExtensionBase
{
	

	FPackTexturesExtension()
		
	{
	}

	void SummonTexturePacker(TArray<UTexture2D*>& Textures)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		FTextureChannelPackerToolModule& IMTexturePackerModule = FModuleManager::LoadModuleChecked<FTextureChannelPackerToolModule>("TextureChannelPackerTool");
		TArray<UObject*> ObjectsToSync;
		
	
		IMTexturePackerModule.Texture2Ds=Textures;
		bool result=IMTexturePackerModule.PluginCommands->ExecuteAction(FTextureChannelPackerToolCommands::Get().OpenPluginWindow.ToSharedRef());
		
		
		if(result)
		{
			UE_LOG(LogTemp,Warning,TEXT("Summoned for %d textures "),Textures.Num());
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Summoned failed for %d textures "),Textures.Num());
		}
		if (ObjectsToSync.Num() > 0)
		{
			ContentBrowserModule.Get().SyncBrowserToAssets(ObjectsToSync);
		}
	}

	virtual void Execute() override
	{
		
		// Create sprites for any selected textures
		TArray<UTexture2D*> Textures;
		for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& AssetData = *AssetIt;
			if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
			{
				Textures.Add(Texture);
			}
		}

		SummonTexturePacker(Textures);
	}
};



class FPackerContentBrowserExtensions_Impl
{
public:
	static void ExecuteSelectedContentFunctor(TSharedPtr<FContentBrowserSelectedAssetExtensionBase> SelectedAssetFunctor)
	{
		SelectedAssetFunctor->Execute();
	}


	
	static void CreatePackerSubMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
	{

		TSharedPtr<FPackTexturesExtension> SpriteCreatorFunctor = MakeShareable(new FPackTexturesExtension());
		SpriteCreatorFunctor->SelectedAssets = SelectedAssets;

		FUIAction Action_Summon_Packer_Window(
            FExecuteAction::CreateStatic(&FPackerContentBrowserExtensions_Impl::ExecuteSelectedContentFunctor, StaticCastSharedPtr<FContentBrowserSelectedAssetExtensionBase>(SpriteCreatorFunctor)));
		
	
		const FName StyleSetName = FName(TEXT("UMGStyle"));
		MenuBuilder.AddMenuEntry(
            LOCTEXT("CB_Pack_Textures", "Pack Textures"),
            LOCTEXT("CB_Extension_Texture_Pack_Tooltip", "Packs diffrent Textures into an single texture's channels"),
            FSlateIcon(StyleSetName, "ClassIcon.Image"),
            Action_Summon_Packer_Window,
            NAME_None,
            EUserInterfaceActionType::Button);
	}

	
	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
	{
		TSharedRef<FExtender> Extender(new FExtender());

		// Run thru the assets to determine if any meet our criteria
		bool bAnyTextures = false;
		for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& Asset = *AssetIt;
			bAnyTextures = bAnyTextures || (Asset.AssetClass == UTexture2D::StaticClass()->GetFName());
		}

		if (bAnyTextures)
		{
			// Add the sprite actions sub-menu extender
			Extender->AddMenuExtension(
				"GetAssetActions",
				EExtensionHook::After,
				nullptr,
				FMenuExtensionDelegate::CreateStatic(&FPackerContentBrowserExtensions_Impl::CreatePackerSubMenu, SelectedAssets));
		}

		return Extender;
	}

	static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetExtenderDelegates()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		return ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	}
};



void FPackerContentBrowserExtensions::InstallHooks()
{
	ContentBrowserExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FPackerContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu);

	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FPackerContentBrowserExtensions_Impl::GetExtenderDelegates();
	CBMenuExtenderDelegates.Add(ContentBrowserExtenderDelegate);
	ContentBrowserExtenderDelegateHandle = CBMenuExtenderDelegates.Last().GetHandle();
}

void FPackerContentBrowserExtensions::RemoveHooks()
{
	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = FPackerContentBrowserExtensions_Impl::GetExtenderDelegates();
	CBMenuExtenderDelegates.RemoveAll([](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == ContentBrowserExtenderDelegateHandle; });
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
