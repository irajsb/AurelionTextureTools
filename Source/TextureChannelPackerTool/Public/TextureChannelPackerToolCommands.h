// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TextureChannelPackerToolStyle.h"

class FTextureChannelPackerToolCommands : public TCommands<FTextureChannelPackerToolCommands>
{
public:

	FTextureChannelPackerToolCommands()
		: TCommands<FTextureChannelPackerToolCommands>(TEXT("TextureChannelPackerTool"), NSLOCTEXT("Contexts", "TextureChannelPackerTool", "TextureChannelPackerTool Plugin"), NAME_None, FTextureChannelPackerToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};