// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/Shield.h"
#include "Items/Potion.h"
#include "Player/PlayerAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Player/SavePoint.h"
#include "GameFramework/PlayerStart.h"
#include "MeleePlayerController.h"
#include "ExecutionArea.h"


APlayerCharacter::APlayerCharacter() :
	// Camera
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),

	// Control
	bControl(false),

	// Stamina
	Stamina(50.f),
	MaximumStamina(50.f),
	StaminaRecoveryAmount(0.2f),
	DefaultStaminaRecoveryAmount(0.2f),
	SprintStaminaRecoveryAmount(0.1f),
	StaminaRecoveryDelayTime(1.f),
	StaminaRecoverySpeed(0.01f),
	GuardStaminaRecoveryAmount(0.1f),
	bRecoverStamina(false),

	// Attack
	bShouldContinueAttack(false),
	ComboAttackMontageIndex(0),
	ChargedComboAttackMontageIndex(0),
	bPressedRollAndSprintButton(false),
	bPressedSubAttackButton(false),
	BeforeAttackRotateSpeed(10.f),
	bIsBeforeAttackRotate(false),
	bIsChargedAttack(false),
	bShouldChargedAttack(false),
	StopAttackMontageBlendOutValue(0.25f),
	MaximumAttackIndex(1),
	AttackCheckTimeDelay(0.0025f),

	// LockOn
	bLockOn(false),
	LockOnCameraSpeed(0.04f),

	// Jump
	bPressedJumpButton(false),
	MaximumZVelocity(0.f),
	JumpRequiredStamina(10.f),

	// Roll
	LastRollMoveValue(0.f, 0.f),
	bBackDodge(false),
	bCanRoll(true),
	RollDelay(2.1f),
	RollRequiredStamina(10.f),
	bShouldPlayRoll(false),

	// Potion
	bDrinkingPotion(false),

	MoveValue(0.f, 0.f),
	LastMoveValue(0.f, 0.f),
	PlayerAttackType(EPlayerAttackType::EPAT_Weapon),
	WeaponAttackType(EWeaponAttackType::EWAT_Normal),

	// SocketName
	LeftHandSocketName("LeftHandSocket"),
	RightHandSocketName("RightHandSocket"),
	PotionSocketName("LeftPotionSocket"),

	// Guard
	bGuardBreak(false),

	//IK_Foot
	IKLeftFootOffset(0.f),
	IKRightFootOffset(0.f),
	IKHipOffset(0.f),
	IKTraceDistance(0.f),
	IKInterpSpeed(15.f),
	IKLeftFootRotator(0.f, 0.f, 0.f),
	IKRightFootRotator(0.f, 0.f, 0.f),
	IKFootAlpha(0.f),

	//bEventAble
	bEventAble(false),
	bRest(false),
	RestEndPoint(0.f, 0.f, 0.f),
	ToRestRotator(0.f, 0.f, 0.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 붐 생성 및 특정 값 초기화
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.f;
	// 폰이 회전할 때 카메라 붐도 같이 회전합니다.
	CameraBoom->bUsePawnControlRotation = true;

	// 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation({ 0.f,0.f,20.f });
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


	ListenerComponent = CreateDefaultSubobject<USceneComponent>("Listener");
	ListenerComponent->SetupAttachment(GetMesh(), FName("head"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	EquipWeapon(SpawnDefaultWeapon());
	if (EquippedWeapon) {
		EquippedWeapon->SetCharacter(this);
	}
	EquipShield(SpawnDefaultShield());
	if (EquippedShield) {
		EquippedShield->SetCharacter(this);
	}
	EquipPotion(SpawnDefaultPotion());
	if (EquippedPotion) {
		EquippedPotion->SetCharacter(this);
	}

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst) {
		UPlayerAnimInstance* PlayerAnimInst = Cast<UPlayerAnimInstance>(AnimInst);
		if (PlayerAnimInst) {
			AnimInstance = PlayerAnimInst;
		}
	}

	// 락온에 필요한 트레이스 타입 정보를 미리 만들어둠
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	// IK Variable 초기화
	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	PlayerController = Cast<AMeleePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!bControl)return;
	if (CombatState == ECombatState::ECS_Attack)return;
	if (CombatState == ECombatState::ECS_Roll)return;
	if (CombatState == ECombatState::ECS_Impact)return;
	if (CombatState == ECombatState::ECS_NonMovingInteraction)return;
	if (CombatState == ECombatState::ECS_RestInteraction)return;
	
	if (Controller && Value != 0.f) {
		if (CheckActionableState() && !GetSprinting() && bLockOn) 
		{
			ApplyLockOnAttackSetting(false);
		}

		//EndToIdleState(true);
		if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.2f);
		}

		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 전방인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

		LastMoveValue.X = MoveValue.X;
		MoveValue.X = Value;
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!bControl)return;
	if (CombatState == ECombatState::ECS_Attack)return;
	if (CombatState == ECombatState::ECS_Roll)return;
	if (CombatState == ECombatState::ECS_Impact)return;
	if (CombatState == ECombatState::ECS_NonMovingInteraction)return;
	if (CombatState == ECombatState::ECS_RestInteraction)return;

	if (Controller && Value != 0.f) {
		if (CheckActionableState() && !GetSprinting() && bLockOn)
		{
			ApplyLockOnAttackSetting(false);
		}

		//EndToIdleState(true);
		if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.2f);
		}

		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		// 어느쪽이 우측인지 알아내고, 그 방향으로 이동
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };

		LastMoveValue.Y = MoveValue.Y;
		MoveValue.Y = Value;
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	if (!bControl)return;
	if (bLockOn || GetAttacking())
	{
		return;
	}

	// 이번 프레임에 이동해야될 Yaw 각도를 구함
	// deg/sec * sec/frame
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	if (!bControl)return;
	if (bLockOn || GetAttacking())
	{
		return;
	}

	// 이번 프레임에 이동해야될 Pitch 각도를 구함
	// deg/sec * sec/frame
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRateInMouse(float Rate)
{
	if (!bControl)return;
	if (bLockOn)
	{
		return;
	}

	AddControllerYawInput(Rate);
}

void APlayerCharacter::LookUpAtRateInMouse(float Rate)
{
	if (!bControl)return;
	if (bLockOn)
	{
		return;
	}

	AddControllerPitchInput(Rate);
}

