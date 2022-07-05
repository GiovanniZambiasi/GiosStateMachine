// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StateMachine.generated.h"

class UState;

DECLARE_DYNAMIC_DELEGATE_OneParam(FStateExitHandler, FName, Output);

UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UStateMachine : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	UState* CurrentState;

	FStateExitHandler StateExitHandler;
	
public:
	UFUNCTION(BlueprintCallable)
	void Run();
	
	void Tick(const float& DeltaTime);

	UState* GetCurrentState() const { return CurrentState; }
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnRun();
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler);
	
private:
	void HandleStateExitRequest(const FName& Output);
};
