


#include "DodgeBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
#include "HealthComponent.h"
#include "PlayerBase.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ADodgeBall::ADodgeBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

	SphereCollision->OnComponentHit.AddDynamic(this, &ADodgeBall::OnCompHit);
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

void ADodgeBall::LaunchProjectile(FVector Direction, int ChargeAmount)
{
	ActorsHit.Empty();
	//SetBallState(Projectile::Thrown); //Causing Error
	BallState = Projectile::Thrown;
	if(SphereCollision)
		SphereCollision->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	ProjectileMovement->Velocity = Direction * (ProjectileMovement->InitialSpeed * ChargeAmount);
	
}

void ADodgeBall::ReturnProjectile()
{
	if (BallState != Projectile::Thrown || BallState==Projectile::Return)
		return;

	//Allow us to hit enemies on the way back;
	ActorsHit.Empty();
	StoreBallPosiitonAndStopMovement();
	SetBallState(DelayReturn);
}

void ADodgeBall::SetBallState(TEnumAsByte<Projectile> ProjectileState)
{
	//If ProjectileMovementComponent does not exist for some reason, prevent nullptr crash by ending the function early
	if(!ProjectileMovement) {return;}

	//Set the state of the ball to the value given in the function
	BallState = ProjectileState;

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
		//DrawDebugPoint(GetWorld(), GetActorLocation(), 10.f, FColor::Red, false, 2.f);
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
		//BallDelayDelegate.BindUFunction(this, FName("SetBallState"), Return);
		//GetWorldTimerManager().SetTimer(BallDelayTimer, BallDelayDelegate, BallReturnDelay, false);
		GetWorldTimerManager().SetTimer(BallDelayTimer, this, &ADodgeBall::BeginReturningProjectile, BallReturnDelay, false);
	}
}

void ADodgeBall::BeginReturningProjectile()
{
	//Update collision profile
	if (SphereCollision)
		SphereCollision->SetCollisionProfileName("OverlapAll");
	
	StoreBallPosiitonAndStopMovement();
	SetBallState(Return);
}

void ADodgeBall::StoreBallPosiitonAndStopMovement()
{
	//Halt all projectile movement, store the position at where the projectile was stopped
	BallPosition = GetActorLocation();
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ADodgeBall::ReturnToPlayer()
{
	//If there is no player reference for some reason, stop the function early to prevent a nullptr
	if(!PlayerRef) {return;}

	//Smoothly interpolate the movement from current position to the players position to simulate returning to the player
	FVector Position = FMath::VInterpTo(GetActorLocation(), PlayerRef->GetActorLocation(), GetWorld()->GetDeltaSeconds(), ReturnSpeed);

	//Update the actors position to the smoothed position
	SetActorLocation(Position, true);

	//Destroy ball when in range of player and allow the player to shoot the ball again
	if (GetActorLocation().Equals(PlayerRef->GetActorLocation(), 100.f))
	{
		Destroy();
		PlayerRef->bCanShoot = true;

		//Play hit sound
		if (SoundHit)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundHit, GetActorLocation());

		//Play UI hit animation
		if (PlayerRef->PlayerUIRef && PlayerRef->UIHitAnim)
			PlayerRef->PlayerUIRef->PlayAnimation(PlayerRef->UIHitAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
	}
}

void ADodgeBall::OverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OverlapComponent(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if(!IsValid(GetOwner()) || OtherActor == GetOwner() || OtherActor == this)
		 return; 

	if (ActorsHit.Contains(OtherActor))
		return;

	ActorsHit.Add(OtherActor);

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

	UE_LOG(LogTemp, Warning, TEXT("Overlap Projectile - Actor added: %s"), *OtherActor->GetName())

	//Prevent ball from being influenced by the player
	bCanCurve = false;
	bIsCurving = false;

	//Make sure GetOwner() is valid and we arent colliding with the actor that spawned us
	AActor* ActorOwner = GetOwner();
	if(!ActorOwner || !OtherActor  || OtherActor == ActorOwner  ) { return; }

	//Get health component and make sure it is valid
	UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
	if (!HealthComp) { return; }

	//Get the controller of the actor that spawned us
	AController* InstigatorController = ActorOwner->GetInstigatorController<AController>();

	//Deal damage to who we overlapped with
	HealthComp->DealDamage(OtherActor, DamageAmount, InstigatorController, SweepResult.ImpactPoint, this, KnockbackForce);

	//Play UI hit animation
	if (PlayerRef->PlayerUIRef && PlayerRef->UIHitAnim)
		PlayerRef->PlayerUIRef->PlayAnimation(PlayerRef->UIHitAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
}

void ADodgeBall::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!IsValid(GetOwner()) || OtherActor == GetOwner() || OtherActor == this)
		return;

	if (!OtherActor)
		return;
	//Prevent ball from being influenced by the player
	bCanCurve = false;
	bIsCurving = false;

	if (ActorsHit.Contains(OtherActor))
		return;

	ActorsHit.Add(OtherActor);

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

	if (!OtherActor)
		return;

	//Get health component and make sure it is valid
	UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
	if (!HealthComp)
		return;

	//Get the controller of the actor that spawned us
	AController* InstigatorController = GetOwner()->GetInstigatorController<AController>();

	//Deal damage to who we hit with
	HealthComp->DealDamage(OtherActor, DamageAmount, InstigatorController, Hit.ImpactPoint, this, KnockbackForce);

	//Play UI hit animation
	if (PlayerRef->PlayerUIRef && PlayerRef->UIHitAnim)
		PlayerRef->PlayerUIRef->PlayAnimation(PlayerRef->UIHitAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, true);
}

