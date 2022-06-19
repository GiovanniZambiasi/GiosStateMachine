// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

GIOSSTATEMACHINES_API DECLARE_LOG_CATEGORY_EXTERN(LogGameStates, Display, Display)

#define LOG_GAME_STATES(Verbosity, Format, ...)	UE_LOG(LogGameStates, Verbosity, Format, __VA_ARGS__)

class FGiosStateMachinesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
