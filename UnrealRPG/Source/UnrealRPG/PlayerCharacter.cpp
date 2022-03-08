// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Shield.h"
#include "PlayerAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

APlayerCharacter::APlayerCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bShouldComboAttack(false),
	AttackCombo(0.f),
	ST(50.f),
	MaximumST(50.f),
	StaminaRecoveryDelayTime(0.3f),
	bPressedSprintButton(false),
	bPressedRollButton(false),
	bPressedSubAttackButton(false),
	BeforeAttackRotateSpeed(0.1f),
	bIsBeforeAttackRotate(false),
	bIsChargedAttack(false),
	bShouldChargedAttack(false),
	RollRequiredStamina(10.f),
	bLockOn(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 붐 생성 및 특정 값 초기화
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	// 폰이 회전할 때 카메라 붐도 같이 회전합니다.
	CameraBoom->bUsePawnControlRotation = true;

	// 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 폰이 회전할 때 카메라는 회전하지 않습니다. (카메라 붐이 움직이므로 카메라를 움직일 필요x)
	FollowCamera->bUsePawnControlRotation = false;

	// 컨트롤러가 회전할 때 캐릭터가 회전하지 않습니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터의 방향은 입력 방향으로 지정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 캐릭터의 회전 속도를 의미합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1000.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	EquipWeapon(SpawnDefaultWeapon());
	EquippedWeapon->SetCharacter(this);
	EquipShield(SpawnDefaultShield());
	EquippedShield->SetCharacter(this);

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UPlayerAnimInstance* PlayerAnimInst = Cast<UPlayerAnimInstance>(AnimInst);
		if (PlayerAnimInst) {
			AnimInstance = PlayerAnimInst;
		}
	}

	/* 락온에 필요한 정보를 미리 만들어둠 */
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
}

void APlayerCharacter::MoveForward(float Value)
{
	if (CombatState == ECombatState::ECS_Attack)return;

	if (Controller && Value != 0.f) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 전방인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (CombatState == ECombatState::ECS_Attack)return;

	if (Controller && Value != 0.f) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 우측인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	if (bLockOn)return;

	// 이번 프레임에 이동해야될 Yaw 각도를 구함
	// deg/sec * sec/frame
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	if (bLockOn)return;

	// 이번 프레임에 이동해야될 Pitch 각도를 구함
	// deg/sec * sec/frame
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRateInMouse(float Rate)
{
	if (bLockOn)return;

	AddControllerYawInput(Rate);
}

void APlayerCharacter::LookUpAtRateInMouse(float Rate)
{
	if (bLockOn)return;

	AddControllerPitchInput(Rate);
}

void APlayerCharacter::Attack(int32 MontageIndex)
{
	// 해당 함수는 Weapon이 존재해야 호출될 수 있는 함수임 (PressedAttack에서 검사함)
	// 공격 시작
	bShouldComboAttack = false;

	if (AnimInstance && AttackMontages.IsValidIndex(MontageIndex) && AttackMontages[MontageIndex])
	{
		AnimInstance->Montage_Play(AttackMontages[MontageIndex]);
	}

	CombatState = ECombatState::ECS_Attack;
}

void APlayerCharacter::SubAttack()
{
	if (EquippedShield) {
		CombatState = ECombatState::ECS_Guard;
	}
}

void APlayerCharacter::PressedAttack()
{
	bIsBattleMode = true;
	bPressedAttackButton = true;

	// 최초 공격
	if (CombatState == ECombatState::ECS_Unoccupied &&
		EquippedWeapon &&
		ST >= EquippedWeapon->GetAttackRequiredStamina()) {
		GetWorldTimerManager().ClearTimer(StaminaRecoveryDelayTimer);
		StopStaminaRecoveryTimer();

		Attack();
	}
}

void APlayerCharacter::ReleasedAttack()
{
	bPressedAttackButton = false;
}

void APlayerCharacter::CheckComboAttack()
{
	AttackCombo++;

	if ((bShouldChargedAttack || bShouldComboAttack) &&
		(ReadyToChargedAttackMontage && ChargedAttackMontage))
	{
		if (bShouldChargedAttack &&
			ST - EquippedWeapon->GetChargedAttackRequiredStamina() >= 0.f) {
			ResetAttack();

			PrepareChargedAttack();

			return;
		}
		else if (bShouldComboAttack &&
			AttackCombo < AttackMontages.Num() &&
			AttackMontages[AttackCombo] &&
			ST - EquippedWeapon->GetAttackRequiredStamina() >= 0.f) {
			Attack(AttackCombo);

			return;
		}
	}

	EndAttack();
}

void APlayerCharacter::EndAttack()
{
	ResetAttack();

	StartStaminaRecoveryDelayTimer();
}

void APlayerCharacter::CheckComboTimer()
{
	if (bPressedChargedAttackButton) {
		bShouldChargedAttack = true;
	}
	else if (bPressedAttackButton) {
		bShouldComboAttack = true;
	}

	GetWorldTimerManager().ClearTimer(ComboTimer);
}

void APlayerCharacter::StartComboTimer()
{
	// clearTimer가 호출되기 전까지 0.1초 마다 콤보를 확인하는 타이머
	GetWorldTimerManager().SetTimer(
		ComboTimer,
		this,
		&APlayerCharacter::CheckComboTimer,
		0.1f,
		true);
}

AWeapon* APlayerCharacter::SpawnDefaultWeapon()
{
	// 기본 무기를 생성한 뒤 반환한다.
	if (DefaultWeaponClass) {
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

AShield* APlayerCharacter::SpawnDefaultShield()
{
	// 기본 방패를 생성한 뒤 반환한다.
	if (DefaultShieldClass) {
		return GetWorld()->SpawnActor<AShield>(DefaultShieldClass);
	}
	return nullptr;
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon, bool bSwapping)
{
	// 무기를 RightHandSocket 위치에 부착한다.
	if (Weapon) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) {
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		EquippedWeapon = Weapon;

		Weapon->ChangeWeaponHandleLocation();

		// HUD에 연결된 아이템 아이콘을 변경한다. (한 손, 양손 무기 위치는 오른쪽)
		UpdateRightItemIcon();
	}
}

void APlayerCharacter::EquipShield(AShield* Shield, bool bSwapping)
{
	// 방패를 LeftHandSocket 위치에 부착한다.
	if (Shield) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("LeftHandSocket"));
		if (HandSocket) {
			HandSocket->AttachActor(Shield, GetMesh());
		}
		EquippedShield = Shield;

		// HUD에 연결된 아이템 아이콘을 변경한다. (방패는 왼쪽)
		UpdateLeftItemIcon();
	}
}

