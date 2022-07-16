// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.generated.h"

class UStateMachineData;

DECLARE_EVENT_OneParam(UState, FStateExitRequestHandler, const FName& /*RequestedOutput*/)

UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UState : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Inputs = { "Default" };

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Outputs = { "Default" };

	UPROPERTY()
	UStateMachineData* StateMachineData = nullptr;
	
	FStateExitRequestHandler ExitRequestedEvent{};
	
public:
	FStateExitRequestHandler& OnExitRequested() { return ExitRequestedEvent; }

	virtual void SetData(UStateMachineData* Data);
	
	virtual void Enter(const FName& Input);
	
	virtual void Tick(const float& DeltaTime);

	UFUNCTION(BlueprintCallable)
	virtual void RequestExit(FName Output);
	
	const TArray<FName>& GetInputs() const { return Inputs; }
	
	const TArray<FName>& GetOutputs() const { return Outputs; }

	UStateMachineData* GetData() const { return StateMachineData; }
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEntered(const FName& Input);
	
	void AddInputs(const TArray<FName>& InputNames)	{ Inputs.Append(InputNames); }

	void AddOutputs(const TArray<FName>& OutputNames)	{ Outputs.Append(OutputNames); }

	void SetInputs(const TArray<FName>& InputNames);

	void SetOutputs(const TArray<FName>& OutputNames);
};
