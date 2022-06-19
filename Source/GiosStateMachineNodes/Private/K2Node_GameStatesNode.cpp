#include "K2Node_GameStatesNode.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#define LOCTEXT_NAMESPACE "UK2Node_GameStatesNode"

void UK2Node_GameStatesNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);

	const auto Key = GetClass();
	if(ActionRegistrar.IsOpenForRegistration(Key))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(Key, NodeSpawner);
	}
}

#undef LOCTEXT_NAMESPACE