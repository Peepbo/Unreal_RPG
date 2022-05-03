// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DarkKnight/PatrolDarkKnight.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

APatrolDarkKnight::APatrolDarkKnight() :
	PatrolIndex(0),
	bPatrol(true)
{

}

void APatrolDarkKnight::BeginPlay()
{
	Super::BeginPlay();

	bPatrol = true;
	RetargetPatrolPath();
}

void APatrolDarkKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool PatrolAble{ !Target && PatrolPoints.Num() > 0 };
	const bool PatrolTurn{ bPatrol && GetTurn() };
	if (PatrolAble && PatrolTurn)
	{
		const FRotator LookRot{ UKismetMathLibrary::FindLookAtRotation(GetNavAgentLocation(), GetPatrolPath()[1]) };

		// 공격 회전 속도와 제자리 회전 속도를 다르게한다.
		const float SelectInterpSpeed{ InplaceRotateSpeed };

		// 회전 방향이 왼쪽인지 오른쪽인지 구한다.
		
		const FVector2D Forward2D{ GetActorForwardVector() };
		const FVector2D ActorToPatrol2D{ UKismetMathLibrary::Normal(GetPatrolPath()[1] - GetNavAgentLocation()) };

		const float Cross{ UKismetMathLibrary::CrossProduct2D(Forward2D, ActorToPatrol2D) };
		const bool bLeft{ Cross < 0.f };
		SetTurnLeft(bLeft);

		const float PlusYaw{ (bLeft ? -SelectInterpSpeed : SelectInterpSpeed) * DeltaTime };

		SetActorRotation(UKismetMathLibrary::ComposeRotators(GetActorRotation(), { 0.f,PlusYaw,0.f }));
	}
}

void APatrolDarkKnight::CallRetargetPathAndDraw()
{
	RetargetPatrolPath();
	DrawPatrolPath(10.f);
}

int32 APatrolDarkKnight::GetPatrolIndex()
{
	return PatrolIndex;
}

TArray<FVector> APatrolDarkKnight::GetPatrolPath()
{
	return PatrolPath[PatrolIndex];
}

void APatrolDarkKnight::InitPatrolPath(int32 Size)
{
	PatrolPath.SetNum(Size);
}

void APatrolDarkKnight::DrawPatrolPath(float DrawTime)
{
	if (PatrolPath.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Path Data is Empty. You mush Call 'RetargetPatrolPath' Function"));
		return;
	}

	const int32 PathCount{ PatrolPath.Num() };

	for (int32 PathIndex = 0; PathIndex < PathCount; PathIndex++)
	{
		const TArray<FVector>& PathRef{ PatrolPath[PathIndex] };
		const int32 PointCount{ PathRef.Num() };

		for (int32 PointIndex = 1; PointIndex < PointCount; PointIndex++)
		{
			const FVector& StartPtrRef{ PathRef[PointIndex - 1] };
			const FVector& EndPtrRef{ PathRef[PointIndex] };

			UKismetSystemLibrary::DrawDebugArrow(
				GetWorld(),
				StartPtrRef,
				EndPtrRef,
				100.f,
				FColor::Magenta,
				DrawTime,
				2.f);
		}
	}
}

void APatrolDarkKnight::RetargetPatrolPath()
{
	//UE_LOG(LogTemp, Warning, TEXT("Retarget Function Call"));

	if (PatrolPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolPoint array is empty"));
		return;
	}

	WorldPatrolPoints.SetNum(PatrolPoints.Num());

	const int32 PatrolCount{ PatrolPoints.Num() };

	// Local 좌표에서 World 좌표로 변환한다.
	for (int32 Index = 0; Index < PatrolCount; Index++)
	{
		WorldPatrolPoints[Index] = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoints[Index]);
	}

	// PatrolPath 크기 초기화
	InitPatrolPath(PatrolCount);

	const int32 LastPatrolIndex{ PatrolCount - 1 };
	int32 SavePathIndex;
	FVector StartPoint;
	FVector EndPoint;
	TArray<FVector> Path;

	// 이제 경로를 계산한다. (만약 Point가 3개(0,1,2)라면 0-1, 1-2, 2-0 경로를 계산)

	// PatrolPoint[0]은 현재 위치이므로 경로 계산할 필요가 없다.
	for (int32 Index = 1; Index < PatrolCount; Index++)
	{
		StartPoint = WorldPatrolPoints[Index - 1];
		EndPoint = WorldPatrolPoints[Index];

		SavePathIndex = Index - 1;

		const UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(),
			StartPoint,
			EndPoint);

		if (NavigationPath == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NavigationPath is nullptr"));
			PatrolPath.Empty();
			return;
		}

		Path = NavigationPath->PathPoints;

		PatrolPath[SavePathIndex] = Path;
	}

	// lastPoint와 firstPoint의 경로를 계산한다.
	StartPoint = WorldPatrolPoints[LastPatrolIndex];
	EndPoint = WorldPatrolPoints[0];

	SavePathIndex = LastPatrolIndex;

	const UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		StartPoint,
		EndPoint);
	Path = NavigationPath->PathPoints;

	PatrolPath[SavePathIndex] = Path;
}

void APatrolDarkKnight::NextPath()
{
	PatrolIndex++;

	if (PatrolIndex == PatrolPoints.Num())
	{
		PatrolIndex = 0;
	}
}

FVector APatrolDarkKnight::GetActorToPatrolFirstPointDirection()
{
	return UKismetMathLibrary::Normal((GetPatrolPath()[0] - GetActorLocation()));
}