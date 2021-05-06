// Fill out your copyright notice in the Description page of Project Settings.

#include "STextureChannelPackerWidget.h"
#include "SlateOptMacros.h"
#include "LevelEditor.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "IDetailsView.h"
#include "AssetRegistryModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Factories/TextureFactory.h"
#include "Materials/MaterialInterface.h"

//#include "UObjectGlobals.h"

#define LOCTEXT_NAMESPACE "STextureChannelPackerWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void STextureChannelPackerWidget::Construct(const FArguments& InArgs)
{
    CreateSettingsView();

    TextureChannelPackerSettings = UTextureChannelPackerSettingsObject::Get();
    SettingsView->SetObject(TextureChannelPackerSettings);



	ChildSlot
	[
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .Padding(0, 0, 0, 0)
        [
            SettingsView->AsShared()
        ]

        +SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .Padding(0, 0, 0, 0)
        [
            SNew(STextBlock)
            .Text_Lambda([this]() -> FText { return GetErrorMessage();})
        ]

        +SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Bottom)
        .HAlign(HAlign_Right)
        .Padding(0, 0, 0, 0)
        [
            SNew(SButton)
            .Text(LOCTEXT("PackChannels", "Pack Texture Channels"))
            .OnClicked(this, &STextureChannelPackerWidget::OnPackTextureChannelsClicked)
            .IsEnabled_Lambda([this]() -> bool { return this->CanPackTextureChannels();})
        ]
	];


}

FReply STextureChannelPackerWidget::OnPackTextureChannelsClicked()
{

    const FString defaultAssetName = FPackageName::FilenameToLongPackageName(FPaths::ProjectContentDir() + TEXT("NewPackedTexture"));

    if(defaultAssetName.Len() > 0)
    {
        const FString defaultPath = FPackageName::GetLongPackagePath(defaultAssetName);
        const FString defaultName = FPackageName::GetShortName(defaultAssetName);

        //initialize save file dialog
        FSaveAssetDialogConfig saveAssetDialogConfig;
        saveAssetDialogConfig.DialogTitleOverride = LOCTEXT("CreatePackedChannelTexture", "Create Packed Channel Texture");
        saveAssetDialogConfig.DefaultPath = defaultPath;
        saveAssetDialogConfig.DefaultAssetName = defaultName;
        saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

        FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
        FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog(saveAssetDialogConfig);

        if(!saveObjectPath.IsEmpty())
        {
            const FString packageName = FPackageName::ObjectPathToPackageName(saveObjectPath);
            const FString longPackagePath = FPackageName::GetLongPackagePath(saveObjectPath);
            const FString filenameToLongPackageName= FPackageName::FilenameToLongPackageName(saveObjectPath);

            UE_LOG(LogTemp, Log, TEXT( "saveObjectPath: %s" ), *saveObjectPath);
            UE_LOG(LogTemp, Log, TEXT( "longPackagePath: %s" ), *longPackagePath);

            UE_LOG(LogTemp, Log, TEXT( "filenameToLongPackageName: %s" ), *filenameToLongPackageName);
            UE_LOG(LogTemp, Log, TEXT( "Package name: %s" ), *packageName);

            CreatePackedTexture(packageName);
            //create the texture at saveObjectPath here
        }
        else
        {
            //create the texture at defaultAssetName here
        }


    }

    return FReply::Handled();
}

bool STextureChannelPackerWidget::CanPackTextureChannels()
{
    if(TextureChannelPackerSettings != nullptr)
    {
        if(ValidTargetTextureSize()
        && TextureSlotsAsigned()
        && TexturesArePropperSizeForTarget())
            return true;
    }
    return false;
}
bool STextureChannelPackerWidget::IsPowerOfTwo(int num)
{
    int n =num;
    if(n>0)
    {
        while(n%2 == 0)
        {
            n/=2;
        }
        if(n == 1)
        {
            return true;
        }
    }
    if(n == 0 || n != 1)
    {
      return false;
    }
    return false;
}

