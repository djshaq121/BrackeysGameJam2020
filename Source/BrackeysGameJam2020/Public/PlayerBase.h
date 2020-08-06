

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

class UCameraComponent;
class ADodgeBall;
class UBoxComponent;

/**
 * The base class for the player
 */
UCLASS()
class BRACKEYSGAMEJAM2020_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this character's properties
	APlayerBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DeflectBox;

	//Determine whether the player can throw their dodgeball
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dodgeball")
	bool bCanShoot = true;

	//Camera component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	//Ball spawn and idle position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* BallIdlePosition;

	//Determine the max dash distance
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashDistance = 500.f;

	//Determine the dash interpolation speed
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashSpeed = 10.f;

	//Determine the cooldown of the dash
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashCooldown = 5.f;

	//Dodgeball class
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ADodgeBall> ProjectileClass;
	
	//Determine the max ball charge that the player can have
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 MaxBallCharge = 5;

	//Rate, in seconds, that the ball charge increases by 1
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float BallChargeInterval = 0.5f;

private:
	//Called to dash the player in a straight line
	UFUNCTION()
	void Dash();

	//Whether the player is dashing
	bool bIsDashing = false;

	//Whether the player can dash
	bool bCanDash = true;

	//Location for the player to dash to
	FVector DashLocation;

	//Timer to handle dash cooldown
	FTimerHandle DashTimer;

	//Called to reenable dash mechanic
	UFUNCTION()
	void EnableDash();

	//Called when the player presses the shoot binding
	UFUNCTION()
	void PressShoot();

	//Called when the player releases the shoot binding
	UFUNCTION()
	void ReleaseShoot();

	//Called when the player presses the shoot binding
	UFUNCTION()
	void PressCurve();

	//Called when the player releases the shoot binding
	UFUNCTION()
	void ReleaseCurve();

	//Dodgeball actor reference
	ADodgeBall* ProjectileRef;

	//Value that stores how charged the dodgeball is
	int32 ChargeAmount = 1;

	//Handle the interval between gaining charges
	FTimerHandle ChargeTimerHandle;

	//Called to increment the ball charge amount
	UFUNCTION()
	void IncrementBallCharge();

	//Whether the player is trying to curve the ball
	bool bTryToCurve;
};
