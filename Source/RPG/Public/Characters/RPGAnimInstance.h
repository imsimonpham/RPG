// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStates.h"
#include "RPGAnimInstance.generated.h"

UCLASS()
class RPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public: 
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite) //BlueprintReadWrite/BlueprintReadOnly will expose this to event graph
	class ARPGCharacter* Character;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CurrentSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CurrentDirection;

	static float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation);
};
