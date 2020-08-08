

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

class UCameraComponent;
class ADodgeBall;
class UBoxComponent;
class UHealthComponent;
class UCameraShake;
class USoundBase;
class UUserWidget;
class UWidgetAnimation;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCooldown = 5.f;

	//Used in BP to visualize the cooldown of the dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCurrentCooldown = 0.f;

	//Dodgeball class
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ADodgeBall> ProjectileClass;
	
	//Determine the max ball charge that the player can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	int32 MaxBallCharge = 5;

	//Value that stores how charged the dodgeball is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	int32 ChargeAmount = 1;

	//Rate, in seconds, that the ball charge increases by 1
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float BallChargeInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<UCameraShake> DamageCamShake;

	//Sound when throwing the ball
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundThrow;

	//Sound when dashing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundDash;

	//Sound when returning to the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundReturn;

	//Sound when charging the dodgeball
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundCharge;

	//Footstep sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundFootstep;

	//Jump sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundJump;

	//Player landing sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundLand;

	//Player landing sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* SoundDeath;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float FootstepInterval = 0.5f;

	void Landed(const FHitResult& Hit) override;

	//Dodgeball actor reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ADodgeBall* ProjectileRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* PlayerUIRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* UIHitAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* UIDashAnim;

protected:
	UHealthComponent* HealthComp;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, FVector HitDirection, class AController* InstigatedBy, AActor* DamageCauser);

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

	//Handle the interval between gaining charges
	FTimerHandle ChargeTimerHandle;

	//Called to increment the ball charge amount
	UFUNCTION()
	void IncrementBallCharge();

	//Whether the player is trying to curve the ball
	bool bTryToCurve;

	//Handle footstep timer handle sounds
	FTimerHandle FootstepTimerHandle;

	void Footstep();

};
