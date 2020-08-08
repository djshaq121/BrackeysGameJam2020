

#include "EnemyAIController.h"
#include "EnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	TargetKeyName = "Target";
}

void AEnemyAIController::OnPossess(APawn* PossessPawn)
{
	Super::OnPossess(PossessPawn);

	if (BehaviorComp == nullptr || BehaviorComp == nullptr)
		return;

	if (auto AICharacter = Cast<AEnemyBase>(PossessPawn))
	{
		if (AICharacter->BehaviorTree == nullptr || AICharacter->BehaviorTree->BlackboardAsset == nullptr)
			return;

		BlackboardComp->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
		
		BehaviorComp->StartTree(*AICharacter->BehaviorTree);
	}
}

void AEnemyAIController::OnUnPossess()
{
	if (!BlackboardComp || !BehaviorComp) 
		return;

	BehaviorComp->StopTree();
	Super::OnUnPossess();
	SetLifeSpan(2.0f); // Destroy ai controller once pawn unpossess
}

void AEnemyAIController::SetTarget(AActor* Target)
{
	BlackboardComp->SetValueAsObject(TargetKeyName, Target);
}


