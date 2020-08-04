

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UHealthComponent;
class UBehaviorTree;

UCLASS()
class BRACKEYSGAMEJAM2020_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, FVector HitDirection, class AController* InstigatedBy, AActor* DamageCauser);

public:	

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

public:

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
