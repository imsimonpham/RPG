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

	UPROPERTY (VisibleAnyWhere, BlueprintReadOnly, Category = "Movement")
	float CurrentSpeed;

protected:
	virtual void BeginPlay() override;

	//movement and look
	void MoveForward(float Value);
	void MoveRight(float Value);
	void UpdateCharacterMovement();
	void Turn(float Value);
	void LookUp(float Value);
	void Sprint();

	UPROPERTY(VisibleAnywhere)
	float SpeedMultiplier = 0.5f;

	UPROPERTY(VisibleAnywhere)
	float MoveForwardValue;

	UPROPERTY(VisibleAnywhere)
	float MoveRightValue;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	bool IsSprinting = false;

	UPROPERTY(VisibleAnywhere)
	bool IsWalking = false;

	UPROPERTY(VisibleAnywhere)
	EMovementState MovementState = EMovementState::EMS_Idle;

	void AdjustSpeedMultiplier();
	void CalculateMovementSpeed();

	float WalkSpeed = 300.f;
	float SprintSpeed = 600.f;
	
public:
	FORCEINLINE EMovementState GetMovementState() const { return MovementState; }
	FORCEINLINE float GetCurrentSpeed() const { return CurrentSpeed; }
};
