

#include "PickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerBase.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create pickup static mesh component so there is a visual for the player.
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(PickupMesh);

	//Create invisible sphere collision component so the player can interact with the powerup from a further range.
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(PickupMesh);
	SphereCollision->SetSphereRadius(500.f);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	//Bind the OnActorBeginOverlap to my own OnPlayerOverlap() function so when one gets called, the other is called.
	OnActorBeginOverlap.AddDynamic(this, &APickupBase::OnPlayerOverlap);
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupBase::OnPlayerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//Get player reference
	APlayerBase* PlayerReference = Cast<APlayerBase>(OtherActor);

	//Call BP function PlayerInteract() and destroy the actor
	if (PlayerReference)
	{
		PlayerInteract(PlayerReference);
		Destroy();
	}
	
}

