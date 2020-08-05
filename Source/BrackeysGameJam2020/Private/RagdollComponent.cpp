

#include "Components/SkeletalMeshComponent.h"
#include "RagdollComponent.h"

// Sets default values for this component's properties
URagdollComponent::URagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void URagdollComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();

}

void URagdollComponent::Ragdoll()
{
	if (!Owner)
		return;

	USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!Mesh)
		return;

	Mesh->SetAllBodiesBelowSimulatePhysics(BoneName, true);
	Mesh->SetAllBodiesBelowPhysicsBlendWeight(BoneName, PhysicsBlendWeight);
}