bool APlayerCharacter::MainAttack()
{
	if (Stamina >= EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Normal))
	{
		EndToIdleState(false);
		UseStaminaAndStopRecovery(EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Normal));

		AnimInstance->Montage_Play(MainComboMontages[ComboAttackMontageIndex]);
		CombatState = ECombatState::ECS_Attack;

		PlayerAttackType = EPlayerAttackType::EPAT_Weapon;
		WeaponAttackType = EWeaponAttackType::EWAT_Normal;

		ComboAttackMontageIndex++;
		return true;
	}

	return false;
}

void APlayerCharacter::SubAttack()
{
	StaminaRecoveryAmount = GuardStaminaRecoveryAmount;
	CombatState = ECombatState::ECS_Guard;
}

void APlayerCharacter::PressedAttack()
{
	bPressedAttackButton = true;

	if (!bControl)return;

	if (!CheckActionableState() || EquippedWeapon == nullptr || AnimInstance == nullptr)
	{
		return;
	}

	// 모든 공격 조건이 성립하면
	if (bLockOn)
	{
		ApplyLockOnAttackSetting(true);
	}

	if (!CheckLand()) 
	{
		PrepareJumpAttack();
	}
	else
	{
		const FVector2D ThumbAxis{ GetMovementLocalAxis() };

		// 질주 상태가 아닐경우 일반 공격 실행
		const bool bPlayMainAttack{ GetSprinting() ? false : true };

		if (bPlayMainAttack)
		{
			MainAttack();
		}
		else
		{
			DashAttack();
		}
	}
}

void APlayerCharacter::ReleasedAttack()
{
	bPressedAttackButton = false;
}

bool APlayerCharacter::CheckComboAttack()
{
	bool MontageValid;
	bool bPlayAttack;

	// 일반 공격
	if (bShouldContinueAttack)
	{
		bShouldContinueAttack = false;

		MontageValid = MainComboMontages.IsValidIndex(ComboAttackMontageIndex) && MainComboMontages[ComboAttackMontageIndex];
		if (MontageValid)
		{
			bPlayAttack = MainAttack();

			if (bPlayAttack) return true;
		}
	}
	// 강 공격
	else if (bShouldChargedAttack)
	{
		bShouldChargedAttack = false;

		MontageValid = ChargedComboMontages.IsValidIndex(ChargedComboAttackMontageIndex) && ChargedComboMontages[ChargedComboAttackMontageIndex];
		if (MontageValid)
		{
			bPlayAttack = ChargedAttack();

			if (bPlayAttack) return true;
		}
	}

	// 아직 return이 되지 않았다면 EndAttack을 호출한다.
	EndAttack();
	return false;
}

void APlayerCharacter::EndAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("End Attack Call"));
	ResetAttack();
}

void APlayerCharacter::CheckComboTimer()
{
	// 여기에 스테미나 검사까지 포함해야됨
	if (bPressedChargedAttackButton) 
	{
		bShouldChargedAttack = true;

		GetWorldTimerManager().ClearTimer(ComboTimer);
	}
	else if (bPressedAttackButton)
	{
		bShouldContinueAttack = true;

		GetWorldTimerManager().ClearTimer(ComboTimer);
	}
}

void APlayerCharacter::StartComboTimer()
{
	// clearTimer가 호출되기 전까지 0.1초 마다 콤보를 확인하는 타이머
	GetWorldTimerManager().SetTimer(
		ComboTimer,
		this,
		&APlayerCharacter::CheckComboTimer,
		0.01f,
		true);
}

AWeapon* APlayerCharacter::SpawnDefaultWeapon()
{
	// 기본 무기를 생성한 뒤 반환한다.
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

AShield* APlayerCharacter::SpawnDefaultShield()
{
	// 기본 방패를 생성한 뒤 반환한다.
	if (DefaultShieldClass) 
	{
		return GetWorld()->SpawnActor<AShield>(DefaultShieldClass);
	}
	return nullptr;
}

APotion* APlayerCharacter::SpawnDefaultPotion()
{
	if (DefaultPotionClass)
	{
		return GetWorld()->SpawnActor<APotion>(DefaultPotionClass);
	}
	return nullptr;
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon, bool bSwapping)
{
	// 무기를 RightHandSocket 위치에 부착한다.
	if (Weapon)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(RightHandSocketName);
		if (HandSocket)
		{
			HandSocket->AttachActor(Weapon, GetMesh());
		}
		EquippedWeapon = Weapon;

		// HUD에 연결된 아이템 아이콘을 변경한다. (한 손, 양손 무기 위치는 오른쪽)
		UpdateRightItemIcon();
	}
}

void APlayerCharacter::EquipShield(AShield* Shield, bool bSwapping)
{
	// 방패를 LeftHandSocket 위치에 부착한다.
	if (Shield) {
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(LeftHandSocketName);
		if (HandSocket) 
		{
			HandSocket->AttachActor(Shield, GetMesh());
		}
		EquippedShield = Shield;

		// HUD에 연결된 아이템 아이콘을 변경한다. (방패는 왼쪽)
		UpdateLeftItemIcon();
	}
}

void APlayerCharacter::EquipPotion(APotion* Potion, bool bSwapping)
{
	if (Potion) 
	{
		const USkeletalMeshSocket* PotionSocket = GetMesh()->GetSocketByName(PotionSocketName);
		if (PotionSocket) 
		{
			PotionSocket->AttachActor(Potion, GetMesh());
		}
		EquippedPotion = Potion;

		// HUD에 연결된 아이템 아이콘을 변경한다. (포션은 아래)
		UpdateBottomItemIcon();
	}
}

void APlayerCharacter::Roll()
{
	if (!bControl)return;

	if (AnimInstance) {
		UseStaminaAndStopRecovery(RollRequiredStamina);

		const FVector2D MoveAxis{ GetMovementLocalAxis() };
		if (MoveAxis.IsNearlyZero())
		{
			bBackDodge = true;
		}

		if (!bLockOn)
		{
			const FRotator ThumbStickRot{ 0,GetMovementDegree(),0 };
			const FRotator ControllRot{ 0,GetControlRotation().Yaw,0 };
			const FRotator ComposeRot{ UKismetMathLibrary::ComposeRotators(ControllRot,ThumbStickRot) };
			SetActorRotation(ComposeRot);
		}

		LastRollMoveValue = GetMovementLocalAxis();

		// 상태를 바꾼다.
		CombatState = ECombatState::ECS_Roll;
	}
}

