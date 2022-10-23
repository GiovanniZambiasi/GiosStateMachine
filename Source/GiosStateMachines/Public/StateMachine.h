// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "StateMachineData.h"
#include "UObject/Object.h"
#include "StateActivation.h"
#include "State.h"
#include "StateMachine.generated.h"

class UState;
class UStateMachineData;

DECLARE_DYNAMIC_DELEGATE_OneParam(FStateExitHandler, FName, Output);

/**
 * @brief The heart of your state machine. This class is meant to be blueprinted to define the state transition flow. 
 */
UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UStateMachine : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStateMachineData> DataType = UStateMachineData::StaticClass();

	UPROPERTY(VisibleAnywhere)
	UStateMachineData* StateMachineData;

	UPROPERTY()
	FStateActivation CurrentActivation{};

	TArray<FStateActivation> StateHistory{};
	
public:
	UFUNCTION(BlueprintCallable)
	void Run();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UState* GetCurrentState() const { return CurrentActivation.State; }

	void Tick(const float& DeltaTime);

	void SetData(UStateMachineData* Data);

	UStateMachineData* GetData() const { return StateMachineData; }

	bool IsRunning() const { return CurrentActivation.IsValid(); }
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnRun();
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterNewState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler);

	virtual UStateMachineData* CreateData();
	
private:
	void SetCurrentActivation(const FStateActivation& Activation);
	
	void HandleStateExitRequest(UState* Context, const FName& Output);

	void HandleStateReturnRequest(UState* Context);
};
