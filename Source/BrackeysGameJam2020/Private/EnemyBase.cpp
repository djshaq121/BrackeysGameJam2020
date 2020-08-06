


#include "EnemyBase.h"
#include "HealthComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Comp"));
	AddOwnedComponent(HealthComponent);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthComponent)
		HealthComponent->OnHealthChange.AddDynamic(this, &AEnemyBase::OnHealthChanged);
	
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player == nullptr)
		return;

	AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
		AIController->SetTarget(Player);
	
}

void AEnemyBase::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health,  FVector HitDirection,  AController * InstigatedBy, AActor * DamageCauser)
{
	if (OwningHealthComp->GetIsDead())
	{
		if (AIController)
		{
			//Stop Behavior tree
			AIController->OnUnPossess();
		}
		
		//Remove collision
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

		//Destroy Actor 
		SetLifeSpan(LifeSpan);
	}
		
	
}

