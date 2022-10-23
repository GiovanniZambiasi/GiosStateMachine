// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.generated.h"

class UStateMachineData;
class UState;

DECLARE_EVENT_TwoParams(UState, FStateExitRequestHandler, UState* /*Context*/, const FName& /*RequestedOutput*/)

DECLARE_EVENT_OneParam(UState, FStateReturnRequestHandler, UState* /*Context*/)

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

	FStateReturnRequestHandler ReturnRequestedEvent{};
	
public:
	FStateExitRequestHandler& OnExitRequested() { return ExitRequestedEvent; }

	FStateReturnRequestHandler& OnReturnRequested() { return ReturnRequestedEvent; }

	virtual void SetData(UStateMachineData* Data);
	
	virtual void Enter(const FName& Input);
	
	virtual void Tick(const float& DeltaTime);
	
	/**
	 * @brief Called when the state machine returns to this state from another state
	 * <br><br>
	 * <i>This a response to a call to RequestReturn() from another state in the owning StateMachine</i>
	 */
	virtual void Returned();

	/**
	 * @brief Requests an exit via the Output pin to it's owning StateMachine
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RequestExit(FName Output);

	/**
	 * @brief Requests a return to the previous state in the owning StateMachine's history
	 * <br><br>
	 * <i>If the history is empty, stops the state machine from running</i>
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RequestReturn();
	
	const TArray<FName>& GetInputs() const { return Inputs; }
	
	const TArray<FName>& GetOutputs() const { return Outputs; }

	UStateMachineData* GetData() const { return StateMachineData; }
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEntered(const FName& Input);

	UFUNCTION(BlueprintImplementableEvent)
	void OnReturned();
	
	void AddInputs(const TArray<FName>& InputNames)	{ Inputs.Append(InputNames); }

	void AddOutputs(const TArray<FName>& OutputNames)	{ Outputs.Append(OutputNames); }

	void SetInputs(const TArray<FName>& InputNames);

	void SetOutputs(const TArray<FName>& OutputNames);

private:
	void RemoveProtectedNames(TArray<FName>& Names, const TSet<FName>& ProtectedNames);

#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
};
