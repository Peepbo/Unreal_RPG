#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	// Common State
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Guard UMETA(DisplayName = "Guard"),

	// Player State
	ECS_Roll UMETA(DisplayName = "Roll"),
	ECS_Interaction UMETA(DisplayName = "Interaction"),

	// Enemy State
	ECS_FaceOff UMETA(DisplayName = "FaceOff"),
	ECS_Chase UMETA(DisplayName = "Chase"),
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};