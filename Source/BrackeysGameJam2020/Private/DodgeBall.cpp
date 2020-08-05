


#include "DodgeBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "HealthComponent.h"
#include "PlayerBase.h"

// Sets default values
ADodgeBall::ADodgeBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create dodgeball static mesh component and make it the root component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	SetRootComponent(StaticMesh);
	
	//Create Projectile Movement Component which will handle the projectile movement (Speed, bouncing, etc)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
}

// Called when the game starts or when spawned
void ADodgeBall::BeginPlay()
{
	Super::BeginPlay();
	
	//Get player reference
	PlayerRef = Cast<APlayerBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	//Display error if for some reason the player was unable to be found or the Cast failed
	if (!PlayerRef)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is unable to find PlayerBase reference!"), *GetName())
	}

	//Bind OnActorHit to my own OnHitActor() function so when OnActorHit is triggered, OnHitActor() is called
	OnActorHit.AddDynamic(this, &ADodgeBall::OnHitActor);
}

// Called every frame
void ADodgeBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Ball state machine which will call different functions based on the value of BallState
	switch (BallState)
	{
		case Thrown: Throw(); break;
		case DelayReturn: ReturnDelay(); break;
		case Return: ReturnToPlayer(); break;
	}

}

void ADodgeBall::SetBallState(TEnumAsByte<Projectile> ProjectileState)
{
	//If ProjectileMovementComponent does not exist for some reason, prevent nullptr crash by ending the function early
	if(!ProjectileMovement) {return;}

	//Set the state of the ball to the value given in the function
	BallState = ProjectileState;

	//Halt all projectile movement, store the position at where the projectile was stopped
	BallPosition = GetActorLocation();
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ADodgeBall::Throw()
{
	if (bIsCurving)
	{
		/**
		 * Calculate how the ball will be influenced by getting the ControlRotation
		 * Then, calculate the Rotation from the player to the Ball.
		 * Find the difference of the two values (This will make the value 0 when the player is looking directly at the ball but allow the velocity to change otherwise)
		 */
		FVector Direction = PlayerRef->GetControlRotation().Vector();
		Direction -= UKismetMathLibrary::FindLookAtRotation(PlayerRef->GetActorLocation(), GetActorLocation()).Vector();

		//Multiply the direction of the force by a value to make the ball move
		ProjectileMovement->AddForce(Direction * 3000.f);

		//Debug points to show ball influence
		DrawDebugPoint(GetWorld(), GetActorLocation(), 10.f, FColor::Red, false, 2.f);
	}
}

void ADodgeBall::ReturnDelay()
{
	//Use the ball position when the player requested to return the ball and a value right above to simulate "hovering".
	//Input these values into a Interp function which will smoothly interpret between the two values based on ReturnSpeed
	FVector Position = FMath::VInterpTo(GetActorLocation(), BallPosition + FVector(0.f,0.f,100.f), GetWorld()->GetDeltaSeconds(), ReturnSpeed);

	//Set the position of the Ball to the Smoothed location
	SetActorLocation(Position);

	//Change ball state to 'Return' after set time and clear the timer
	if (!GetWorldTimerManager().IsTimerActive(BallDelayTimer))
	{
		BallDelayDelegate.BindUFunction(this, FName("SetBallState"), Return);
		GetWorldTimerManager().SetTimer(BallDelayTimer, BallDelayDelegate, BallReturnDelay, false);
	}
}

void ADodgeBall::ReturnToPlayer()
{
	//If there is no player reference for some reason, stop the function early to prevent a nullptr
	if(!PlayerRef) {return;}

	//Smoothly interpolate the movement from current position to the players position to simulate returning to the player
	FVector Position = FMath::VInterpTo(GetActorLocation(), PlayerRef->GetActorLocation(), GetWorld()->GetDeltaSeconds(), ReturnSpeed);

	//Update the actors position to the smoothed position
	SetActorLocation(Position);

	//Destroy ball when in range of player and allow the player to shoot the ball again
	if (GetActorLocation().Equals(PlayerRef->GetActorLocation(), 100.f))
	{
		Destroy();
		PlayerRef->bCanShoot = true;
	}
}

void ADodgeBall::OnHitActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//Prevent ball from being influenced by the player
	bCanCurve = false;
	bIsCurving = false;

	if (!OtherActor)
		return;

	auto HealthComp = Cast<UHealthComponent>(OtherActor->GetComponentByClass(UHealthComponent::StaticClass()));
	if (!HealthComp)
		return;

	HealthComp->DealDamage(OtherActor, DamageAmount, GetOwner()->GetInstigatorController(), Hit.ImpactPoint, this);
}