void APlayerCharacter::EndRoll()
{
	//CombatState = ECombatState::ECS_Unoccupied;

	bBackDodge = false;

	GetWorldTimerManager().SetTimer(
		RollDelayTimer,
		this,
		&APlayerCharacter::StopDelayForRoll,
		0.2f,
		false,
		RollDelay);
}

void APlayerCharacter::PressedRollAndSprint()
{
	bPressedRollAndSprintButton = true;

	if (!bControl)return;

	// 구르기와 뛰기는 둘 다 특정 모션을 취하고 있으면 안되고, 공중에 있으면 안된다. 
	if (!CheckActionableState())return;
	if (GetCharacterMovement()->IsFalling())return;
	//if (!bCanRoll)return;
	// 여기에 스테미나 검사도 포함

	bCanRoll = false;

	//EndToIdleState(true);
	ForceStopAllMontage();

	UE_LOG(LogTemp, Warning, TEXT("롤 앤 스프린트 타이머 시작"));

	// 0.5초 내에 키를 제거했을 때 = 구르기, 0.5초가 지나 Sprint함수를 출력했을 때 = 뛰기
	GetWorldTimerManager().SetTimer(
		SprintAndRollPlayTimer,
		this,
		&APlayerCharacter::Sprint,
		0.5f,
		false,
		0.5f);
}

void APlayerCharacter::ReleasedRollAndSprint()
{
	bPressedRollAndSprintButton = false;

	if (!bControl)return;
	if (!CheckActionableState())return;
	if (GetCharacterMovement()->IsFalling())return;

	// 1. 뛰고 있을 때(sprint)
	if (GetSprinting())
	{
		EndSprint();
	}
	// 2. 일반 상태
	else {
		// 아직 타이머가 진행중이면 (0.5초 이내에 키를 놨을 때)
		if (GetWorldTimerManager().IsTimerActive(SprintAndRollPlayTimer))
		{
			GetWorldTimerManager().ClearTimer(SprintAndRollPlayTimer);

			// 몇가지 조건을 비교하고 구르기를 실행한다.
			/*
			* 한번 더 검사하는 이유?
			*	Pressed 호출시 땅에 있었고 특정 모션을 취하지 않았지만
			*	Released 호출시에 공중에 있거나 특정 모션을 취하면 구르기를 하면 안되기 때문
			*/
			if (!GetCharacterMovement()->IsFalling() &&
				CheckActionableState() &&
				Stamina >= RollRequiredStamina)
			{
				if (AnimInstance)
				{
					AnimInstance->StopAllMontages(0.f);
				}

				Roll();
			}
			else 
			{
				bCanRoll = true;
			}
		}
	}
}

void APlayerCharacter::PrepareShieldAttack()
{
	PlayerAttackType = EPlayerAttackType::EPAT_Shield;
}

void APlayerCharacter::RecoverStamina()
{
	if (Stamina + StaminaRecoveryAmount < MaximumStamina)
	{
		Stamina += StaminaRecoveryAmount;
	}
	else 
	{
		Stamina = MaximumStamina;
	}
}

void APlayerCharacter::StartStaminaRecovery()
{
	bRecoverStamina = true;
}

void APlayerCharacter::StopStaminaRecovery()
{
	bRecoverStamina = false;

	GetWorldTimerManager().SetTimer(
		StaminaRecoveryDelayTimer,
		this,
		&APlayerCharacter::StartStaminaRecovery,
		StaminaRecoveryDelayTime);
}

void APlayerCharacter::UseStaminaAndStopRecovery(float UseStamina)
{
	Stamina -= UseStamina;
	StopStaminaRecovery();
}

void APlayerCharacter::ResetEnemyDamageState()
{
	EnemyDamageTypeResetDelegate.Broadcast();
}

void APlayerCharacter::PressedSubAttack()
{
	bPressedSubAttackButton = true;

	if (!bControl)return;

	if (EquippedShield == nullptr)return;

	if (CheckActionableState())
	{
		ForceStopAllMontage();

		EndToIdleState(false);

		SubAttack();
	}
}

void APlayerCharacter::ReleasedSubAttack()
{
	bPressedSubAttackButton = false;

	// Impact중에는 Shield를 종료할 수 없음. (Impact 종료 후 Released를 다시 검사함)
	if (CombatState == ECombatState::ECS_Guard && !bIsShieldImpact)
	{
		EndSubAttack();
	}
}

void APlayerCharacter::EndSubAttack()
{
	StaminaRecoveryAmount = DefaultStaminaRecoveryAmount;

	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::SaveDegree()
{
	// 방향키의 방향
	const FVector2D MovementAxis{ GetMovementLocalAxis() };
	const float MovementDegree{ GetMovementDegree() };
	const FRotator MovementRotation{ 0,MovementDegree,0 };

	// 정면 방향
	const FRotator ControllerRotation{ 0,GetControlRotation().Yaw,0 };

	// 만약 방향키를 이동하지 않았을 땐 Rotation을 저장할 필요가 없다. 어차피 각도가 같아 회전할 필요가 없기 때문
	if (MovementAxis.IsNearlyZero()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[Player:SaveDegree]Axis is nearly zero : {%f,%f}"), MovementAxis.X, MovementAxis.Y);
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
	const FVector2D ComposeDirection{ GetAxisOfMovementRelativeController() };
	const FVector ComposeDirection3D{ ComposeDirection.X,ComposeDirection.Y,0.f };

	// Dot Product를 사용하여 액터의 정면과 각도 차이가 얼마나 나는지 확인한다. (값의 범위 : -1 ~ 1)
	const float DotProductValue{ UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(), ComposeDirection3D) };

	/* DotProduct Value(Radian)
	* 
	*				 1 (Forward)
	*				 ↑
	* (Leftward) 0 ←   → 0 (Rightward)
	*				 ↓
	*				-1 (Backward)
	*/
	// 0보다 클 때? 각도가 0~90 내로 차이가 날 때
	if (DotProductValue >= 0.f) {
		// 정면 회전과 스틱 회전을 결합하여 최종 회전을 구한다.
		SaveRotator = UKismetMathLibrary::ComposeRotators(ControllerRotation, MovementRotation);
	}

	// 0보다 작을 때, 각도가 90~180 내로 차이가 날 때 (최대 허용 각도를 90도로 제한함)
	else {
		// 이제는 Cross Product를 사용하여 최단 회전 방향이 왼쪽인지 오른쪽인지 구한다.
		const FVector CrossProductValue{ UKismetMathLibrary::Cross_VectorVector(GetActorForwardVector(), ComposeDirection3D) };

		// Z에 저장된 값이 음수면 왼쪽이 최단 회전 방향, 양수면 오른쪽이 최단 회전 방향이 된다.
		if (CrossProductValue.Z < 0.f) {
			SaveRotator = UKismetMathLibrary::ComposeRotators(GetActorRotation(), { 0,-90.f,0 });
		}
		else {
			SaveRotator = UKismetMathLibrary::ComposeRotators(GetActorRotation(), { 0,+90.f,0 });
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Start Rot"));
}

void APlayerCharacter::BeginAttackRotate(float DeltaTime)
{
	// InterpTo를 사용하여 일정한 속도로 회전을 진행한다.
	SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), SaveRotator, DeltaTime, BeforeAttackRotateSpeed));
}

