

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DodgeBall.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;
class APlayerBase;

//Enumerator for dodge ball state machine
UENUM()
enum Projectile
{
	Thrown,
	DelayReturn,
	Return
};

UCLASS()
class BRACKEYSGAMEJAM2020_API ADodgeBall : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ADodgeBall();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Projectile Movement Component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	//Projectile Mesh Component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	//Current state of Dodge Ball
	UPROPERTY()
	TEnumAsByte<Projectile> BallState = Thrown;

	//Called to modify whether the ball is returning to the player
	UFUNCTION(BlueprintCallable, Category = "Dodgeball")
	void SetBallState(TEnumAsByte<Projectile> ProjectileState);

	//The speed at which the ball returns to the player
	UPROPERTY(EditDefaultsOnly, Category = "Dodgeball")
	float ReturnSpeed = 5.f;

	//The return delay before the ball goes back to the player
	UPROPERTY(EditDefaultsOnly, Category = "Dodgeball")
	float BallReturnDelay = 2.f;

	//Called when the projectile hits something
	UFUNCTION()
	void OnHitActor(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

private:
	
	//Called when ball is in Throw state
	void Throw();

	//Called when ball is in ReturnDelay state
	void ReturnDelay();

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

};
