


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UHealthComponent::DealDamage(AActor * DamagedActor, float Damage, AController * InstigatedBy, FVector HitLocation, AActor * DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	if (CurrentHealth <= 0.f && !bIsDead)
		bIsDead = true;

	OnHealthChange.Broadcast(this, CurrentHealth, HitLocation, InstigatedBy, DamageCauser);
		
	
	
}