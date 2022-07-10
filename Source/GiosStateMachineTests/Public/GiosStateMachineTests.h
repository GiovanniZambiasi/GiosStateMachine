// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"

class GIOSSTATEMACHINETESTS_API FGioTestUtils
{
public:
	static const FString EmptyTestSceneName;
	
	static void Exit(UWorld* World);

	template<typename T>
	static T* CastResource(UObject* Resource)
	{	
		if (UBlueprint* BP = Cast<UBlueprint>(Resource))
		{
			return BP->GeneratedClass->GetDefaultObject<T>();
		}
		
		return Cast<T>(Resource);
	}

private:
	FGioTestUtils() = delete;
};

struct FStreamableHandle;