void APlayerCharacter::Roll()
{
	if (GetCharacterMovement()->IsFalling())return;
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (AnimInstance && RollMontage) {
		ST -= RollRequiredStamina;

		// 구르기 모션을 재생하고 상태를 바꾼다.
		AnimInstance->Montage_Play(RollMontage);
		CombatState = ECombatState::ECS_Roll;
	}
}

void APlayerCharacter::EndRoll()
{
	CombatState = ECombatState::ECS_Unoccupied;

	StartStaminaRecoveryDelayTimer();
}

void APlayerCharacter::PressedRoll()
{
	bPressedRollButton = true;

	if (CombatState == ECombatState::ECS_Unoccupied &&
		ST >= RollRequiredStamina) {
		GetWorldTimerManager().ClearTimer(StaminaRecoveryDelayTimer);
		StopStaminaRecoveryTimer();

		Roll();
	}
}

void APlayerCharacter::ReleasedRoll()
{
	bPressedRollButton = false;
}

void APlayerCharacter::RecoverStamina()
{
	if (ST + 1.f < MaximumST) {
		ST += 0.1f;
	}
	else {
		ST = MaximumST;
		StopStaminaRecoveryTimer();
	}
}

void APlayerCharacter::StartStaminaRecoveryTimer()
{
	GetWorldTimerManager().SetTimer(
		StaminaRecoveryTimer,
		this,
		&APlayerCharacter::RecoverStamina,
		0.01f,
		true);
}

void APlayerCharacter::StopStaminaRecoveryTimer()
{
	GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
}

void APlayerCharacter::StartStaminaRecoveryDelayTimer()
{
	GetWorldTimerManager().SetTimer(
		StaminaRecoveryDelayTimer,
		this,
		&APlayerCharacter::StartStaminaRecoveryTimer,
		StaminaRecoveryDelayTime);
}

void APlayerCharacter::ResetDamageState()
{
	EnemyDamageTypeResetDelegate.Broadcast();
}

void APlayerCharacter::PressedSubAttack()
{
	bIsBattleMode = true;
	bPressedSubAttackButton = true;

	if (CombatState == ECombatState::ECS_Unoccupied) {
		GetWorldTimerManager().ClearTimer(StaminaRecoveryDelayTimer);
		StopStaminaRecoveryTimer();

		SubAttack();
	}
}

void APlayerCharacter::ReleasedSubAttack()
{
	bPressedSubAttackButton = false;

	if (CombatState == ECombatState::ECS_Guard) {
		EndSubAttack();
	}
}

