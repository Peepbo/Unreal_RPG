// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkKnight.h"
#include "KnightAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MeleeCharacter.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

ADarkKnight::ADarkKnight():
	bShouldDrawWeapon(true)
{

}

void ADarkKnight::BeginPlay()
{
	Super::BeginPlay();

	WalkSpeed = 130.f;
	RunSpeed = 400.f;
	NonBattleWalkSpeed = 170.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UKnightAnimInstance* KnightAnimInst = Cast<UKnightAnimInstance>(AnimInst);

		if (KnightAnimInst) {
			AnimInstance = KnightAnimInst;
		}
	}
}

void ADarkKnight::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bShouldDrawWeapon)return;
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<AMeleeCharacter>(OtherActor);
	if (Character) {
		bShouldDrawWeapon = false;

		if (EnemyAIController) {
			Target = Character;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);

			StartDraw();
		}
	}

}

void ADarkKnight::StartDraw()
{
	GetCharacterMovement()->StopActiveMovement();

	if (DrawMontage) {
		if (AnimInstance) {
			AnimInstance->Montage_Play(DrawMontage);
			UE_LOG(LogTemp, Warning, TEXT("Montage Play"));
		}
	}
}

void ADarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target) {
		
		const FVector Direction{ Target->GetActorLocation() - this->GetActorLocation() };
		EnemyToTargetDir = Direction.GetSafeNormal();
		
		// enemy의 방향
		EnemyDir = Target->GetActorForwardVector().GetSafeNormal();
	}
}