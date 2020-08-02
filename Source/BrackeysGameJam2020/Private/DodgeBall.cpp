


#include "DodgeBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBase.h"

// Sets default values
ADodgeBall::ADodgeBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	SetRootComponent(StaticMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
}

// Called when the game starts or when spawned
void ADodgeBall::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerRef = Cast<APlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!PlayerRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is unable to find player reference!"), *GetName())
	}
}

// Called every frame
void ADodgeBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (BallState)
	{
		case Thrown: Throw(); break;
		case DelayReturn: ReturnDelay(); break;
		case Return: ReturnToPlayer(); break;
	}

}

void ADodgeBall::SetBallState(TEnumAsByte<Projectile> ProjectileState)
{
	BallState = ProjectileState;

	BallPosition = GetActorLocation();
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ADodgeBall::Throw()
{

}

void ADodgeBall::ReturnDelay()
{
	FVector Position = FMath::VInterpTo(GetActorLocation(), BallPosition + FVector(0.f,0.f,100.f), GetWorld()->GetDeltaSeconds(), ReturnSpeed);

	SetActorLocation(Position);

	//Change ball state to return after set time
	if (!GetWorldTimerManager().IsTimerActive(BallDelayTimer))
	{
		BallDelayDelegate.BindUFunction(this, FName("SetBallState"), Return);
		GetWorldTimerManager().SetTimer(BallDelayTimer, BallDelayDelegate, BallReturnDelay, false);
	}
}

void ADodgeBall::ReturnToPlayer()
{
	if(!PlayerRef) {return;}

	FVector Position = FMath::VInterpTo(GetActorLocation(), PlayerRef->GetActorLocation(), GetWorld()->GetDeltaSeconds(), ReturnSpeed);

	SetActorLocation(Position);

	//Destroy ball when in range of player and allow the player to shoot the ball again
	if (GetActorLocation().Equals(PlayerRef->GetActorLocation(), 100.f))
	{
		Destroy();
		PlayerRef->bCanShoot = true;
	}
}

