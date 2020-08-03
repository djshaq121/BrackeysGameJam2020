

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PickupBase.h"
#include "PlayerBase.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(PickupMesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetupAttachment(PickupMesh);
	SphereCollision->SetSphereRadius(500.f);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &APickupBase::OnPlayerOverlap);
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupBase::OnPlayerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerBase* PlayerReference = Cast<APlayerBase>(OtherActor);

	//Call blueprint function and destroy actor
	if (PlayerReference)
	{
		PlayerInteract(PlayerReference);
		Destroy();
	}
	
}

