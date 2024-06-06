// Fill out your copyright notice in the Description page of Project Settings.


#include "GioHumbleNode.h"

void UGioHumbleNode::OnEntered(const FName& Input)
{
	Super::OnEntered(Input);
	bEntered = true;
}

void UGioHumbleNode::OnExited(const FName& Output)
{
	Super::OnExited(Output);
	bExited = true;
}

void UGioHumbleNode::OnReturned()
{
	Super::OnReturned();
	bReturned = true;
}
