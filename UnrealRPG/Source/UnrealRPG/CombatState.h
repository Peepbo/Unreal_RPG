#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Roll UMETA(DisplayName = "Roll"),
	ECS_Guard UMETA(DisplayName = "Guard"),
	ECS_Interaction UMETA(DisplayName = "Interaction"),

	ECS_MAX UMETA(DisplayName = "DefaultMax")
};