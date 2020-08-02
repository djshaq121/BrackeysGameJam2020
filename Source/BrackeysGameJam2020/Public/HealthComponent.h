

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangeSignature, UHealthComponent*, HealthComp, float, Health, FVector, HitDirection, class AController*, InstigatedBy, AActor*, DamageCauser);


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

protected:

	float CurrentHealth;

	bool bIsDead = false;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0), Category = "HealthComponent")
	float MaxHealth;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHealthChangeSignature OnHealthChange;
	
	UFUNCTION(BlueprintCallable, Category = "Health Component")
	void DealDamage(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, AActor * DamageCauser);
};
