#pragma once

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Walk UMETA(DisplayName = "Walk"),
	EMS_Sprint UMETA(DisplayName = "Sprint")
};