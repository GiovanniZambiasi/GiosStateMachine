// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GioStateMachineRunnerComponent.generated.h"

class UGioStateMachine;

UCLASS( ClassGroup=("Gio's State Machines"), meta=(BlueprintSpawnableComponent) )
class GIOSSTATEMACHINES_API UGioStateMachineRunnerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(Category="Gio's State Machines", EditAnywhere)
	TSubclassOf<UGioStateMachine> StateMachineClass{};

	/**
	 * If this component should automatically run the state machine on BeginPlay. If false, the StateMachine instance
	 * will be created anyway, and can be run via the RunStateMachine method
	 */
	UPROPERTY(Category="Gio's State Machines", EditDefaultsOnly)
	bool bAutoRun{true};
	
	UPROPERTY()
	UGioStateMachine* StateMachine{};
	
public:	
	UGioStateMachineRunnerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void CreateAndRunStateMachine(TSubclassOf<UGioStateMachine> Class);

	UFUNCTION(BlueprintCallable)
	UGioStateMachine* GetStateMachine() const { return StateMachine; }

	UFUNCTION(BlueprintCallable)
	void CreateStateMachine(TSubclassOf<UGioStateMachine> Class);

	void RunStateMachine();
	
	void SetAutoRun(bool bValue);
	
protected:
	virtual void BeginPlay() override;
};
