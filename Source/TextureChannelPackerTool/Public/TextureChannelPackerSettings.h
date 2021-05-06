// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/NoExportTypes.h"
#include "TextureChannelPackerSettings.generated.h"


UENUM()
enum TextureChannel
{
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
	Alpha UMETA(DisplayName = "Alpha"),
};

/**
 * 
 */
//UCLASS()
//class TEXTURECHANNELPACKERTOOL_API UTextureChannelPackerSettings : public UObject
USTRUCT(Blueprintable)
struct FTextureChannelPackerSettings
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	int32 targetTextureSize;

   

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	UTexture2D * rChannelTargetSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	TEnumAsByte<TextureChannel> rSourceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	UTexture2D * gChannelTargetSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	TEnumAsByte<TextureChannel> gSourceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	UTexture2D * bChannelTargetSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	TEnumAsByte<TextureChannel> bSourceChannel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	UTexture2D * aChannelTargetSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TextureChannelPacker)
	TEnumAsByte<TextureChannel> aSourceChannel;

	//Value to apply to empty Channels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Advanced)
	uint8 DefaultR;
	//Value to apply to empty Channels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Advanced)
	uint8 DefaultG;
	//Value to apply to empty Channels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Advanced)
	uint8 DefaultB;
	//Value to apply to empty Channels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Advanced)
	uint8 DefaultA;

	FTextureChannelPackerSettings()
	:targetTextureSize(1024)
	,rSourceChannel(TextureChannel::Red)
	,gSourceChannel(TextureChannel::Green)
	,bSourceChannel(TextureChannel::Blue)
	,aSourceChannel(TextureChannel::Alpha)
	{

	}
};

UCLASS()
class UTextureChannelPackerSettingsObject : public UObject
{
	GENERATED_BODY()
public:
	UTextureChannelPackerSettingsObject()
	{
		//Settings.bMergePhysicsData = true;
		// In this case set to AllLODs value since calculating the LODs is not possible and thus disabled in the UI
		//Settings.LODSelectionType = EMeshLODSelectionType::AllLODs;
	}

	static UTextureChannelPackerSettingsObject* Get()
	{
		static bool bInitialized = false;
		// This is a singleton, use default object
		UTextureChannelPackerSettingsObject* DefaultSettings = GetMutableDefault<UTextureChannelPackerSettingsObject>();

		if (!bInitialized)
		{
			bInitialized = true;
		}

		return DefaultSettings;
	}

public:
	UPROPERTY(editanywhere, meta = (ShowOnlyInnerProperties), Category = TextureChannelPackerSettings)
	FTextureChannelPackerSettings Settings;
};