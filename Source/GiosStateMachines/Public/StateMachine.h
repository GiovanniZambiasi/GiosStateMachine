// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "State.h"
#include "StateMachineData.h"
#include "StateActivation.h"
#include "StateMachine.generated.h"

class UStateMachineData;

DECLARE_DYNAMIC_DELEGATE_OneParam(FStateExitHandler, FName, Output);

/**
 * @brief The heart of your state machine. This class is meant to be blueprinted to define the state transition flow. 
 */
UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UStateMachine : public UState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStateMachineData> DataType = UStateMachineData::StaticClass();

	UPROPERTY()
	FStateActivation CurrentActivation{};

	TArray<FStateActivation> StateHistory{};
	
public:
	virtual void Enter(const FName& Input) override;
	
	void EnterViaFirstInput();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UState* GetCurrentState() const { return CurrentActivation.State; }

	virtual void Tick(const float& DeltaTime) override;

	bool IsRunning() const { return CurrentActivation.IsValid(); }
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterNewState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler);

	virtual UStateMachineData* CreateData();
	
private:
	void SetCurrentActivation(const FStateActivation& Activation);
	
	void HandleStateExitRequest(UState* Context, const FName& Output);

	void HandleStateReturnRequest(UState* Context);
};
