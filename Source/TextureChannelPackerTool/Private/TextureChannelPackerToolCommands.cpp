// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TextureChannelPackerToolCommands.h"

#define LOCTEXT_NAMESPACE "FTextureChannelPackerToolModule"

void FTextureChannelPackerToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "TextureChannelPackerTool", "Bring up TextureChannelPackerTool window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
