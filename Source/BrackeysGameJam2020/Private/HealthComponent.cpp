


#include "HealthComponent.h"
#include "KnockbackComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	TeamNumber = 255;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	 OwnerKnockbackComp = GetOwner()->FindComponentByClass<UKnockbackComponent>();
}

void UHealthComponent::DealDamage(AActor* DamagedActor, float Damage, AController * InstigatedBy, FVector HitImpactPoint, AActor* DamageCauser, float KnockbackForce)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	if (CurrentHealth <= 0.f && !bIsDead)
		bIsDead = true;

	if (OwnerKnockbackComp)
	{
		OwnerKnockbackComp->Knockback(KnockbackForce, HitImpactPoint, DamageCauser);
	}

	OnHealthChange.Broadcast(this, CurrentHealth, HitImpactPoint, InstigatedBy, DamageCauser);
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	//If an actor doesnt have no component we dont wont to do no more damage
	if (ActorA == nullptr || ActorB == nullptr)
		return true;

	auto* HealthCompA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	auto* HealthCompB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));
	
	if (HealthCompA == nullptr || HealthCompB == nullptr)
		return true;

	//UE_LOG(LogTemp, Warning, TEXT("A: %d B: %d"), HealthCompA->TeamNumber, HealthCompB->TeamNumber)
	if (HealthCompA->TeamNumber == HealthCompB->TeamNumber)
		return true;
	
	//UE_LOG(LogTemp, Warning, TEXT("Not Same Team"))
	return false;
}