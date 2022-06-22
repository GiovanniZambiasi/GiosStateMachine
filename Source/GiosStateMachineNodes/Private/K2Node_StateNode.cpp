// Copyright MiddleMast. All rights reserved

#include "K2Node_StateNode.h"
#include "State.h"

#define LOCTEXT_NAMESPACE "UK2Node_StateNode"

UState* UK2Node_StateNode::GetDefaultStateObject() const
{
	auto Class = GetBlueprint()->GeneratedClass;
	
	if(!Class)
	{
		return nullptr;
	}
	
	return Cast<UState>(Class->ClassDefaultObject);
}

#undef LOCTEXT_NAMESPACE