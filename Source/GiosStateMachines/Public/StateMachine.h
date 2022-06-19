// Fill out your copyright notice in the Description page of Project Settings.

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
	void Begin();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBegin();
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler);

private:
	void HandleStateExitRequest(const FName& Output);
};
