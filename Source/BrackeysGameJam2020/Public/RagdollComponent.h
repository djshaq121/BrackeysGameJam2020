

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RagdollComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRACKEYSGAMEJAM2020_API URagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URagdollComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "RagdollComponent")
	void Ragdoll();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RagdollComponent")
	FName BoneName = "pelvis";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RagdollComponent")
	float PhysicsBlendWeight = 1.f;

private:
	AActor* Owner;
};
