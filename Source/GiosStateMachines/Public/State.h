// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.generated.h"

DECLARE_EVENT_OneParam(UState, FStateExitRequestHandler, const FName& /*RequestedOutput*/)

UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UState : public UObject
{
private:
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Inputs = { "Default" };

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Outputs = { "Default" };

	FStateExitRequestHandler ExitRequestedEvent{};
	
public:
	virtual void Enter(const FName& Input);

	const TArray<FName>& GetInputs() const { return Inputs; }
	
	const TArray<FName>& GetOutputs() const { return Outputs; }

	FStateExitRequestHandler& OnExitRequested() { return ExitRequestedEvent; }

	UFUNCTION(BlueprintImplementableEvent)
	void OnEntered(const FName& Input);
	
protected:
	
	UFUNCTION(BlueprintCallable)
	void RequestExit(const FName& Output);
	
	void AddInputs(const TArray<FName>& InputNames)	{ Inputs.Append(InputNames); }

	void AddOutputs(const TArray<FName>& OutputNames)	{ Outputs.Append(OutputNames); }

	void SetInputs(const TArray<FName>& InputNames);

	void SetOutputs(const TArray<FName>& OutputNames);
};
