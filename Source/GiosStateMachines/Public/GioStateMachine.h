// Copyright MiddleMast. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GioNode.h"
#include "GioStateMachineData.h"
#include "GioStateActivation.h"
#include "GioStateMachine.generated.h"

class UGioStateMachineData;

DECLARE_DYNAMIC_DELEGATE_OneParam(FNodeExitHandler, FName, Output);

/**
 * @brief The state machine is a Node which contains child nodes (states). There will always be only one node active at
 * a time, and it will be Ticked along with the state machine. The state machine also has a property to define what
 * class its GioStateMachineData should be. By default, whenever the state machine is entered, a new instance of
 * GioStateMachineData will be created. This instance will be passed on to any child nodes, enabling them to share data.
 * This behaviour can be overriden.
 * <br><br>
 * This class is meant to be blueprinted to define transitions between Nodes using the blueprint graph. Along with the
 * basic Node's available blueprint nodes, the state machine can also use a special node:<br>
 * - <b>Enter Node</b>: Tells the state machine to enter a new state. The state is defined by class, and a new instance
 * of it will be created the first time the node is accessed. One execution pin will be created for each <b>input</b>
 * and <b>output</b> available for that node class.
 */
UCLASS(Blueprintable)
class GIOSSTATEMACHINES_API UGioStateMachine : public UGioNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGioStateMachineData> DataType = UGioStateMachineData::StaticClass();

	UPROPERTY()
	FGioStateActivation CurrentActivation{};

	UPROPERTY()
	TMap<const UClass*, UGioNode*> NodePool{};
	
	UPROPERTY()
	TArray<FGioStateActivation> StateHistory{};
	
public:
	void EnterViaFirstInput();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGioNode* GetCurrentNode() const { return CurrentActivation.Node; }

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	void EnterNewNode(UClass* NodeClass, FName Input, FString NodeGuid, const FNodeExitHandler& ExitHandler);
	
	virtual void Tick(const float& DeltaTime) override;

	bool IsRunning() const { return CurrentActivation.IsValid(); }
	
protected:
	virtual void OnEntered(const FName& Input) override;

	virtual UGioStateMachineData* CreateData();

	virtual UGioStateMachineData* GetDataForNewNode() const { return GetData(); }
	
private:
	void SetCurrentActivation(const FGioStateActivation& Activation);
	
	void HandleNodeExitRequest(UGioNode* Context, const FName& Output);

	void HandleNodeReturnRequest(UGioNode* Context);

	UGioNode* FindOrMakeNode(const UClass* NodeClass);
};
