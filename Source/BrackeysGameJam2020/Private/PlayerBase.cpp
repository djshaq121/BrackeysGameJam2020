


#include "Camera/CameraComponent.h"
#include "PlayerBase.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "DodgeBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

	BallIdlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Position"));
	BallIdlePosition->SetupAttachment(Camera);
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is unable to find Projectile Class reference!"), *GetName())
	}
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerBase::Dash);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerBase::PressShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerBase::ReleaseShoot);
}

void APlayerBase::Dash()
{
	if (bCanDash)
	{
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
			//DrawDebugCapsule(GetWorld(), Hit.Location + FVector(0.f,0.f, Height/1.5f), Height / 4.f, Radius, FQuat(GetActorRotation()), FColor::Red, false, 20.f);
		}
		else
		{
			DashLocation = Hit.TraceEnd;
		}

		bIsDashing = true;
		bCanDash = false;
		GetCharacterMovement()->GravityScale = 0.f;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->StopMovementImmediately();
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
		if (!GetWorldTimerManager().IsTimerActive(ChargeTimerHandle))
		{
			GetWorldTimerManager().SetTimer(ChargeTimerHandle, this, &APlayerBase::IncrementBallCharge, BallChargeInterval, true);
		}
	}
	else if (ProjectileRef)
	{
		ProjectileRef->SetBallState(DelayReturn);
	}
}

void APlayerBase::ReleaseShoot()
{
	if (!ProjectileClass) { return; }
	if (bCanShoot)
	{
		FTransform ProjectileTransform = BallIdlePosition->GetComponentTransform();
		ProjectileTransform.SetScale3D(FVector(0.1f));
		ProjectileRef = GetWorld()->SpawnActor<ADodgeBall>(ProjectileClass, ProjectileTransform);
		bCanShoot = false;

		const float& InitSpeed = ProjectileRef->ProjectileMovement->InitialSpeed;
		ProjectileRef->ProjectileMovement->Velocity = GetControlRotation().Vector() * (InitSpeed * ChargeAmount);
		UE_LOG(LogTemp, Log, TEXT("Charge: %d | Speed: %f"), ChargeAmount, ProjectileRef->ProjectileMovement->Velocity.Size());
		GetWorldTimerManager().ClearTimer(ChargeTimerHandle);
		ChargeAmount = 1;
	}
}

void APlayerBase::IncrementBallCharge()
{
	if (ChargeAmount < MaxBallCharge)
	{
		ChargeAmount++;
	}
}

