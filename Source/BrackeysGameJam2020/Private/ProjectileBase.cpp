


#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create dodgeball static mesh component and make it the root component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	SetRootComponent(StaticMesh);

	//Create Projectile Movement Component which will handle the projectile movement (Speed, bouncing, etc)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	//Create sphere collision and attach to StaticMesh. Used for tracking overlapping actors
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereCollision->SetupAttachment(StaticMesh);
	SphereCollision->SetSphereRadius(200.f);
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	//Bind OnActorHit to my own OnHitActor() function so when OnActorHit is triggered, OnHitActor() is called
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapComponent);
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::OnOverlapComponent_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapComponent(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AProjectileBase::OverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Meant to be overridden in child class
}

