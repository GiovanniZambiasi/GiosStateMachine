// Copyright MiddleMast. All rights reserved

#include "K2Node_GioStateNode.h"
#include "GioNode.h"

#define LOCTEXT_NAMESPACE "UK2Node_StateNode"

UGioNode* UK2Node_GioStateNode::GetDefaultStateObject() const
{
	auto Class = GetBlueprint()->GeneratedClass;
	
	if(!Class)
	{
		return nullptr;
	}
	
	return Cast<UGioNode>(Class->ClassDefaultObject);
}

#undef LOCTEXT_NAMESPACE