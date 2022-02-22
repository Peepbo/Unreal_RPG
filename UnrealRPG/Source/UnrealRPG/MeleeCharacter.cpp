// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Shield.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MeleeAnimInstance.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMeleeCharacter::AMeleeCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bShouldComboAttack(false),
	CombatState(ECombatState::ECS_Unoccupied),
	AttackCombo(0.f),
	// Stat
	HP(100.f),
	MaximumHP(100.f),
	AD(0.f),
	ST(50.f),
	MaximumST(50.f),
	bIsSprint(false),
	MaximumWalkSpeed(350.f),
	MaximumSprintSpeed(800.f),
	StaminaRecoveryDelayTime(0.3f),
	bPressedSprintButton(false),
	bPressedRollButton(false),
	bPressedSubAttackButton(false)
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

// Called when the game starts or when spawned
void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	EquipWeapon(SpawnDefaultWeapon());
	EquippedWeapon->SetCharacter(this);
	EquipShield(SpawnDefaultShield());
	EquippedShield->SetCharacter(this);
	
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UMeleeAnimInstance* MeleeAnimInst = Cast<UMeleeAnimInstance>(AnimInst);
		if (MeleeAnimInst) {
			AnimInstance = MeleeAnimInst;
		}
	}

	auto WeaponCollision = EquippedWeapon->GetWeaponCollision();
	if (WeaponCollision) {
		WeaponCollision->OnComponentBeginOverlap.AddDynamic(
			this,
			&AMeleeCharacter::OnRightWeaponOverlap);
	}
}