FText STextureChannelPackerWidget::GetErrorMessage()
{    FText WidgetHelpText = LOCTEXT("WidgetReadyText", "Ready to package");

    if(!ValidTargetTextureSize())
    {
        WidgetHelpText = LOCTEXT("WidgetBatTargetSizeText", "Target texture size must be power of two ");
    }
    else if(!TexturesArePropperSizeForTarget())
    {
        WidgetHelpText = LOCTEXT("WidgetNullTextureInSlotErrorText", "You must assign a texture to each slot");
    }
    else if(!TexturesArePropperSizeForTarget()) {
        WidgetHelpText = LOCTEXT("WidgetTextureSizeErrorText", "Source textures must be equal size than target texture");
    }
    return WidgetHelpText;
}

bool STextureChannelPackerWidget::ValidTargetTextureSize()
{
   
   return IsPowerOfTwo(TextureChannelPackerSettings->Settings.targetTextureSize);
    
}

bool STextureChannelPackerWidget::TextureSlotsAsigned()
{
    if(TextureChannelPackerSettings != nullptr)
    {
        
                return true;
    }
    return false;
}

bool STextureChannelPackerWidget::TexturesArePropperSizeForTarget()
{
    bool Result=true;
    TArray<UTexture2D*>Channels;
   
    if(TextureChannelPackerSettings != nullptr && TextureSlotsAsigned()) {

        if(TextureChannelPackerSettings->Settings.aChannelTargetSlot)
            Channels.Add(TextureChannelPackerSettings->Settings.aChannelTargetSlot);
        if(TextureChannelPackerSettings->Settings.rChannelTargetSlot)
            Channels.Add(TextureChannelPackerSettings->Settings.rChannelTargetSlot);
        if(TextureChannelPackerSettings->Settings.gChannelTargetSlot)
            Channels.Add(TextureChannelPackerSettings->Settings.gChannelTargetSlot);
        if(TextureChannelPackerSettings->Settings.bChannelTargetSlot)
            Channels.Add(TextureChannelPackerSettings->Settings.bChannelTargetSlot);

        for(UTexture2D* Channel : Channels)
        {
        if(    Channel->PlatformData->SizeX!=Channel->PlatformData->SizeY||!IsPowerOfTwo(Channel->PlatformData->SizeX))
            Result=false;
        }
       
    }
    return Result;
}

