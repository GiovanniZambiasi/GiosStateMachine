// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"

struct FStateActivation
{
	TSharedPtr<class UState> State;

	class FStateExitHandler& ExitHandler;
};