void APlayerCharacter::EndSubAttack()
{
	CombatState = ECombatState::ECS_Unoccupied;

	StartStaminaRecoveryDelayTimer();
}

void APlayerCharacter::SaveDegree()
{
	// 게임 패드 스틱의 방향
	const FVector2D ThumbstickAxis{ GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight") };
	const float ThumbstickDegree{ UKismetMathLibrary::DegAtan2(ThumbstickAxis.Y, ThumbstickAxis.X) };
	const FRotator ThumbstickRot{ 0,ThumbstickDegree,0 };

	// 정면 방향
	const FRotator ControllerRot{ 0,GetControlRotation().Yaw,0 };

	// 만약 패드 스틱을 조작하지 않았을 땐 Rotation을 저장할 필요가 없다. 어차피 각도가 같아 회전할 필요가 없기 때문
	if (ThumbstickAxis.X == 0.f && ThumbstickAxis.Y == 0.f) {
		return;
	}

	// 이외의 경우는 Rotation을 저장해야 한다.
	bIsBeforeAttackRotate = true;

	/*
	* 일단 한 번에 회전할 수 있는 최대 각도는 90도로 지정한다.
	* 90도를 넘어서지 않으면 정면 각도 + 스틱 각도의 결과를 rotation에 저장한다.
	* 90도가 넘어서면 좌, 우를 확인하여 플레이어 각도 +-90도의 rotation을 저장한다.
	*/

	// ControllerRot, ThumbstickRot을 결합하여 '정면이 기준일 때' 스틱의 회전을 구한다.
	const FRotator ComposeRot = UKismetMathLibrary::ComposeRotators(ControllerRot, ThumbstickRot);
	// 회전으로 정규화된 방향을 구한다.
	const FVector ComposeDirection = UKismetMathLibrary::Normal(ComposeRot.Vector());

	// Dot Product를 사용하여 액터의 정면과 각도 차이가 얼마나 나는지 확인한다. (값의 범위 : -1 ~ 1)
	const float DotProductValue{ UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(), ComposeDirection) };
	// 0보다 클 때, 각도가 0~90 내로 차이가 날 때
	if (DotProductValue >= 0.f) {
		// 정면 회전과 스틱 회전을 결합하여 최종 회전을 구한다.
		SaveRotator = UKismetMathLibrary::ComposeRotators(ControllerRot, ThumbstickRot);
	}

	// 0보다 작을 때, 각도가 90~180 내로 차이가 날 때 (최대 허용 각도를 90도로 제한함)
	else {
		// 이제는 Cross Product를 사용하여 최단 회전 방향이 왼쪽인지 오른쪽인지 구한다.
		const FVector CrossProductValue{ UKismetMathLibrary::Cross_VectorVector(GetActorForwardVector(), ComposeDirection) };

		// Z에 저장된 값이 음수면 왼쪽이 최단 회전 방향, 양수면 오른쪽이 최단 회전 방향이 된다.
		if (CrossProductValue.Z < 0.f) {
			SaveRotator = UKismetMathLibrary::ComposeRotators(GetActorRotation(), { 0,-90.f,0 });
		}
		else {
			SaveRotator = UKismetMathLibrary::ComposeRotators(GetActorRotation(), { 0,+90.f,0 });
		}
	}
}

void APlayerCharacter::BeginAttackRotate(float DeltaTime)
{
	// 각도가 거의 비슷해졌다면 회전을 종료한다.
	if (UKismetMathLibrary::EqualEqual_RotatorRotator(GetActorRotation(), SaveRotator, 0.01f)) {
		bIsBeforeAttackRotate = false;
		return;
	}

	// InterpTo를 사용하여 일정한 속도로 회전을 진행한다.
	SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), SaveRotator, DeltaTime, BeforeAttackRotateSpeed));
}

void APlayerCharacter::PressedBattleModeChange()
{
	if (CombatState == ECombatState::ECS_Unoccupied) {
		bIsBattleMode = !bIsBattleMode;
	}
}

void APlayerCharacter::PressedChargedAttack()
{
	bPressedChargedAttackButton = true;

	if (CombatState != ECombatState::ECS_Unoccupied)return;
	if (EquippedWeapon == nullptr)return;
	if (ST < EquippedWeapon->GetChargedAttackRequiredStamina())return;

	// 스태미나 검사
	// Attack 중에 호출할 수 있으며 마무리 공격처럼 사용할 수 있음.
	bIsBattleMode = true;

	ResetAttack();

	PrepareChargedAttack();
}

