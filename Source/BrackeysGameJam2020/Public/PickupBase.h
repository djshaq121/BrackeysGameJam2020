

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class BRACKEYSGAMEJAM2020_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Pickup mesh
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PickupMesh;

	//Sphere Collision
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereCollision;

	//Called for when an actor overlaps with Sphere Collision Component
	UFUNCTION()
	void OnPlayerOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//Called when player overlaps with Sphere Collision Component
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
	void PlayerInteract(const APlayerBase* PlayerReference);
};
