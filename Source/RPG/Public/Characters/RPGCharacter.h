#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterStates.h"
#include "RPGCharacter.generated.h"

UCLASS()
class RPG_API ARPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	//movement input handlers
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Sprint();
	void Dodge();

	void AdjustSpeedMultiplier();
	void CalculateMovementSpeed();

	//combat
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	//montages
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	class UAnimMontage* DodgeMontage;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;

	//camera
	void AdjustCameraDistance();
	float MaxTargetArmLength = 200.f;
	float MinTargetArmLength = 150.f;
	float InterpSpeed = 2.f;
	UPROPERTY(VisibleAnywhere)
	bool CanAdjustCameraDistance = false;

	//character movement
	float SprintingSpeedMultiplier = 0.5f;
	FVector Direction;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurrentSpeed;

	UPROPERTY(VisibleAnywhere)
	bool IsSprinting = false;

	UPROPERTY(VisibleAnywhere)
	float MoveForwardValue;

	UPROPERTY(VisibleAnywhere)
	float MoveRightValue;

	float LookUpValue;
	float TurnValue;
	float LookSensitityModifier = 0.3f;
	float TurnSensitivityModifier = 0.3f;

	//character state
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Neutral;
	
public:
	FORCEINLINE float GetCurrentSpeed() const { return CurrentSpeed; }
};
