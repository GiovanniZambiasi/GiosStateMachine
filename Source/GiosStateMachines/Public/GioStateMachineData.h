// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GioStateMachineData.generated.h"

/**
 * @brief Simple UObject that is meant to transfer data between different nodes in a state machine. You should sub-class
 * this and define your own set of variables and events this object should hold. By default, a new instance of it will be
 * created when a state machine is entered, and the instance will be shared with all its child nodes. This enables two
 * separate nodes in the state machine to communicate, with a level of abstraction between them.
*/
UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UGioStateMachineData : public UDataAsset
{
	GENERATED_BODY()
};
