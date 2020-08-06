

#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "DodgeBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Camera component and attach it to the root component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	//Create Scene component to store where the dodge ball spawns from the player. Attach it to the camera so the position/rotation is relative to the camera component
	BallIdlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Position"));
	BallIdlePosition->SetupAttachment(Camera);

	//Create box component so there is an area where the player can deflect the attacks.
	DeflectBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Deflect Box"));
	DeflectBox->SetupAttachment(Camera);
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
	//Send an error if there is no ProjectileClass reference in the BP actor.
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is unable to find Projectile Class reference!"), *GetName())
	}
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Smoothly transition the player position from 'A' to 'B' while dashing
	if (bIsDashing)
	{
		FVector SmoothDashLocation = FMath::VInterpTo(GetActorLocation(), DashLocation, DeltaTime, 10.f);
		SetActorLocation(SmoothDashLocation);

		//Complete player dash and return to normal movement
		if (GetActorLocation().Equals(DashLocation, 50.f))
		{
			bIsDashing = false;
			GetCharacterMovement()->SetDefaultMovementMode();
			GetCharacterMovement()->GravityScale = 1.f;
		}
	}

}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind selected actions (which can be found in the Input settings in UE4) to the specified functions
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerBase::Dash);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerBase::PressShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerBase::ReleaseShoot);
	PlayerInputComponent->BindAction("CurveBall", IE_Pressed, this, &APlayerBase::PressCurve);
	PlayerInputComponent->BindAction("CurveBall", IE_Released, this, &APlayerBase::ReleaseCurve);
}

void APlayerBase::Dash()
{
	if (bCanDash)
	{
		/**
		 * Collect necessary variables 
		 * - getting the end location based on PlayerController rotation and DashDistance
		 * - ignoring 'self' actor in sweep trace
		 * - getting player capsule size
		 * to calculate the Sweep Trace
		 */
		FHitResult Hit;
		FVector ControlDirection = GetControlRotation().Vector();
		FVector EndLocation = GetActorLocation() + (ControlDirection * DashDistance);
		FCollisionQueryParams CollisionParams = FCollisionQueryParams(NAME_None, false, this);
		float Height = 0.f, Radius = 0.f;
		GetCapsuleComponent()->GetUnscaledCapsuleSize(Radius, Height);

		//Move line trace start position to in front of player so it is easier to dash downwards
		FVector PlayerFront = GetActorLocation() + ControlDirection * 100.f;
		PlayerFront.Z = GetActorLocation().Z;

		//Perform capsule sweep to detect incoming collision. Reduce capsule height to allow for smoother dashes
		GetWorld()->SweepSingleByChannel(Hit, PlayerFront, EndLocation, FQuat(GetActorRotation()), ECC_Visibility, FCollisionShape::MakeCapsule(Radius, Height / 4.f), CollisionParams);

		//Set dash location to slightly above first blocking hit or furthest location
		if (Hit.bBlockingHit)
		{
			DashLocation = Hit.Location + FVector(0.f, 0.f, Height / 1.5f);
		}
		else
		{
			DashLocation = Hit.TraceEnd;
		}

		//To allow for smoother dashes, disable gravity, stop movement, and set movement mode to flying
		GetCharacterMovement()->GravityScale = 0.f;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->StopMovementImmediately();

		//Set appropriate variables to let the player/code know that they are dashing and that they cannot dash until cooldown is over
		bIsDashing = true;
		bCanDash = false;
		
		//Set dash timer to DashCooldown variable
		GetWorldTimerManager().SetTimer(DashTimer, this, &APlayerBase::EnableDash, DashCooldown, false);
	}
	
}

void APlayerBase::EnableDash()
{
	bCanDash = true;
}

void APlayerBase::PressShoot()
{
	if (bCanShoot)
	{
		//Setup timer to call IncrementBallCharge() function every BallChargeInterval seconds
		if (!GetWorldTimerManager().IsTimerActive(ChargeTimerHandle))
		{
			GetWorldTimerManager().SetTimer(ChargeTimerHandle, this, &APlayerBase::IncrementBallCharge, BallChargeInterval, true);
		}
	}
	else if (ProjectileRef && ProjectileRef->GetBallState() == Thrown)
	{
		//If the projectile exists and the player cannot shoot (Meaning that they can only recall the ball), set the dodgeball to the DelayReturn state
		ProjectileRef->SetBallState(DelayReturn);
	}
}

void APlayerBase::ReleaseShoot()
{
	//Stop the function early if ProjectileClass is nullptr to prevent a crash
	if (!ProjectileClass) { return; }

	if (bCanShoot)
	{
		//Spawn the actor at the BallIdlePosition and set the Scale to 0.1 so the ball is not so large
		FTransform ProjectileTransform = BallIdlePosition->GetComponentTransform();
		ProjectileTransform.SetScale3D(FVector(0.1f));
		ProjectileRef = GetWorld()->SpawnActor<ADodgeBall>(ProjectileClass, ProjectileTransform);
		ProjectileRef->SetOwner(this);
		bCanShoot = false;

		//Adjust the velocity of the ball based on the amount of charges the player held for. Reset the ChargeAmount and clear the ChargeTimerHandle (since it was set to loop) after calculating
		const float& InitSpeed = ProjectileRef->ProjectileMovement->InitialSpeed;
		ProjectileRef->ProjectileMovement->Velocity = GetControlRotation().Vector() * (InitSpeed * ChargeAmount);
		GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
		ChargeAmount = 1;

		if (bTryToCurve)
		{
			ProjectileRef->SetIsCurving(true);
		}
	}
}

void APlayerBase::PressCurve()
{
	bTryToCurve = true;

	if (ProjectileRef && ProjectileRef->GetCanCurve())
	{
		ProjectileRef->SetIsCurving(true);
	}
}

void APlayerBase::ReleaseCurve()
{
	bTryToCurve = false;
	if (ProjectileRef)
	{
		ProjectileRef->SetCanCurve(false);
		ProjectileRef->SetIsCurving(false);
	}
}

void APlayerBase::IncrementBallCharge()
{
	if (ChargeAmount < MaxBallCharge)
	{
		ChargeAmount++;
	}
}

