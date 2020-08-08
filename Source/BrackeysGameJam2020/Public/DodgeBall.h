

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.h"
#include "DodgeBall.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class APlayerBase;
class USphereComponent;

//Enumerator for dodge ball state machine
UENUM()
enum Projectile
{
	Idle,
	Thrown,
	DelayReturn,
	Return
};

UCLASS()
class BRACKEYSGAMEJAM2020_API ADodgeBall : public AProjectileBase
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ADodgeBall();

	void LaunchProjectile(FVector Direction, int ChargeAmount);

	void ReturnProjectile();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called to modify whether the ball is returning to the player
	UFUNCTION(BlueprintCallable, Category = "Dodgeball")
	void SetBallState(TEnumAsByte<Projectile> ProjectileState);

	//Called to get current state of ball
	UFUNCTION(BlueprintCallable)
	TEnumAsByte<Projectile> GetBallState() const { return BallState; }

	//The speed at which the ball returns to the player
	UPROPERTY(EditDefaultsOnly, Category = "Dodgeball")
	float ReturnSpeed = 5.f;

	//The return delay before the ball goes back to the player
	UPROPERTY(EditDefaultsOnly, Category = "Dodgeball")
	float BallReturnDelay = 2.f;

	//Called when the projectile hits something
	void OverlapComponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	//Accessor to get bCanCurve
	bool GetCanCurve() const { return bCanCurve; }

	//Accessor to get bIsCurving
	UFUNCTION(BlueprintCallable)
	bool GetIsCurving() const { return bIsCurving; }

	//Set value of bCanCurve
	void SetCanCurve(const bool& CanCurve) { bCanCurve = CanCurve; }

	//Set value of bIsCurving
	void SetIsCurving(const bool& IsCurving) { bIsCurving = IsCurving; }

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* DodgeBallReturnToPlayerSound;

private:
	
	//Called when ball is in Throw state
	void Throw();

	//Called when ball is in ReturnDelay state
	void ReturnDelay();

	void BeginReturningProjectile();

	void StoreBallPosiitonAndStopMovement();

	//Called when ball is in Return state
	void ReturnToPlayer();

	//Store ball position
	FVector BallPosition;

	//ReturnDelay timer handle
	FTimerHandle BallDelayTimer;

	//ReturnDelay timer delegate
	FTimerDelegate BallDelayDelegate;

	//Player reference
	APlayerBase* PlayerRef;

	//Determine whether the player can curve the ball
	bool bCanCurve = true;

	//Determine whether the ball is actually curving
	bool bIsCurving = false;

	//Current state of Dodge Ball
	UPROPERTY()
	TEnumAsByte<Projectile> BallState = Idle;
};