void STextureChannelPackerWidget::CreatePackedTexture(const FString& filePath)
{
    if(CanPackTextureChannels())
    {
        FString FileName = FPackageName::GetShortName(*filePath);

        UE_LOG(LogTemp, Log, TEXT( "FileName: %s" ), *FileName);

        int width = TextureChannelPackerSettings->Settings.targetTextureSize;
        int height = TextureChannelPackerSettings->Settings.targetTextureSize;

        FTextureChannelPackerSettings Settings=TextureChannelPackerSettings->Settings;
         UTexture2D*  NullTexture= UTexture2D::CreateTransient(Settings.targetTextureSize,Settings.targetTextureSize);
        
        
        UTexture2D * rSourceTexture = Settings.rChannelTargetSlot?Settings.rChannelTargetSlot:NullTexture;
        UTexture2D * gSourceTexture = Settings.gChannelTargetSlot?Settings.gChannelTargetSlot:NullTexture;
        UTexture2D * bSourceTexture = Settings.bChannelTargetSlot?Settings.bChannelTargetSlot:NullTexture;
        UTexture2D * aSourceTexture = Settings.aChannelTargetSlot?Settings.aChannelTargetSlot:NullTexture;
        
        
        //set temporal texture settings for accesing textures
       const TextureCompressionSettings OldCompressionSettings_R = rSourceTexture->CompressionSettings;
       const TextureCompressionSettings OldCompressionSettings_G = gSourceTexture->CompressionSettings;
      const  TextureCompressionSettings OldCompressionSettings_B = bSourceTexture->CompressionSettings;
       const TextureCompressionSettings OldCompressionSettings_A = aSourceTexture->CompressionSettings;

       const TextureMipGenSettings OldMipGenSettings_R = rSourceTexture->MipGenSettings;
      const  TextureMipGenSettings OldMipGenSettings_G = gSourceTexture->MipGenSettings;
       const TextureMipGenSettings OldMipGenSettings_B = bSourceTexture->MipGenSettings;
      const  TextureMipGenSettings OldMipGenSettings_A = aSourceTexture->MipGenSettings;

       const bool OldSRGB_R = rSourceTexture->SRGB;
       const bool OldSRGB_G = gSourceTexture->SRGB;
      const  bool OldSRGB_B = bSourceTexture->SRGB;
      const  bool OldSRGB_A = aSourceTexture->SRGB;

        rSourceTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        gSourceTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        bSourceTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        aSourceTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

        rSourceTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        gSourceTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        bSourceTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        aSourceTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

        rSourceTexture->SRGB = false;
        gSourceTexture->SRGB = false;
        bSourceTexture->SRGB = false;
        aSourceTexture->SRGB = false;

        rSourceTexture->UpdateResource();
        gSourceTexture->UpdateResource();
        bSourceTexture->UpdateResource();
        aSourceTexture->UpdateResource();
        const FColor NullColor=FColor(Settings.DefaultR,Settings.DefaultG,Settings.DefaultB,Settings.DefaultA);
        //create new texture data buffer
        uint8 *pixels = (uint8 *) malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

        //set red channel
        const FColor* rSourceImageData = static_cast<const FColor*>( rSourceTexture->PlatformData->Mips[0].BulkData.LockReadOnly());
      
        int RDownScale=rSourceTexture->PlatformData->SizeX/width;
        int RUpScale=width/rSourceTexture->PlatformData->SizeX;
        if(RDownScale>RUpScale)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =rSourceTexture==NullTexture?NullColor: rSourceImageData[((y*RDownScale) * width*RDownScale) + x*RDownScale];
               
                    pixels[y * 4 * width + x * 4 + 2] = GetColorValue(PixelColor, Settings.rSourceChannel);//PixelColorR.R;
                }
            }
        }else
        {//scaling up
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =rSourceTexture==NullTexture?NullColor: rSourceImageData[((y/RUpScale) * width/RUpScale) + x/RUpScale];
               
                    pixels[y * 4 * width + x * 4 + 2] = GetColorValue(PixelColor, Settings.rSourceChannel);//PixelColorR.R;
                }
            }
        }
        rSourceTexture->PlatformData->Mips[0].BulkData.Unlock();

        //set green channel
        const FColor* gSourceImageData = static_cast<const FColor*>( gSourceTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

        int GDownScale=gSourceTexture->PlatformData->SizeX/width;
        int GUpScale=width/gSourceTexture->PlatformData->SizeX;
        if(GDownScale>GUpScale)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =gSourceTexture==NullTexture?NullColor: gSourceImageData[((y*GDownScale) * width*GDownScale) + x*GDownScale];
               
                    pixels[y * 4 * width + x * 4 + 1] = GetColorValue(PixelColor, Settings.gSourceChannel);//PixelColorR.R;
                }
            }
        }else
        {//scaling up
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =gSourceTexture==NullTexture?NullColor: gSourceImageData[((y/GUpScale) * width/GUpScale) + x/GUpScale];
               
                    pixels[y * 4 * width + x * 4 + 1] = GetColorValue(PixelColor, Settings.gSourceChannel);//PixelColorR.R;
                }
            }
        }
        gSourceTexture->PlatformData->Mips[0].BulkData.Unlock();

        //set blue channel
        const FColor* bSourceImageData = static_cast<const FColor*>( bSourceTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

        int BDownScale=bSourceTexture->PlatformData->SizeX/width;
        int BUpScale=width/bSourceTexture->PlatformData->SizeX;
        if(GDownScale>GUpScale)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =bSourceTexture==NullTexture?NullColor: bSourceImageData[((y*BDownScale) * width*BDownScale) + x*BDownScale];
               
                    pixels[y * 4 * width + x * 4 + 0] = GetColorValue(PixelColor, Settings.bSourceChannel);//PixelColorR.R;
                }
            }
        }else
        {//scaling up
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =bSourceTexture==NullTexture?NullColor: bSourceImageData[((y/BUpScale) * width/BUpScale) + x/BUpScale];
               
                    pixels[y * 4 * width + x * 4 + 0] = GetColorValue(PixelColor, Settings.bSourceChannel);//PixelColorR.R;
                }
            }
        }

        bSourceTexture->PlatformData->Mips[0].BulkData.Unlock();

        //set alpha channel
        const FColor* aSourceImageData = static_cast<const FColor*>( aSourceTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

        int ADownScale=aSourceTexture->PlatformData->SizeX/width;
        int AUpScale=width/aSourceTexture->PlatformData->SizeX;
        if(GDownScale>GUpScale)
        {
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =aSourceTexture==NullTexture?NullColor: aSourceImageData[((y*ADownScale) * width*ADownScale) + x*ADownScale];
               
                    pixels[y * 4 * width + x * 4 + 3] = GetColorValue(PixelColor, Settings.aSourceChannel);//PixelColorR.R;
                }
            }
        }else
        {//scaling up
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    FColor PixelColor =aSourceTexture==NullTexture?NullColor: aSourceImageData[((y/AUpScale) * width/AUpScale) + x/AUpScale];
               
                    pixels[y * 4 * width + x * 4 + 3] = GetColorValue(PixelColor, Settings.aSourceChannel);//PixelColorR.R;
                }
            }
        }


        aSourceTexture->PlatformData->Mips[0].BulkData.Unlock();

        //revert source texture settings
        rSourceTexture->CompressionSettings = OldCompressionSettings_R;
        gSourceTexture->CompressionSettings = OldCompressionSettings_G;
        bSourceTexture->CompressionSettings = OldCompressionSettings_B;
        aSourceTexture->CompressionSettings = OldCompressionSettings_A;

        rSourceTexture->MipGenSettings = OldMipGenSettings_R;
        gSourceTexture->MipGenSettings = OldMipGenSettings_G;
        bSourceTexture->MipGenSettings = OldMipGenSettings_B;
        aSourceTexture->MipGenSettings = OldMipGenSettings_A;

        rSourceTexture->SRGB = OldSRGB_R;
        gSourceTexture->SRGB = OldSRGB_G;
        bSourceTexture->SRGB = OldSRGB_B;
        aSourceTexture->SRGB = OldSRGB_A;

        rSourceTexture->UpdateResource();
        gSourceTexture->UpdateResource();
        bSourceTexture->UpdateResource();
        aSourceTexture->UpdateResource();

        FString pathPackage = *filePath;

        //UE_LOG(LogTemp, Log, TEXT( "pathPackage: %s" ), *pathPackage);
        //UE_LOG(LogTemp, Log, TEXT( "absolutePathPackage: %s" ), *absolutePathPackage);

        UPackage * Package = CreatePackage(nullptr, *pathPackage);
        Package->FullyLoad();

        UTexture2D* Texture = NewObject<UTexture2D>(Package, FName(*FileName), RF_Public | RF_Standalone | RF_MarkAsRootSet);
        Texture->AddToRoot();

        // Texture Settings
        Texture->PlatformData = new FTexturePlatformData();
        Texture->PlatformData->SizeX = width;
        Texture->PlatformData->SizeY = height;
        Texture->PlatformData->PixelFormat = EPixelFormat ::PF_R8G8B8A8;

        // Passing the pixels information to the texture
        FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();

        Mip->SizeX = width;
        Mip->SizeY = height;
        Mip->BulkData.Lock(LOCK_READ_WRITE);
        uint8* TextureData = (uint8 *) Mip->BulkData.Realloc(height * width * sizeof(uint8)*4);
        FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
        Mip->BulkData.Unlock();

        Texture->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_BGRA8, pixels);

        Texture->UpdateResource();
        Package->MarkPackageDirty();

        FAssetRegistryModule::AssetCreated( Texture );

        FString savedFileName = FPackageName::LongPackageNameToFilename(pathPackage, FPackageName::GetAssetPackageExtension());
        UPackage::SavePackage(Package, nullptr, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *savedFileName);

        free(pixels);
        pixels = NULL;

    }
}

void STextureChannelPackerWidget::CreateSettingsView()
{
    // Create a property view
    FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = true;
    DetailsViewArgs.bLockable = true;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::ComponentsAndActorsUseNameArea;
    DetailsViewArgs.bCustomNameAreaLocation = false;
    DetailsViewArgs.bCustomFilterAreaLocation = true;
    DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

    SettingsView = EditModule.CreateDetailView(DetailsViewArgs);
    //SettingsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateStatic(&Local::IsPropertyVisible, true));
}

uint8 STextureChannelPackerWidget::GetColorValue(FColor color, TextureChannel channel)
{
    int retVal = 0;

    switch(channel)
    {
        case TextureChannel::Red: {
            retVal = color.R;
            break;
        }

        case TextureChannel::Green:
        {
            retVal = color.G;
            break;
        }

        case TextureChannel::Blue:
        {
            retVal = color.B;
            break;
        }

        case TextureChannel::Alpha:
        {
            retVal = color.A;
            break;
        }

    }

    return retVal;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
