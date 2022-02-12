// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMeleeCharacter::AMeleeCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bShouldComboAttack(false),
	CombatState(ECombatState::ECS_Unoccupied),
	AttackCombo(0),
	MaximumAttackCombo(3),
	// Stat
	HP(100),
	MaximumHP(100),
	AD(0),
	AP(0),
	DEF(0)
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
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	EquipWeapon(SpawnDefaultWeapon());
	EquippedWeapon->SetCharacter(this);
}

void AMeleeCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f) {
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 전방인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.f) {
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
	bShouldComboAttack = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontages[MontageIndex])
	{
		AnimInstance->Montage_Play(AttackMontages[MontageIndex]);
	}

	CombatState = ECombatState::ECS_Attack;
}

void AMeleeCharacter::PressedAttack()
{
	bPressedAttackButton = true;

	// 공격 시작
	if (CombatState == ECombatState::ECS_Unoccupied) 
	{
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
	if (bShouldComboAttack && AttackCombo < AttackMontages.Num() && AttackMontages[AttackCombo])
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
	if (DefaultWeaponClass) {
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AMeleeCharacter::EquipWeapon(AWeapon* Weapon, bool bSwapping)
{
	if (Weapon) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) {
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		EquippedWeapon = Weapon;
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
}

