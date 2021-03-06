#pragma once

DECLARE_MULTICAST_DELEGATE(FEnemyDamageTypeResetDelegate);

DECLARE_DELEGATE(FEnemyLockOnResetDelegate);

UENUM(BlueprintType)
enum class EPlayerAttackType : uint8
{
	EPAT_Weapon UMETA(DisplayName = "Weapon"),
	EPAT_Shield UMETA(DisplayName = "Charged"),

	EPAT_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponAttackType : uint8
{
	EWAT_Normal UMETA(DisplayName = "Normal"),
	EWAT_Charged UMETA(DisplayName = "Charged"),
	EWAT_Dash UMETA(DisplayName = "Dash"),

	EWAT_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	// Common State
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_ToIdle UMETA(DisplayName = "ToIdle"),
	ECS_AttackToIdle UMETA(DisplayName = "AttackToIdle"),
	ECS_Guard UMETA(DisplayName = "Guard"),

	// Player State
	ECS_Roll UMETA(DisplayName = "Roll"),
	ECS_Impact UMETA(DisplayName = "Impact"),
	ECS_MovableInteraction UMETA(DisplayName = "Movable Interaction"),
	ECS_NonMovingInteraction UMETA(DisplayName = "Non-Moving Interaction"),
	ECS_RestInteraction UMETA(DisplayName = "Rest Interaction"),

	// Enemy State
	ECS_FaceOff UMETA(DisplayName = "FaceOff"),
	ECS_Chase UMETA(DisplayName = "Chase"),
	ECS_Stun UMETA(DisplayName = "Stun"),
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemySize : uint8
{
	EES_Small UMETA(DisplayName = "Small"),
	EES_Medium UMETA(DisplayName = "Medium"),
	EES_Large UMETA(DisplayName = "Large"),

	EES_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EDamageState : uint8
{
	EDS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EDS_invincibility UMETA(DisplayName = "invincibility"),

	EDS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_OneHandWeapon UMETA(DisplayName = "OneHandWeapon"),
	EWT_TwoHandWeapon UMETA(DisplayName = "TwoHandWeapon"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	EAT_Light UMETA(DisplayName = "Light Attack"),
	EAT_Medium UMETA(DisplayName = "Medium Attack"),
	EAT_Strong UMETA(DisplayName = "Strong Attack"),
	EAT_MAX UMETA(DisplayName = "DefaultMax")
};

#define EnemyAttackLinkedListNode TDoubleLinkedList<FEnemyAttackData>::TDoubleLinkedListNode

#define EnemyNormalAttackLinkedListNode TDoubleLinkedList<FEnemyNormalAttack>::TDoubleLinkedListNode
#define EnemySpecialAttackLinkedListNode TDoubleLinkedList<FEnemySpecialAttack>::TDoubleLinkedListNode
#define EnemyMagicAttackLinkedListNode TDoubleLinkedList<FEnemyMagicAttack>::TDoubleLinkedListNode


UENUM(BlueprintType)
enum class EEnemyMontageType : uint8
{
	EEMT_Normal UMETA(DisplayName = "Normal"),
	EEMT_Special UMETA(DisplayName = "Special"),
	EEMT_Back UMETA(DisplayName = "Back"),
	EEMT_Magic UMETA(DisplayName = "Magic"),
	EEMT_MAX UMETA(DisplayName = "DefualtMax")
};