// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "StateMachineData.h"
#include "UObject/Object.h"
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
	
	UPROPERTY()
	UState* CurrentState;

	UPROPERTY()
	UStateMachineData* StateMachineData;
	
	FStateExitHandler StateExitHandler;
	
public:
	UFUNCTION(BlueprintCallable)
	void Run();
	
	void Tick(const float& DeltaTime);

	void SetData(UStateMachineData* Data);
	
	UState* GetCurrentState() const { return CurrentState; }

	UStateMachineData* GetData() const { return StateMachineData; }
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnRun();
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterState(UClass* StateClass, FName Input, const FStateExitHandler& ExitHandler);

	virtual UStateMachineData* CreateData();
	
private:
	void HandleStateExitRequest(const FName& Output);
};
