

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangeSignature, UHealthComponent*, HealthComp, float, Health, FVector, HitImpactPoint, class AController*, InstigatedBy, AActor*, DamageCauser);

class UKnockbackComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRACKEYSGAMEJAM2020_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health Component")
	bool GetIsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void DealDamage(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitImpactPoint, AActor * DamageCauser, float KnockbackForce = 1.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

protected:

	float CurrentHealth;

	bool bIsDead = false;

	UKnockbackComponent* OwnerKnockbackComp;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "HealthComponent")
	float MaxHealth;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHealthChangeSignature OnHealthChange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	uint8 TeamNumber;

private:

	
};
