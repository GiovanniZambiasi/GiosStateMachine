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

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGioStateMachine> StateMachineClass;

	UPROPERTY()
	UGioStateMachine* StateMachine;
	
public:	
	UGioStateMachineRunnerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void RunStateMachine(TSubclassOf<UGioStateMachine> Class);

	UFUNCTION(BlueprintCallable)
	UGioStateMachine* GetStateMachine() const { return StateMachine; }

protected:
	virtual void BeginPlay() override;
};
