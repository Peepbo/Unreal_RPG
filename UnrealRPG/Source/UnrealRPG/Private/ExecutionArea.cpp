// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecutionArea.h"
#include "Player/PlayerCharacter.h"
#include "Enemy/Enemy.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AExecutionArea::AExecutionArea():
	bActive(false)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExecutionArea::ActiveExecution()
{
	bActive = true;
	Enemy->ClearStunTimer();
	Enemy->PlayTakeExecutionMontage();

	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AExecutionArea::InitExecutionData(APlayerCharacter* Character, float FDamage, float SDamage)
{
	if (Character)
	{
		PlayerCharacter = Character;
	}
	ExecutionDamages.Add(FDamage);
	if (SDamage != 0.f) 
	{
		ExecutionDamages.Add(SDamage);
	}
}

void AExecutionArea::ApplyDamageToEnemy(int32 Index)
{
	float SelectDamage;

	if (ExecutionDamages.IsValidIndex(Index))
	{
		SelectDamage = ExecutionDamages[Index];
	}
	else
	{
		SelectDamage = ExecutionDamages[0];
	}

	Enemy->CustomApplyDamage(Enemy, SelectDamage, PlayerCharacter, EAttackType::EAT_Strong);

	// 피해 파티클이 존재할 때 타격 위치에 파티클을 생성한다.
	if (Enemy->GetBloodParticle()) {
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			Enemy->GetBloodParticle(),
			Enemy->GetMesh()->GetSocketTransform("TakeExecutionBloodSocket"));
	}
}

void AExecutionArea::InitEventLocationAndRotation()
{
	EventLocation = GetActorLocation();
	EventLocation.Z = PlayerCharacter->GetActorLocation().Z;

	// 최종 회전 값을 구한다.
	FVector ControlForward{ FRotator(0.f,PlayerCharacter->GetControlRotation().Yaw,0.f).Vector() };
	ControlForward = ControlForward.GetSafeNormal2D();
	ControlForward = UKismetMathLibrary::Normal(ControlForward);
	FVector PointToEnemy{ -(GetEnemy()->GetActorForwardVector()) };
	PointToEnemy = PointToEnemy.GetSafeNormal2D();

	const float DotResult{ UKismetMathLibrary::Dot_VectorVector(ControlForward,PointToEnemy) };
	float Degree{ UKismetMathLibrary::DegAcos(DotResult) };

	const FVector CrossResult{ UKismetMathLibrary::Cross_VectorVector(ControlForward, PointToEnemy) };
	Degree = CrossResult.Z < 0.f ? -Degree : Degree;

	const float LookDegree{ UKismetMathLibrary::ComposeRotators(FRotator(0.f, PlayerCharacter->GetControlRotation().Yaw, 0.f), { 0.f,Degree,0.f }).Yaw };
	EventRotator.Yaw = LookDegree;
}

void AExecutionArea::PlayerRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActive)
	{
		return;
	}
	if (bClosePlayer)
	{
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		bClosePlayer = true;
		Player->SetEventAble(true, this);
	}
}