void APlayerCharacter::PressedChargedAttack()
{
	bPressedChargedAttackButton = true;

	if (!bControl)return;
	if (!CheckActionableState())return;
	if (EquippedWeapon == nullptr)return;
	if (!CheckLand())return;

	PrepareChargedAttack();
}

void APlayerCharacter::ReleasedChargedAttack()
{
	bPressedChargedAttackButton = false;
}

void APlayerCharacter::EndSprint()
{
	StaminaRecoveryAmount = DefaultStaminaRecoveryAmount;
	ChangeMoveState(false);
}

void APlayerCharacter::PrepareChargedAttack()
{
	if (Stamina >= EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Charged))
	{
		// 0번은 차지 공격 준비 자세
		if (ChargedComboMontages[0])
		{
			EndToIdleState(false);
			ResetAttack();

			CombatState = ECombatState::ECS_NonMovingInteraction;
			bShouldChargedAttack = false;


			// 준비 몽타주(0번 애니메이션) 플레이
			ChargedAttack();
		}
	}
}

bool APlayerCharacter::ChargedAttack()
{
	if (Stamina >= EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Charged))
	{
		const bool bPrepare{ ChargedComboAttackMontageIndex == 0 };

		// 준비 몽타주 차례가 아닐 때
		if (!bPrepare)
		{
			bIsChargedAttack = true;

			CombatState = ECombatState::ECS_Attack;
			UseStaminaAndStopRecovery(EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Charged));
		}

		AnimInstance->Montage_Play(ChargedComboMontages[ChargedComboAttackMontageIndex], 1.f);

		WeaponAttackType = EWeaponAttackType::EWAT_Charged;
		ChargedComboAttackMontageIndex++;

		return true;
	}
	return false;
}

void APlayerCharacter::ResetAttack()
{
	GetWorldTimerManager().ClearTimer(ComboTimer);

	// 콤보를 초기화하고, 캐릭터 상태도 바꿔준다.
	ComboAttackMontageIndex = 0;
	ChargedComboAttackMontageIndex = 0;
	bIsChargedAttack = false;

	// AttackToIdle 상태에선 구르기, 막기, 강 공격 등을 사용하여 딜레이 스킵 가능 (막기는 나중에 막힐 수도 있음)
	CombatState = ECombatState::ECS_AttackToIdle;
	PlayerAttackType = EPlayerAttackType::EPAT_Weapon;
}

void APlayerCharacter::StartAttackCheckTime()
{
	switch (PlayerAttackType)
	{
	case EPlayerAttackType::EPAT_Weapon:
		EquippedWeapon->InitAttackData(WeaponAttackType, bVisibleTraceSphere);

		GetWorldTimerManager().SetTimer(
			AttackCheckTimer,
			EquippedWeapon->GetAttackDelegate(),
			AttackCheckTimeDelay,
			true);
		break;
	case EPlayerAttackType::EPAT_Shield:
		EquippedShield->InitPushShiledData(bVisibleTraceSphere);

		GetWorldTimerManager().SetTimer(
			AttackCheckTimer,
			EquippedShield->GetPushShieldDelegate(),
			AttackCheckTimeDelay,
			true);
		break;
	}
}

void APlayerCharacter::EndAttackCheckTime()
{
	GetWorldTimerManager().ClearTimer(AttackCheckTimer);
}

void APlayerCharacter::PressedLockOn()
{
	if (!bControl)return;

	if (bLockOn) 
	{
		// 락온 취소
		ResetLockOn();
	}
	else if(CheckActionableState())
	{
		// 락온 선택 시도
		TArray<AActor*> OutActors;

		UKismetSystemLibrary::SphereOverlapActors(
			this,
			GetActorLocation(),
			1500.f,
			TraceObjectTypes,
			AEnemy::StaticClass(),
			{ this },
			OutActors);

		if (OutActors.Num() > 0) 
		{
			// 가장 가까운 락온 대상을 가져온다.
			AEnemy* LockOnTarget{ GetNearestEnemyWithLockOn(OutActors) };

			// 락온된 타겟이 있으면 설정
			if (LockOnTarget)
			{
				LockOnTarget->SetLockOn(true);
				EnemyLockOnResetDelegate.BindUFunction(LockOnTarget, FName("ResetLockOn"));

				LockOnWidgetData = LockOnTarget->GetLockOnWidget();
				MinimumLockOnPitchValue = LockOnTarget->GetMinimumLockOnPitchValue();

				bLockOn = true;
				LockOnWidgetData->SetVisibility(true);
			}
		}
	}
}

