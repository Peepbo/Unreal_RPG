// Fill out your copyright notice in the Description page of Project Settings.


#include "DarkKnight.h"
#include "KnightAnimInstance.h"

void ADarkKnight::BeginPlay()
{
	Super::BeginPlay();

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UKnightAnimInstance* KnightAnimInst = Cast<UKnightAnimInstance>(AnimInst);

		if (KnightAnimInst) {
			AnimInstance = KnightAnimInst;
		}
	}
}