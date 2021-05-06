// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TextureChannelPackerSettings.h"

class IDetailsView;

/**
 * 
 */
class TEXTURECHANNELPACKERTOOL_API STextureChannelPackerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STextureChannelPackerWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FReply OnPackTextureChannelsClicked();

	bool CanPackTextureChannels();
	void CreatePackedTexture(const FString& filePath);

private:
	/** Creates and sets up the settings view element*/
	void CreateSettingsView();
	uint8 GetColorValue(FColor color, TextureChannel channel);
	bool ValidTargetTextureSize();
	bool TexturesArePropperSizeForTarget();
	bool TextureSlotsAsigned();
	bool IsPowerOfTwo(int num);
	FText GetErrorMessage();

private:
	UTextureChannelPackerSettingsObject* TextureChannelPackerSettings;
	TSharedPtr<IDetailsView> SettingsView;
};