AEnemy* APlayerCharacter::GetNearestEnemyWithLockOn(const TArray<AActor*> Actors)
{
	AEnemy* OutputEnemy{ nullptr };
	float LastDistance{ 0.f };

	for (AActor* Actor : Actors)
	{
		AEnemy* NowEnemy{ Cast<AEnemy>(Actor) };
		if (NowEnemy->GetDying())
		{
			continue;
		}

		// 카메라 정면과 50도 이상 떨어져있는지 검사한다.
		const FVector PlayerToEnemyDirection{ UKismetMathLibrary::Normal(Actor->GetActorLocation() - GetActorLocation()) };

		const FRotator ControlRot{ 0.f,GetControlRotation().Yaw,0.f };
		const FVector ControlForward{ UKismetMathLibrary::Normal(ControlRot.Vector()) };

		const float DotProductValue{ UKismetMathLibrary::Dot_VectorVector( PlayerToEnemyDirection, ControlForward) };

		// 라디안 값을 디그리 값으로 바꿔준다.
		const float ValueToDegree{ UKismetMathLibrary::DegAcos(DotProductValue) };

		// 카메라 정면으로 부터 좌, 우 50도내에 있는 경우
		if (ValueToDegree <= 50.f)
		{
			const float NowDistance{ UKismetMathLibrary::Vector_Distance( Actor->GetActorLocation(), GetActorLocation()) };

			// 이미 선택된 적이 있으면
			if (OutputEnemy) 
			{
				// 이전 정보, 현재 정보 중 더 가까운 적을 타겟팅한다.
				if (NowDistance < LastDistance)
				{
					OutputEnemy = NowEnemy;
					LastDistance = NowDistance;
				}
			}
			// 선택된 적이 아직 없으면
			else 
			{
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
	const FRotator FixLookRotator{ LookRotator.Pitch, LookRotator.Yaw, GetActorRotation().Roll };

	// RLerp를 사용해 카메라를 부드럽게 전환시킨다.
	GetController()->SetControlRotation( UKismetMathLibrary::RLerp( GetControlRotation(), FixLookRotator, LockOnCameraSpeed, true));
}

void APlayerCharacter::PressedUseItem()
{
	bPressedUseItemButton = true;

	if (!bControl)return;

	if (EquippedPotion) 
	{
		// 특정 액션(공격, 구르기, 점프 상태)을 취하고 있지 않을 때만 사용 가능
		if (!CheckActionableState())return;
		if (!CheckLand())return;

		ForceStopAllMontage();

		// 풀피가 아닐 때 사용 (포션)
		if (!UKismetMathLibrary::EqualEqual_FloatFloat(HP, MaximumHP))
		{
			EndToIdleState(true);
			UseItem();
		}
	}
}

void APlayerCharacter::ReleasedUseItem()
{
	bPressedUseItemButton = false;
}

void APlayerCharacter::UseItem()
{
	bDrinkingPotion = true;
	CombatState = ECombatState::ECS_MovableInteraction;
}

void APlayerCharacter::SkipUseItem()
{
	bDrinkingPotion = false;
}

void APlayerCharacter::DrinkPotion()
{
	const float NextHP{ HP + EquippedPotion->GetRecoveryAmount() };

	// 체력을 최대 체력을 넘어서지 못하게 한다.
	HP = (NextHP < MaximumHP) ? NextHP : MaximumHP;
}

void APlayerCharacter::EndUseItem()
{
	bDrinkingPotion = false;
	CombatState = ECombatState::ECS_Unoccupied;
}

bool APlayerCharacter::CheckLand()
{
	return !GetCharacterMovement()->IsFalling();
}

void APlayerCharacter::JumpLandAttack()
{
	if (CombatState == ECombatState::ECS_Attack)return;

	if (JumpAttackMontage)
	{
		UseStaminaAndStopRecovery(EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Dash));

		CombatState = ECombatState::ECS_Attack;
		AnimInstance->Montage_Play(JumpAttackMontage);
	}
}

void APlayerCharacter::SaveMaxmimumVelocity()
{
	// 하강 최대 가속도를 구한다.
	// 나중에는 착지시 까지 시간으로 계산할 수도 있음
	MaximumZVelocity = UKismetMathLibrary::FMax( MaximumZVelocity, UKismetMathLibrary::Abs(GetVelocity().Z));
}

void APlayerCharacter::DashAttack()
{
	if (Stamina >= EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Dash))
	{
		if (DashAttackMontage)
		{
			EndToIdleState(false);
			UseStaminaAndStopRecovery(EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Dash));

			bShouldContinueAttack = false;
			AnimInstance->Montage_Play(DashAttackMontage);
			CombatState = ECombatState::ECS_Attack;
			WeaponAttackType = EWeaponAttackType::EWAT_Dash;
		}
	}
}

