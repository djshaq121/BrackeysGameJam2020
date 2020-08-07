


#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create sphere collision  and make it the root component. Used for tracking overlapping actors
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(200.f);

	//Create dodgeball static mesh component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	//Create Projectile Movement Component which will handle the projectile movement (Speed, bouncing, etc)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	
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
	if(!IsValid(GetOwner()) || OtherActor == GetOwner()) {return;}

	if (Cast<APawn>(OtherActor))
	{
		//Play actor hit sound
		if (SoundActorHit)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundActorHit, GetActorLocation());
	}
	else
	{
		//Play hit sound
		if (SoundHit)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundHit, GetActorLocation());
	}
}

