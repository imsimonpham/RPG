#include "Characters/RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARPGCharacter::ARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//CameraBoom and Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Match character rotation with moving direction
	/*GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);*/

}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateMovementSpeed();
}

#pragma region PlayerInput
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ARPGCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ARPGCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ARPGCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGCharacter::Sprint);
}

void ARPGCharacter::Turn(float Value)
{
	TurnValue = Value;
	AddControllerYawInput(TurnValue * TurnSensitivityModifier);
}
void ARPGCharacter::LookUp(float Value)
{
	LookUpValue = Value;
	AddControllerPitchInput(LookUpValue * LookSensitityModifier);
}

void ARPGCharacter::MoveForward(float Value)
{
	if (MoveRightValue == 0)
	{
		MoveForwardValue = Value;
	}
	else
	{
		MoveForwardValue = Value / FMath::Sqrt(2.0f);
	}

	/*if (MoveForwardValue < 0)
	{
		BackwardSpeedMultiplier = 0.85f;
	}
	else
	{
		BackwardSpeedMultiplier = 1.f;
	}*/

	if (Controller && (MoveForwardValue != 0.f))
	{
		//FRotator3d(double InPitch, double InYaw, double InRoll)
		const FRotator  ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //X is forward vector
		AddMovementInput(Direction.GetSafeNormal(), MoveForwardValue * SprintingSpeedMultiplier);
	}
}

void ARPGCharacter::MoveRight(float Value)
{
	if (MoveForwardValue == 0)
	{
		MoveRightValue = Value;
	}
	else
	{
		MoveRightValue = Value / FMath::Sqrt(2.0f);
	}

	/*if (MoveForwardValue < 0)
	{
		BackwardSpeedMultiplier = 0.85f;
	}
	else
	{
		BackwardSpeedMultiplier = 1.f;
	}*/

	if (Controller && (MoveRightValue != 0.f))
	{
		//FRotator3d(double InPitch, double InYaw, double InRoll)
		const FRotator  ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //Y is right vector
		AddMovementInput(Direction.GetSafeNormal(), MoveRightValue * SprintingSpeedMultiplier);
	}
}

void ARPGCharacter::Sprint()
{
	IsSprinting = !IsSprinting;
}

void ARPGCharacter::AdjustSpeedMultiplier()
{
	if (IsSprinting)
	{
		SprintingSpeedMultiplier = 1.f;
	}
	else {
		SprintingSpeedMultiplier = 0.5f;
	}
}

void ARPGCharacter::CalculateMovementSpeed()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	CurrentSpeed = UKismetMathLibrary::VSizeXY(Velocity);

	if (MoveForwardValue < 0)
	{
		IsSprinting = false;
	}

	if (CurrentSpeed == 0)
	{
		IsSprinting = false;
		bUseControllerRotationYaw = false;
	}
	else
	{
		bUseControllerRotationYaw = true;
	}
	AdjustSpeedMultiplier();
}



#pragma endregion