void APlayerCharacter::ReleasedChargedAttack()
{
	bPressedChargedAttackButton = false;
}

void APlayerCharacter::PrepareChargedAttack()
{
	// 해당 함수는 Weapon이 존재해야 호출될 수 있는 함수임 (PressedChargedAttack에서 검사함)
	CombatState = ECombatState::ECS_Interaction;
	bShouldChargedAttack = false;

	StopStaminaRecoveryTimer();
	// 준비 몽타주 플레이 후 ChargedAttack을 callable로 적용하여 nodify가 호출될 때 함수를 부르면 될듯.
	if (ReadyToChargedAttackMontage) {
		AnimInstance->Montage_Play(ReadyToChargedAttackMontage, 1.f);
	}
}

void APlayerCharacter::ChargedAttack()
{
	// Montage play?가 좋을듯 어차피 무기마다 애니메이션 속도를 다르게 하려면 필요하니까.
	if (ChargedAttackMontage) {
		CombatState = ECombatState::ECS_Attack;
		bIsChargedAttack = true;

		AnimInstance->Montage_Play(ChargedAttackMontage, 1.f);
	}
}

void APlayerCharacter::ResetAttack()
{
	GetWorldTimerManager().ClearTimer(ComboTimer);

	// 콤보를 초기화하고, 캐릭터 상태도 바꿔준다.
	AttackCombo = 0;
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::EndChargedAttack()
{
	CombatState = ECombatState::ECS_Unoccupied;
	bIsChargedAttack = false;
	StartStaminaRecoveryDelayTimer();
}

void APlayerCharacter::StartAttackCheckTime()
{
	// 0.005f 마다 함수를 호출함.
	GetWorldTimerManager().SetTimer(
		AttackCheckTimer,
		this,
		&APlayerCharacter::TracingAttackSphere,
		0.005f,
		true);
}

void APlayerCharacter::EndAttackCheckTime()
{
	GetWorldTimerManager().ClearTimer(AttackCheckTimer);
}

void APlayerCharacter::TracingAttackSphere()
{
	/* 아이템의 TopSocket과 BottomSocket의 위치를 받아온다. */
	const FVector TopSocketLoc{ EquippedWeapon->GetItemMesh()->GetSocketLocation("TopSocket") };
	const FVector BottomSocketLoc{ EquippedWeapon->GetItemMesh()->GetSocketLocation("BottomSocket") };

	FHitResult HitResult;
	// SphereTraceSingle로 원을 그리고 원에 겹치는 오브젝트를 HitResult에 담는다.
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		BottomSocketLoc,
		TopSocketLoc,
		50.f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ this },
		//EDrawDebugTrace::ForDuration,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	// Hit이 되었을 때
	if (bHit) {
		// Actor가 nullptr이 아니면
		if (HitResult.Actor != nullptr) {
			// 한 번 AEnemy로 Cast를 시도해본다.
			auto Enemy = Cast<AEnemy>(HitResult.Actor);

			// Enemy로 Cast가 성공적으로 됬을 때
			if (Enemy) {
				// Enemy가 데미지를 입을 수 있는 상태라면
				if (Enemy->GetDamageState() == EDamageState::EDS_Unoccupied) {
					// 상태 초기화 함수를 리셋 델리게이트에 넣고, 몬스터에 피해를 가한다.
					EnemyDamageTypeResetDelegate.AddUFunction(Enemy, FName("ResetDamageState"));

					UGameplayStatics::ApplyDamage(
						Enemy,
						(bIsChargedAttack ? EquippedWeapon->GetWeaponChargedDamage() : EquippedWeapon->GetWeaponDamage()),
						GetController(),
						this,
						UDamageType::StaticClass());
				}
			}
		}
	}
}

void APlayerCharacter::UseStaminaToAttack()
{
	if (bIsChargedAttack) {
		ST -= EquippedWeapon->GetChargedAttackRequiredStamina();
	}
	else {
		ST -= EquippedWeapon->GetAttackRequiredStamina();
	}
}

void APlayerCharacter::PressedLockOn()
{
	if (bLockOn) {
		bLockOn = false;
		LockOnWidgetData->SetVisibility(false);
		LockOnWidgetData = nullptr;
	}
	else {
		TArray<AActor*> OutActors;

		UKismetSystemLibrary::SphereOverlapActors(
			this,
			GetActorLocation(),
			1500.f,
			TraceObjectTypes,
			AEnemy::StaticClass(),
			{ this },
			OutActors);

		if (OutActors.Num() > 0) {

			const AEnemy* LockOnTarget{ GetNearestEnemyWithLockOn(OutActors) };

			if (LockOnTarget) {
				LockOnWidgetData = LockOnTarget->GetLockOnWidget();
				MinimumLockOnPitchValue = LockOnTarget->GetMinimumLockOnPitchValue();
			}
		}
	}

	if (LockOnWidgetData) {
		bLockOn = true;
		LockOnWidgetData->SetVisibility(true);
	}
}

