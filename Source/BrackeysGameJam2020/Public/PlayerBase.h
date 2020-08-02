

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerBase.generated.h"

class UCameraComponent;

/**
 * The base class for the player
 */
UCLASS()
class BRACKEYSGAMEJAM2020_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
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
};
