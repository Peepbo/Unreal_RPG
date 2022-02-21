#pragma once

UENUM(BlueprintType)
enum class EDamageState : uint8
{
	EDS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EDS_invincibility UMETA(DisplayName = "invincibility"),

	EDS_MAX UMETA(DisplayName = "DefaultMax")
};