AEnemy* APlayerCharacter::GetNearestEnemyWithLockOn(const TArray<AActor*> Actors)
{
	AEnemy* OutputEnemy{ nullptr };
	float LastDistance{ 0.f };

	for (AActor* Actor : Actors) {

		// 카메라 정면과 50도 이상 떨어져있는지 검사한다.
		const FVector PlayerToEnemyDirection{
			UKismetMathLibrary::Normal(Actor->GetActorLocation() - GetActorLocation()) };

		const FRotator ControlRot{ 0.f,GetControlRotation().Yaw,0.f };
		const FVector ControlForward{ UKismetMathLibrary::Normal(ControlRot.Vector()) };

		const float DotProductValue{
			UKismetMathLibrary::Dot_VectorVector(
				PlayerToEnemyDirection,
				ControlForward) };

		// 라디안 값을 디그리 값으로 바꿔준다.
		const float ValueToDegree{ UKismetMathLibrary::DegAcos(DotProductValue) };

		// 카메라 정면으로 부터 좌, 우 50도내에 있는 경우
		if (ValueToDegree <= 50.f) {
			AEnemy* NowEnemy{ Cast<AEnemy>(Actor) };

			const float NowDistance{
				UKismetMathLibrary::Vector_Distance(
					Actor->GetActorLocation(),
					GetActorLocation()) };

			// 이미 선택된 적이 있으면
			if (OutputEnemy) {

				// 이전 정보, 현재 정보 중 더 가까운 적을 타겟팅한다.
				if (NowDistance < LastDistance) {
					OutputEnemy = NowEnemy;
					LastDistance = NowDistance;
				}
			}
			// 선택된 적이 없으면
			else {
				OutputEnemy = NowEnemy;
				LastDistance = NowDistance;
			}
		}
	}

	return OutputEnemy;
}

void APlayerCharacter::RotateCameraByLockOn()
{
	FVector LockOnEnemyLoc{ LockOnWidgetData->GetComponentLocation() };
	LockOnEnemyLoc.Z -= 280.f;

	FRotator LookRotator{ UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockOnEnemyLoc) };

	// 너무 가까워 지면 보기가 힘들 수 있어 Pitch 최소 값을 clamp로 제한한다.
	LookRotator.Pitch = UKismetMathLibrary::ClampAngle(LookRotator.Pitch, MinimumLockOnPitchValue, 15.f);

	// Roll은 고정하고 Pitch와 Yaw만 이동한다.
	const FRotator FixLookRotator{
		LookRotator.Pitch,
		LookRotator.Yaw,
		GetActorRotation().Roll };

	// RLerp를 사용해 카메라를 부드럽게 전환시킨다.
	GetController()->SetControlRotation(
		UKismetMathLibrary::RLerp(
			GetControlRotation(),
			FixLookRotator,
			0.04f,
			true));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBeforeAttackRotate) {
		BeginAttackRotate(DeltaTime);
	}

	if (bLockOn) {
		RotateCameraByLockOn();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnAtRateInMouse);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUpAtRateInMouse);

	PlayerInputComponent->BindAction("AttackButton", IE_Pressed, this, &APlayerCharacter::PressedAttack);
	PlayerInputComponent->BindAction("AttackButton", IE_Released, this, &APlayerCharacter::ReleasedAttack);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::PressedRoll);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &APlayerCharacter::ReleasedRoll);

	PlayerInputComponent->BindAction("SubAttackButton", IE_Pressed, this, &APlayerCharacter::PressedSubAttack);
	PlayerInputComponent->BindAction("SubAttackButton", IE_Released, this, &APlayerCharacter::ReleasedSubAttack);

	PlayerInputComponent->BindAction("BattleModeChangeButton", IE_Pressed, this, &APlayerCharacter::PressedBattleModeChange);

	PlayerInputComponent->BindAction("ChargedAttackButton", IE_Pressed, this, &APlayerCharacter::PressedChargedAttack);
	PlayerInputComponent->BindAction("ChargedAttackButton", IE_Released, this, &APlayerCharacter::ReleasedChargedAttack);

	PlayerInputComponent->BindAction("LockOnButton", IE_Pressed, this, &APlayerCharacter::PressedLockOn);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDying)return DamageAmount;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}
