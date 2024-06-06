// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GioNode.generated.h"

class UGioStateMachineData;
class UGioNode;

DECLARE_EVENT_TwoParams(UGioNode, FNodeExitRequestHandler, UGioNode* /*Context*/, const FName& /*RequestedOutput*/)

DECLARE_EVENT_OneParam(UGioNode, FNodeReturnRequestHandler, UGioNode* /*Context*/)

/**
 * @brief The core class for all states and state machines. A Node is a simple UObject which can be <b>Entered</b>,
 * <b>Exited</b> and <b>Ticked*</b>. It has <b>inputs</b> and <b>outputs</b>, which represent points that it can be
 * entered from, and exited to. The purpose of these points is to allow abstraction between different nodes in the state
 * machine. Inputs and Outputs can be defined in the class default panels, or set via code during construction.
 * <br><br>
 * <i>*The Tick is not automatic, and must be manually called. If you don't want to manually call it, use the
 * StateMachineRunnerComponent</i>
 * <br><br>
 * Nodes can also <b>Return</b>. This tells their owning state machine to return to the previous Node, if applicable.
 * <br><br>
 * A Node also contains a pointer to a <b>StateMachineData</b> object. This is set by the owning state machine before
 * entering the Node. By default, the StateMachineData instance is shared across the state machine, and is meant to
 * serve as a way to transfer data between different Nodes in the state machine.
 * <br><br>
 * A few custom blueprint nodes can be used to unlock the full potential of the Node class:
 * - <b>Node Entered</b>: Exposes one execution pin for each <b>input</b> in the Node class.
 * - <b>Exit Node</b>: Exposes one execution pin for each <b>output</b> in the Node class. Will request exit via the pin
 * that gets executed.
 */
UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UGioNode : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Inputs = { TEXT("Default") };

	UPROPERTY(EditDefaultsOnly, Category="Gio's StateMachines")
	TArray<FName> Outputs = { TEXT("Default") };

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	UGioStateMachineData* StateMachineData = nullptr;
	
	FNodeExitRequestHandler ExitRequestedEvent{};

	FNodeReturnRequestHandler ReturnRequestedEvent{};
	
public:
	FNodeExitRequestHandler& OnExitRequested() { return ExitRequestedEvent; }

	FNodeReturnRequestHandler& OnReturnRequested() { return ReturnRequestedEvent; }

	UFUNCTION(BlueprintCallable)
	virtual void SetData(UGioStateMachineData* Data);
	
	void Enter(const FName& Input);
	
	virtual void Tick(const float& DeltaTime);

	/**
	 * @brief Requests an exit via the Output pin to it's owning StateMachine
	 */
	UFUNCTION(BlueprintCallable)
	void RequestExit(FName Output);

	/**
	 * @brief Requests a return to the previous node in the owning StateMachine's history
	 * <br><br>
	 * <i>If the history is empty, stops the state machine from running</i>
	 */
	UFUNCTION(BlueprintCallable)
	void RequestReturn();

	virtual void OnEntered(const FName& Input) { }

	virtual void OnExited(const FName& Output) { }
	
	/**
	 * @brief Called when the state machine returns to this node from another node
	 * <br><br>
	 * <i>This a response to a call to RequestReturn() from another state in the owning StateMachine</i>
	 */
	virtual void OnReturned();

	virtual const TArray<FName>& GetInputs() const { return Inputs; }
	
	virtual const TArray<FName>& GetOutputs() const { return Outputs; }

	UGioStateMachineData* GetData() const { return StateMachineData; }
	
	template<typename TData>
	TData* GetDataChecked()
	{
		return CastChecked<TData>(StateMachineData);
	}
	
protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnEntered")
	void K2_OnEntered(const FName& Input);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnReturned")
	void K2_OnReturned();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnTick")
	void K2_OnTick(float DeltaTime);
	
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
