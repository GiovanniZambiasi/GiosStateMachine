// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"

GIOSSTATEMACHINES_API DECLARE_LOG_CATEGORY_EXTERN(LogGiosStateMachines, Display, Display)

#define LOG_GIOS_STATEMACHINES(Verbosity, Format, ...)	UE_LOG(LogGiosStateMachines, Verbosity, Format, __VA_ARGS__)

class FGiosStateMachinesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