void APlayerCharacter::ApplyLockOnMovementSetting()
{
	if (bLockOn)
	{
		// Other -> Sprint animation
		if (GetSprinting())
		{
			// 잠시 lockOn Movement에 필요한 세팅을 끈다.
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		// Sprint -> Other animation
		else
		{
			// 다시 lockOn Movement에 필요한 세팅을 킨다.
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void APlayerCharacter::ApplyLockOnAttackSetting(bool bStartAttack)
{
	if (!bLockOn)
	{
		return;
	}

	if (bStartAttack)
	{
		// 잠시 lockOn Movement에 필요한 세팅을 끈다.
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		// 다시 lockOn Movement에 필요한 세팅을 킨다.
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void APlayerCharacter::InitPlayerData(FPlayerData InputPlayerData)
{
	PlayerData = InputPlayerData;
}

void APlayerCharacter::EndRestMode()
{
	bRest = false;
}

void APlayerCharacter::SetCheckPoint(ASavePoint* Point)
{
	if (Point)
	{
		CheckPoint = Point;
		PlayerData.SavePointTransform = CheckPoint->GetResponPointTransform();
		PlayerData.SavePointName = CheckPoint->GetSavePointName();
	}
}

float APlayerCharacter::GetStPercentage()
{
	return Stamina / MaximumStamina;
}

void APlayerCharacter::IncreaseGold(float Value)
{
	PlayerData.Gold += Value;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDying)return;

	if (GetSprinting() && GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		HardResetSprint();
	}

	if (bIsBeforeAttackRotate)
	{
		BeginAttackRotate(DeltaTime);
	}

	if (bLockOn)
	{
		RotateCameraByLockOn();
	}

	if (bRecoverStamina)
	{
		RecoverStamina();
	}

	// Audio Listener Update
	if (PlayerController)
	{
		UpdateListenerRotation();
	}

	UpdateIKFootData(DeltaTime);
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

	PlayerInputComponent->BindAction("RollAndSprint", IE_Pressed, this, &APlayerCharacter::PressedRollAndSprint);
	PlayerInputComponent->BindAction("RollAndSprint", IE_Released, this, &APlayerCharacter::ReleasedRollAndSprint);

	PlayerInputComponent->BindAction("SubAttackButton", IE_Pressed, this, &APlayerCharacter::PressedSubAttack);
	PlayerInputComponent->BindAction("SubAttackButton", IE_Released, this, &APlayerCharacter::ReleasedSubAttack);

	PlayerInputComponent->BindAction("ChargedAttackButton", IE_Pressed, this, &APlayerCharacter::PressedChargedAttack);
	PlayerInputComponent->BindAction("ChargedAttackButton", IE_Released, this, &APlayerCharacter::ReleasedChargedAttack);

	PlayerInputComponent->BindAction("LockOnButton", IE_Pressed, this, &APlayerCharacter::PressedLockOn);

	// UseItemButton
	PlayerInputComponent->BindAction("UseItemButton", IE_Pressed, this, &APlayerCharacter::PressedUseItem);
	PlayerInputComponent->BindAction("UseItemButton", IE_Released, this, &APlayerCharacter::ReleasedUseItem);

	// Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::PressedJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::ReleasedJump);

	// EventMotionButton
	PlayerInputComponent->BindAction("EventMotionButton", IE_Pressed, this, &APlayerCharacter::PressedEventMotion);
	//PlayerInputComponent->BindAction("EventMotionButton", IE_Released, this, &APlayerCharacter::ReleasedJump);
}

bool APlayerCharacter::CustomTakeDamage(float DamageAmount, AActor* DamageCauser, EAttackType AttackType)
{
	if (bDying || CombatState == ECombatState::ECS_Roll)
	{
		return false;
	}

	const FVector PlayerForward{ GetActorForwardVector() };
	const FVector PlayerToHitPoint{ UKismetMathLibrary::Normal(DamageCauser->GetActorLocation() - GetActorLocation()) };
	const float DotProductResult{ UKismetMathLibrary::DotProduct2D(FVector2D(PlayerForward),FVector2D(PlayerToHitPoint)) };
	const float DegreeDifference{ UKismetMathLibrary::DegAcos(DotProductResult) };
	//UE_LOG(LogTemp, Warning, TEXT("hit angle : %f"), DegreeDifference);

	switch (CombatState)
	{
	case ECombatState::ECS_Guard:
		// 각도가 -DeffenceAngle,+DeffenceAngle 사이인지 구한다.
		if (DegreeDifference <= EquippedShield->GetDefenceDegree()) {
			const float HalfDamage{ DamageAmount / 2 };
			if (Stamina >= HalfDamage) {
				// 스태미나를 일정 수치 깎는다.
				Stamina -= HalfDamage;

				SetShiledImpact(true);
				AnimInstance->SetShouldPlayShieldImpact(true);

				// 데미지를 가드가 모두 받아냈으니 함수를 종료한다.
				return true;
			}
			else {
				Stamina = 0.f;
				// 가드로 막을 수 없는 공격이면 가드를 취소하고 특정 애니메이션을 실행한다.
				bGuardBreak = true;

				EndSubAttack();
			}
		}
		break;
	case ECombatState::ECS_Attack:
		// 약 공격이라면 무시하고 공격을 진행한다.
		if (AttackType != EAttackType::EAT_Light)
		{
			ResetAttack();
		}
		break;
	case ECombatState::ECS_MovableInteraction:
		// 사용중인 아이템이 있다면 취소한다.
		SkipUseItem();
		break;
	}

	// 공격중이 아니면 -> 피해 애니메이션 실행
	// 공격중인데 약 공격이 아니면 -> 피해 애니메이션 실행
	if (CombatState != ECombatState::ECS_Attack || AttackType != EAttackType::EAT_Light)
	{
		// 피해 애니메이션을 실행하기 위해 모든 몽타주 종료
		AnimInstance->StopAllMontages(0.15f);
		//CombatState = ECombatState::ECS_Impact;
	}

	// 데미지 적용
	const bool bDamaged{ Super::CustomTakeDamage(DamageAmount, DamageCauser, AttackType) };
	
	if (bDamaged)
	{
		CombatState = ECombatState::ECS_Impact;
		// 잠시 무적상태로 만들고 Hit이 종료되면 그 때 무적 상태를 푼다.
		ChangeDamageState(EDamageState::EDS_invincibility);

		if (AttackType == EAttackType::EAT_Strong)
		{
			FVector ToTarget{ DamageCauser->GetActorLocation() - GetActorLocation() };
			ToTarget.Z = 0.f;
			ToTarget = UKismetMathLibrary::Normal(ToTarget);

			float LookAtDegree{ UKismetMathLibrary::DegAtan2(ToTarget.Y,ToTarget.X) };

			// 화면이 잠깐 튀는 이슈가 있음
			SetActorRotation(FQuat(FVector::UpVector, FMath::DegreesToRadians(LookAtDegree)));


			//FVector LaunchVelocity{ DamageCauser->GetActorForwardVector() };
			//LaunchVelocity.Z = 0.f;
			//LaunchVelocity = -LaunchVelocity;
			//LaunchVelocity *= 1000.f;
			//LaunchVelocity.Z = 200.f;
			//LaunchCharacter(LaunchVelocity, false, false);
		}

 		if (bDying)
		{
			// Die Montage 호출
			if (DieMontage)
			{
				AnimInstance->Montage_Play(DieMontage);
			}

			UnPossessed();
		}
	}

	return true;
}

bool APlayerCharacter::FallingDamage(float LastMaxmimumZVelocity)
{
	if (bDying)
	{
		return false;
	}

	const bool bDamaged{ Super::FallingDamage(LastMaxmimumZVelocity) };

	if (bDamaged && bDying)
	{
		// Die Montage 호출
		if (DieMontage)
		{
			AnimInstance->Montage_Play(DieMontage);
		}

		UnPossessed();
	}

	return bDamaged;
}

void APlayerCharacter::AddFunctionToDamageTypeResetDelegate(AEnemy* Enemy, const FName& FunctionName)
{
	EnemyDamageTypeResetDelegate.AddUFunction(Enemy, FunctionName);
}

void APlayerCharacter::ResetLockOn()
{
	// 락온 취소
	bLockOn = false;
	if (LockOnWidgetData)
	{
		LockOnWidgetData->SetVisibility(false);
		LockOnWidgetData = nullptr;
	}

	EnemyLockOnResetDelegate.ExecuteIfBound();
	EnemyLockOnResetDelegate.Unbind();

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

float APlayerCharacter::GetMoveAngle()
{
	const FVector2D Axis{ GetThumStickAxisForce() };
	return UKismetMathLibrary::DegAtan2(Axis.Y, Axis.X);
}

void APlayerCharacter::SetEventAble(bool bNext, AEventArea* EArea)
{
	bEventAble = bNext;
	EventArea = EArea;
	if (EventArea)
	{
		EventText = EventArea->GetEventText();
	}
	
	SetButtonEventUIVisibility(bEventAble);
}

void APlayerCharacter::WaitControlFadeTime(float FadeTime)
{
	GetWorldTimerManager().SetTimer(
		ControlDelayTimer,
		this,
		&APlayerCharacter::TurnOnControl,
		FadeTime);
}

void APlayerCharacter::HardResetSprint()
{
	Super::HardResetSprint();

	// 스태미나 슬로우를 풀어준다.
	StaminaRecoveryAmount = DefaultStaminaRecoveryAmount;
}

void APlayerCharacter::EndShieldImpact()
{
	Super::EndShieldImpact();

	// SubAttack을 누르지 않았을 때 종료한다.
	if (!bPressedSubAttackButton) 
	{
		EndSubAttack();
		CombatState = ECombatState::ECS_ToIdle;
	}
}

void APlayerCharacter::EndDamageImpact()
{
	CombatState = ECombatState::ECS_ToIdle;
	ChangeDamageState(EDamageState::EDS_Unoccupied);
}

void APlayerCharacter::TurnOnControl()
{
	bControl = true;
}

const FVector2D APlayerCharacter::GetMovementLocalAxis()
{
	return { GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight") };
}

const float APlayerCharacter::GetMovementDegree()
{
	const FVector2D MoveLocalAxis{ GetMovementLocalAxis() };
	return UKismetMathLibrary::DegAtan2(MoveLocalAxis.Y, MoveLocalAxis.X);
}

const FVector2D APlayerCharacter::GetAxisOfMovementRelativeController()
{
	const FRotator MoveRotation{ 0,GetMovementDegree(),0 };
	const FRotator ControllRotation{ 0,GetControlRotation().Yaw,0 };
	const FRotator ComposeRotaton{ UKismetMathLibrary::ComposeRotators(ControllRotation, MoveRotation) };
	const FVector2D ComposeDirection{ UKismetMathLibrary::Normal(ComposeRotaton.Vector()) };

	return ComposeDirection;
}

void APlayerCharacter::Sprint()
{
	bCanRoll = true;

	if (CheckActionableState())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	else
	{
		return;
	}
	if (GetCharacterMovement()->IsFalling())return;

	if (GetMovementComponent()->Velocity.Size() > 0.f) 
	{
		// 스태미나 속도 늦추기 (차기는 참)
		StaminaRecoveryAmount = SprintStaminaRecoveryAmount;

		ChangeMoveState(true);
	}
}

void APlayerCharacter::StopDelayForRoll()
{
	CombatState = ECombatState::ECS_ToIdle;
	bCanRoll = true;
}

bool APlayerCharacter::CheckMontageState()
{
	if (AnimInstance)
	{
		return AnimInstance->IsAnyMontagePlaying();
	}
	return false;
}

void APlayerCharacter::StopAllMontage()
{
	if (AnimInstance) 
	{
		AnimInstance->StopAllMontages(0.2f);
	}
}

void APlayerCharacter::ForceStopAllMontage()
{
	if (AnimInstance && AnimInstance->IsAnyMontagePlaying()) 
	{
		AnimInstance->StopAllMontages(0.2f);
	}
}

void APlayerCharacter::PressedJump()
{
	bPressedJumpButton = true;

	// 조건 검사
	if (!bControl)return;
	if (Stamina < JumpRequiredStamina)return;
	if (!CheckActionableState())return;
	if (!CheckLand())return;
	EndToIdleState(true);
	MaximumZVelocity = 0.f;

	UseStaminaAndStopRecovery(JumpRequiredStamina);

	ACharacter::Jump();
}

void APlayerCharacter::ReleasedJump()
{
	bPressedJumpButton = false;

	StopJumping();
}

void APlayerCharacter::PrepareJumpAttack()
{
	// 점프 하강 공격은 대쉬 공격과 같은 스태미나를 사용한다.
	if (Stamina >= EquippedWeapon->GetRequiredStamina(EWeaponAttackType::EWAT_Dash))
	{
		if (PrepareJumpAttackMontage) {
			EndToIdleState(false);

			CombatState = ECombatState::ECS_NonMovingInteraction;
			AnimInstance->Montage_Play(PrepareJumpAttackMontage);
		}
	}
}

void APlayerCharacter::UpdateIKFootData(float DeltaTime)
{
	// IK_Foot을 잠시 끄는 조건
	if (/*bLockOn ||*/
		CombatState == ECombatState::ECS_Attack ||
		CombatState == ECombatState::ECS_Impact ||
		CombatState == ECombatState::ECS_Roll) {

		IKFootAlpha = 0.f;
		return;
	}

	ContinueUpdateIKData(DeltaTime);
}

void APlayerCharacter::IKFootTrace(const FName& SocketName, FHitResult& HitResult)
{
	const FVector SocketLoc{ GetMesh()->GetBoneLocation(SocketName) };
	const FVector ActorLoc{ GetActorLocation() };

	const FVector TraceStartPoint{ SocketLoc.X,SocketLoc.Y,ActorLoc.Z };
	const FVector TraceEndPoint{ SocketLoc.X,SocketLoc.Y,SocketLoc.Z - IKTraceDistance };

	const bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		TraceStartPoint,
		TraceEndPoint,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ this },
		EDrawDebugTrace::None,
		HitResult,
		true);
}

void APlayerCharacter::ContinueUpdateIKData(float DeltaTime)
{
	IKFootAlpha = UKismetMathLibrary::FInterpTo(IKFootAlpha, 1.f, DeltaTime, IKInterpSpeed);

	FVector HitLocation[2]{ FVector::ZeroVector, FVector::ZeroVector };

	// for-loop variable
	FHitResult HitResult;
	FVector Location;
	float Offset;
	FVector Normal;

	// other variable
	float TargetHipOffset{ 0.f };


	// { left, right }
	// offset과 rotator를 업데이트 시킨다.
	for (int32 i = 0; i < 2; i++) 
	{
		const bool bLeft{ i == 0 ? true : false };

		const FName SelectSocketName{ bLeft ? LeftFootSocketName : RightFootSocketName };
		float& SelectOffset{ bLeft ? IKLeftFootOffset : IKRightFootOffset };
		FRotator& SelectRotator{ bLeft ? IKLeftFootRotator : IKRightFootRotator };

		// reset last data
		HitResult.Reset();
		Location = FVector::ZeroVector;
		Offset = 0.f;
		Normal = FVector::ZeroVector;

		IKFootTrace(SelectSocketName, HitResult);

		Location = HitResult.Location;
		// 나중에 hipOffset의 위치를 구할 때 사용하므로 따로 저장한다.
		HitLocation[i] = Location;

		const FVector Temp = (HitResult.GetActor() ? HitResult.Location - GetMesh()->GetComponentLocation() : FVector::ZeroVector);
		Offset = Temp.Z - IKHipOffset;
		Normal = HitResult.Normal;

		// FootOfftset을 interp를 사용해 구한다.
		SelectOffset = UKismetMathLibrary::FInterpTo(
			SelectOffset,
			Offset,
			DeltaTime,
			IKInterpSpeed);

		const float Degree1{ UKismetMathLibrary::DegAtan2(Normal.Y,Normal.Z) };
		const float Degree2{ -(UKismetMathLibrary::DegAtan2(Normal.X,Normal.Z)) };
		FRotator FootRot;
		FootRot.Roll = Degree1;
		FootRot.Pitch = Degree2;

		SelectRotator = UKismetMathLibrary::RInterpTo(
			SelectRotator,
			FootRot,
			DeltaTime,
			IKInterpSpeed);
	}

	// 위에서 구한 location(left, right foot)으로 HipOffset을 구한다.
	FVector Temp1{ HitLocation[0] - HitLocation[1] };
	// Why use Z? 두 발의 높이차를 구하기위해 Z축을 사용한다.
	float HightOfTwoFeet{ UKismetMathLibrary::Abs(Temp1.Z) };

	// 높이 차이가 50이하로 차이나는지 검사한다.
	if (HightOfTwoFeet < 50.f) {
		TargetHipOffset = HightOfTwoFeet * (-0.5f);
	}

	IKHipOffset = UKismetMathLibrary::FInterpTo(
		IKHipOffset,
		TargetHipOffset,
		DeltaTime,
		IKInterpSpeed);
}

FVector APlayerCharacter::GetFootLocation(bool bLeft)
{
	const FName SelectSocketName{ bLeft ? LeftFootSocketName : RightFootSocketName };
	FHitResult HitResult;
	IKFootTrace(SelectSocketName, HitResult);

	if (HitResult.bBlockingHit)
	{
		return HitResult.ImpactPoint;
	}
	else
	{
		FVector BoneLoc{ GetMesh()->GetBoneLocation(SelectSocketName) };
		return BoneLoc + FVector(0.f, 0.f, -13.5f);
	}
}

void APlayerCharacter::StopAttackToIdle()
{
	if (bLockOn)
	{
		ApplyLockOnAttackSetting(false);
	}
	ForceStopAllMontage();
	CombatState = ECombatState::ECS_Unoccupied;
}

bool APlayerCharacter::CheckActionableState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
	case ECombatState::ECS_ToIdle:
	case ECombatState::ECS_AttackToIdle:
		return true;
	}
	return false;
}

void APlayerCharacter::EndToIdleState(bool bForceStopMontage)
{
	if (CombatState == ECombatState::ECS_AttackToIdle ||
		CombatState == ECombatState::ECS_ToIdle)
	{
		if (bForceStopMontage && CombatState == ECombatState::ECS_AttackToIdle)
		{
			StopAttackToIdle();
		}
		// ForceStopMontage가 아니거나 ToIdle 일 경우
		else
		{
			CombatState = ECombatState::ECS_Unoccupied;
		}
	}
}

void APlayerCharacter::PressedEventMotion()
{
	if (!bControl)return;

	if (bRest)
	{
		return;
	}

	// 조건 확인
	if (bEventAble && EventArea && CheckActionableState() && CheckLand())
	{
		ASavePoint* SaveP = Cast<ASavePoint>(EventArea);
		AExecutionArea* ExeA = Cast<AExecutionArea>(EventArea);

		if (SaveP)
		{
			ResetLockOn();

			bRest = true;
			CombatState = ECombatState::ECS_RestInteraction;
			SetButtonEventUIVisibility(false);
			HP = MaximumHP;

			SaveP->ActiveAutoArrange(150.f);
		}

		if (ExeA)
		{
			const bool bEnemyHaveExecutionMontage{ ExeA->GetEnemy() && ExeA->GetEnemy()->ValidTakeExecutionMontage() };
			const bool bExecutionable{ ExecutionMontage && bEnemyHaveExecutionMontage };

			if (bExecutionable)
			{
				ResetLockOn();

				CombatState = ECombatState::ECS_Attack;
				SetButtonEventUIVisibility(false);

				ExecutionArea = ExeA;
				ExecutionArea->GetEnemy()->ClearStunTimer();
				ExecutionArea->GetEnemy()->ChangePawnCollision(false);


				float FDamage, SDamage;
				EquippedWeapon->GetExecutionDamage(FDamage, SDamage);
				ExeA->InitExecutionData(this, FDamage, SDamage);
				ExeA->ActiveAutoArrange();

				AnimInstance->Montage_Play(ExecutionMontage);
				ExecutionArea->ActiveExecution();
			}
		}
	}
}

void APlayerCharacter::UpdateListenerRotation()
{
	const FRotator ListenerRot{ UKismetMathLibrary::ComposeRotators(GetActorRotation(), GetControlRotation()) };
	PlayerController->SetAudioListenerOverride(ListenerComponent, { 0.f,0.f,0.f }, ListenerRot);
}

float APlayerCharacter::GetPlayerGold()
{
	return PlayerData.Gold;
}

void APlayerCharacter::TargetDeath(float TargetRewardGold)
{
	if (TargetRewardGold == 0.f)
	{
		return;
	}

	PlayerData.Gold += TargetRewardGold;
	PlayPlusGoldAnimation(TargetRewardGold);
}
