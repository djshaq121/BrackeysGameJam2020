

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

class UCameraComponent;
class ADodgeBall;
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

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ADodgeBall> ProjectileClass;

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

	UFUNCTION()
	void PressShoot();

	UFUNCTION()
	void ReleaseShoot();

	ADodgeBall* ProjectileRef;
};
