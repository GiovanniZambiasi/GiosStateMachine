// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GioNode.h"
#include "GioHumbleNode.generated.h"

UCLASS()
class GIOSSTATEMACHINETESTS_API UGioHumbleNode : public UGioNode
{
	GENERATED_BODY()

public:
	bool bEntered{};
	
	bool bExited{};
	
	bool bReturned{};
	
	virtual void OnEntered(const FName& Input) override;

	virtual void OnExited(const FName& Output) override;

	virtual void OnReturned() override;

};
