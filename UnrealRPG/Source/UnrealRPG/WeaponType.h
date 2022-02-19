#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_OneHandWeapon UMETA(DisplayName = "OneHandWeapon"),
	EWT_TwoHandWeapon UMETA(DisplayName = "TwoHandWeapon"),

	EWT_MAX UMETA(DisplayName = "DefaultMax")
};