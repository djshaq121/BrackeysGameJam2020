

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
UCLASS()
class BRACKEYSGAMEJAM2020_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

private:
	 UBehaviorTreeComponent* BehaviorComp;

	 UBlackboardComponent* BlackboardComp;

public:
	AEnemyAIController();

	virtual void OnPossess(APawn* PossessPawn) override;

	virtual void OnUnPossess() override;

	void SetTarget(AActor* Target);

public:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetKeyName;
};
