// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/RPGAnimInstance.h"
#include "Characters/RPGCharacter.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URPGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ARPGCharacter>(TryGetPawnOwner());
	Character = Cast<ARPGCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void URPGAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (CharacterMovement)
	{
		const FRotator  Rotation = Character->GetActorRotation();
		const FVector Velocity = Character->GetCharacterMovement()->Velocity;
		CurrentDirection = CalculateDirection(Velocity, Rotation);
		CurrentSpeed = Character->GetCurrentSpeed();
		//if (GEngine)
		//{
		//	FString DebugMessage = FString::Printf(TEXT("Current Speed: %f"), CurrentSpeed);
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, DebugMessage);
		//}
	}
}

//Calcualte direction float based on velocity and rotation - this is a blueprint built-in function
float URPGAnimInstance::CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation)
{
	if (!Velocity.IsNearlyZero())
	{
		FMatrix RotMatrix = FRotationMatrix(BaseRotation);
		FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		FVector NormalizedVel = Velocity.GetSafeNormal2D();

		// get a cos(alpha) of forward vector vs velocity
		float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
		// now get the alpha and convert to degree
		float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

		// depending on where right vector is, flip it
		float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);
		if (RightCosAngle < 0)
		{
			ForwardDeltaDegree *= -1;
		}

		return ForwardDeltaDegree;
	}

	return 0.f;
}
