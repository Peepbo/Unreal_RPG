// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PlayerCharacter.h"

// Sets default values
AEnemy::AEnemy() :
	bInAttackRange(false),
	BattleWalkSpeed(130.f),
	BattleRunSpeed(400.f),
	bIsSprint(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->ComponentTags.Add(TEXT("Agro"));

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->ComponentTags.Add(TEXT("CombatRange"));

	MaximumWalkSpeed = 170.f;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true
	);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true
	);

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController) {
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		EnemyAIController->RunBehaviorTree(BehaviorTree);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		if (EnemyAIController) {
			Target = Character;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) {
		bInAttackRange = true;

		if (EnemyAIController) {
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) {
		bInAttackRange = false;

		if (EnemyAIController) {
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::ChangeBattleMode()
{
	bIsBattleMode = !bIsBattleMode;

	GetCharacterMovement()->MaxWalkSpeed = (bIsBattleMode ? BattleWalkSpeed : MaximumWalkSpeed);

	if (EnemyAIController) {
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsBattleMode"), bIsBattleMode);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDying)return DamageAmount;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageState = EDamageState::EDS_invincibility;
	if (bDying) {
		HideHealthBar();
	}

	return DamageAmount;
}