void AMeleeCharacter::MoveForward(float Value)
{
	// 공격중일 때 이동을 제한한다.
	if (CombatState == ECombatState::ECS_Attack)return;
	if (Controller && Value != 0.f) {
		//if (bIsSprint) {
		//	// 스프린트 상태일 때 음수면 -1, 양수면 1을 반환하여 속도를 고정시킨다.
		//	// (패드에서 엄지 그립 위치에 따라 속도가 변화하는것을 방지)
		//	Value = Value < 0.f ? -1.f : 1.f;
		//}

		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 전방인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::MoveRight(float Value)
{
	// 공격중일 때 이동을 제한한다.
	if (CombatState == ECombatState::ECS_Attack)return;
	if (Controller && Value != 0.f) {
		//if (bIsSprint) {
		//	// 스프린트 상태일 때 음수면 -1, 양수면 1을 반환하여 속도를 고정시킨다.
		//	// (패드에서 엄지 그립 위치에 따라 속도가 변화하는것을 방지)
		//	Value = Value < 0.f ? -1.f : 1.f;
		//}

		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 우측인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::TurnAtRate(float Rate)
{
	// 이번 프레임에 이동해야될 Yaw 각도를 구함
	// deg/sec * sec/frame
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::LookUpAtRate(float Rate)
{
	// 이번 프레임에 이동해야될 Pitch 각도를 구함
	// deg/sec * sec/frame
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::Attack(int32 MontageIndex)
{
	// 공격 시작
	bShouldComboAttack = false;
	ST -= 10.f;

	if (AnimInstance && AttackMontages.IsValidIndex(MontageIndex) && AttackMontages[MontageIndex])
	{
		AnimInstance->Montage_Play(AttackMontages[MontageIndex]);
	}

	CombatState = ECombatState::ECS_Attack;
}

void AMeleeCharacter::SubAttack()
{
	if (EquippedShield) {
		CombatState = ECombatState::ECS_Guard;
		// Montage Play
	}
}

void AMeleeCharacter::PressedAttack()
{
	bPressedAttackButton = true;

	// 최초 공격
	if(CombatState == ECombatState::ECS_Unoccupied &&
		ST - 10.f > 0.f) {
		GetWorldTimerManager().ClearTimer(StaminaRecoveryDelayTimer);
		StopStaminaRecoveryTimer();
		EndSprint(true);

		Attack();
	}
}

void AMeleeCharacter::ReleasedAttack()
{
	bPressedAttackButton = false;
}

void AMeleeCharacter::CheckComboAttack()
{
	AttackCombo++;
	if (bShouldComboAttack && 
		AttackCombo < AttackMontages.Num() && 
		AttackMontages[AttackCombo] &&
		ST - 10.f > 0.f)
	{
		Attack(AttackCombo);
	}
	else
	{
		EndAttack();
	}
}

void AMeleeCharacter::EndAttack()
{
	GetWorldTimerManager().ClearTimer(ComboTimer);

	// 콤보를 초기화하고, 캐릭터 상태도 바꿔준다.
	AttackCombo = 0;
	CombatState = ECombatState::ECS_Unoccupied;

	StartStaminaRecoveryDelayTimer();
}

void AMeleeCharacter::CheckComboTimer()
{
	if (bPressedAttackButton) {
		bShouldComboAttack = true;
		GetWorldTimerManager().ClearTimer(ComboTimer);
	}
}

void AMeleeCharacter::StartComboTimer()
{
	// clearTimer가 호출되기 전까지 0.1초 마다 콤보를 확인하는 타이머
	GetWorldTimerManager().SetTimer(
		ComboTimer,
		this,
		&AMeleeCharacter::CheckComboTimer,
		0.1f,
		true);
}

AWeapon* AMeleeCharacter::SpawnDefaultWeapon()
{
	// 기본 무기를 생성한 뒤 반환한다.
	if (DefaultWeaponClass) {
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

AShield* AMeleeCharacter::SpawnDefaultShield()
{
	// 기본 방패를 생성한 뒤 반환한다.
	if (DefaultShieldClass) {
		return GetWorld()->SpawnActor<AShield>(DefaultShieldClass);
	}
	return nullptr;
}

void AMeleeCharacter::EquipWeapon(AWeapon* Weapon, bool bSwapping)
{
	// 무기를 RightHandSocket 위치에 부착한다.
	if (Weapon) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) {
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		EquippedWeapon = Weapon;

		// HUD에 연결된 아이템 아이콘을 변경한다. (한 손, 양손 무기 위치는 오른쪽)
		UpdateRightItemIcon();
	}
}

void AMeleeCharacter::EquipShield(AShield* Shield, bool bSwapping)
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

void AMeleeCharacter::Roll()
{
	if (GetCharacterMovement()->IsFalling())return;
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (AnimInstance && RollMontage) {
		ST -= 10.f;
		
		// 구르기 모션을 재생하고 상태를 바꾼다.
		AnimInstance->Montage_Play(RollMontage);
		CombatState = ECombatState::ECS_Roll;
	}
}

void AMeleeCharacter::EndRoll()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
	StartStaminaRecoveryDelayTimer();
}

void AMeleeCharacter::PressedRoll()
{
	bPressedRollButton = true;

	if (CombatState == ECombatState::ECS_Unoccupied &&
		ST > 10.f) {
		GetWorldTimerManager().ClearTimer(StaminaRecoveryDelayTimer);
		StopStaminaRecoveryTimer();
		EndSprint(true);

		Roll();
	}
}

void AMeleeCharacter::ReleasedRoll()
{
	bPressedRollButton = false;
}

void AMeleeCharacter::Sprint()
{
	if (GetCharacterMovement()->IsFalling())return;
	if (CombatState != ECombatState::ECS_Unoccupied)return;

	if (AnimInstance &&
		AnimInstance->GetSpeed() > 0.f &&
		ST > 1.f) {
		bIsSprint = true;

		// 최대 속도를 스프린트 속도로 바꿔준다.
		GetCharacterMovement()->MaxWalkSpeed = MaximumSprintSpeed;

		// 스태미나 타이머를 멈춘다.
		StopStaminaRecoveryTimer();
		// 질주 스태미나 타이머를 시작한다.
		StartStaminaReductionTimer();
	}
}

void AMeleeCharacter::EndSprint(bool bChangeState)
{
	if (bIsSprint) {
		bIsSprint = false;

		// 속도 확인을 멈춘다.
		GetWorldTimerManager().ClearTimer(VelocityChecker);

		// 최대 속도를 기본 속도로 되돌린다.
		GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;

		// 스프린트 스태미나 타이머를 멈춘다.
		StopStaminaReductionTimer();

		if (!bChangeState) {
			StartStaminaRecoveryDelayTimer();
		}
	}
}

void AMeleeCharacter::RecoverStamina()
{
	if (ST + 1.f < MaximumST) {
		ST += 0.1f;
	}
	else {
		ST = MaximumST;
		StopStaminaRecoveryTimer();
	}

}

void AMeleeCharacter::StartStaminaRecoveryTimer()
{
	GetWorldTimerManager().SetTimer(
		StaminaRecoveryTimer,
		this,
		&AMeleeCharacter::RecoverStamina,
		0.01f,
		true);
}

void AMeleeCharacter::StopStaminaRecoveryTimer()
{
	GetWorldTimerManager().ClearTimer(StaminaRecoveryTimer);
}

void AMeleeCharacter::StartStaminaRecoveryDelayTimer()
{
	GetWorldTimerManager().SetTimer(
		StaminaRecoveryDelayTimer,
		this,
		&AMeleeCharacter::StartStaminaRecoveryTimer,
		StaminaRecoveryDelayTime);
}

void AMeleeCharacter::StartStaminaReductionTimer()
{
	GetWorldTimerManager().SetTimer(
		StaminaReductionTimer,
		this,
		&AMeleeCharacter::ReduceStamina,
		0.01f,
		true);
}

void AMeleeCharacter::StopStaminaReductionTimer()
{
	GetWorldTimerManager().ClearTimer(StaminaReductionTimer);
}

void AMeleeCharacter::ReduceStamina()
{
	if (ST > 0.1f) {
		ST -= 0.1f;
	}
	else {
		EndSprint();
	}
}

void AMeleeCharacter::PressedSprint()
{
	bPressedSprintButton = true;
	Sprint();

	//VelocityChecker
	GetWorldTimerManager().SetTimer(
		VelocityChecker,
		this,
		&AMeleeCharacter::CheckVelocity,
		0.1f,
		true, 
		1.f);
}

void AMeleeCharacter::ReleasedSprint()
{
	bPressedSprintButton = false;
	EndSprint();
}

void AMeleeCharacter::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CombatState != ECombatState::ECS_Attack)return;

	if (OtherActor) {
		auto Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy && 
			Enemy->GetDamageState() == EDamageState::EDS_Unoccupied) {
			// DoDamage
			UGameplayStatics::ApplyDamage(
				Enemy,
				10,
				GetController(),
				this,
				UDamageType::StaticClass()
			);

			EnemyDamageTypeResetDelegate.AddUFunction(Enemy, FName("ResetDamageState"));
		}
	}
}

void AMeleeCharacter::ResetDamageState()
{
	EnemyDamageTypeResetDelegate.Broadcast();
}

void AMeleeCharacter::PressedSubAttack()
{
	bPressedSubAttackButton = true;

	if (CombatState == ECombatState::ECS_Unoccupied) {
		SubAttack();
	}
}

void AMeleeCharacter::ReleasedSubAttack()
{
	bPressedSubAttackButton = false;

	if (CombatState == ECombatState::ECS_Guard) {
		EndSubAttack();
	}
}

void AMeleeCharacter::EndSubAttack()
{
	CombatState = ECombatState::ECS_Unoccupied;
	// End Montage
}

void AMeleeCharacter::CheckVelocity()
{
	if (GetCharacterMovement()->Velocity.Size() == 0.f) {
		EndSprint();
	}
}

// Called every frame
void AMeleeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMeleeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMeleeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMeleeCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMeleeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMeleeCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("AttackButton", IE_Pressed, this, &AMeleeCharacter::PressedAttack);
	PlayerInputComponent->BindAction("AttackButton", IE_Released, this, &AMeleeCharacter::ReleasedAttack);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AMeleeCharacter::PressedRoll);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &AMeleeCharacter::ReleasedRoll);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMeleeCharacter::PressedSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMeleeCharacter::ReleasedSprint);

	PlayerInputComponent->BindAction("SubAttackButton", IE_Pressed, this, &AMeleeCharacter::PressedSubAttack);
	PlayerInputComponent->BindAction("SubAttackButton", IE_Released, this, &AMeleeCharacter::ReleasedSubAttack);
}

