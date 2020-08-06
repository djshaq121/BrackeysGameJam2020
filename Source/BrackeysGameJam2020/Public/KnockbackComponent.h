

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KnockbackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRACKEYSGAMEJAM2020_API UKnockbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKnockbackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable, Category = "KnockbackComponent")
	void Knockback(float KnockbackForce, FVector ImpactPoint, AActor* DamageCauser);

private:

	bool bCanKnockBack = true;

	
		
};
