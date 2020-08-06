


#include "KnockbackComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UKnockbackComponent::UKnockbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UKnockbackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UKnockbackComponent::Knockback(float KnockbackForce, FVector ImpactPoint, AActor* DamageCauser)
{
	if (!GetOwner())
		return;

	if (!bCanKnockBack)
		return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
		return;

	FVector Direction = OwnerCharacter->GetActorLocation() - DamageCauser->GetActorLocation();
	FVector LaunchVector = Direction * KnockbackForce;
	LaunchVector = FVector(LaunchVector.X, LaunchVector.Y, 5);
	OwnerCharacter->LaunchCharacter(LaunchVector, false, true);
}
