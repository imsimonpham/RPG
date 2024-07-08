#pragma once

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Neutral UMETA(Displayname = "Neutral"),
	EAS_Dodging UMETA(DisplayName = "Dodging"),
	EAS_Attacking UMETA(DisplayName = "Attacking")
};