#include "Characters/RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharactermovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"

ARPGCharacter::ARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//CameraBoom and Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

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
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ARPGCharacter::Dodge);
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
	MoveForwardValue = MoveRightValue == 0 ? Value : Value / FMath::Sqrt(2.0f);

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
	MoveRightValue = MoveForwardValue == 0 ? Value : Value / FMath::Sqrt(2.0f);

	if (Controller && (MoveRightValue != 0.f))
	{
		//FRotator3d(double InPitch, double InYaw, double InRoll)
		const FRotator  ControlRotation = GetActorRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //Y is right vector
		AddMovementInput(Direction.GetSafeNormal(), MoveRightValue * SprintingSpeedMultiplier);
	}
}

void ARPGCharacter::Sprint()
{
	IsSprinting = !IsSprinting;
}

void ARPGCharacter::Dodge()
{	
	// Map to store montage section names based on MoveForwardValue and MoveRightValue
	TMap<FVector2D, FName> DirectionMap;
	DirectionMap.Add(FVector2D(0.f, 0.f), FName("Dodge_Neutral"));
	DirectionMap.Add(FVector2D(1.f, 0.f), FName("Dodge_Forward"));
	DirectionMap.Add(FVector2D(-1.f, 0.f), FName("Dodge_Backward"));
	DirectionMap.Add(FVector2D(0.f, 1.f), FName("Dodge_Right"));
	DirectionMap.Add(FVector2D(0.f, -1.f), FName("Dodge_Left"));
	DirectionMap.Add(FVector2D(1.f, 1.f), FName("Dodge_Forward_Right"));
	DirectionMap.Add(FVector2D(1.f, -1.f), FName("Dodge_Forward_Left"));
	DirectionMap.Add(FVector2D(-1.f, 1.f), FName("Dodge_Backward_Right"));
	DirectionMap.Add(FVector2D(-1.f, -1.f), FName("Dodge_Backward_Left"));

	// Create a vector representing current movement direction
	FVector2D GroundDirection = FVector2D(MoveForwardValue, MoveRightValue).GetSafeNormal();

	// Find the closest matching direction in the map (using tolerance)
	float BestMatchDot = -2.f; // Initialize to a value lower than -1 to ensure it's updated
	FName MontageSectionName = FName("Dodge_Neutral"); // Default to neutral if no exact match found

	for (const auto& Pair : DirectionMap)
	{
		float Dot = FVector2D::DotProduct(GroundDirection, Pair.Key);

		// Use a small tolerance to handle floating point precision issues
		if (Dot > BestMatchDot + 0.1f)
		{
			BestMatchDot = Dot;
			MontageSectionName = Pair.Value;
		}
	}

	// Play the montage section
	if (ActionState == EActionState::EAS_Neutral)
	{
		PlayMontageSection(DodgeMontage, MontageSectionName);
		ActionState = EActionState::EAS_Dodging;
	}
	
}

void ARPGCharacter::AdjustSpeedMultiplier()
{
	SprintingSpeedMultiplier = IsSprinting ? 1.f : 0.5f;
}

void ARPGCharacter::AdjustCameraDistance()
{
	if (IsSprinting || ActionState == EActionState::EAS_Dodging)
	{
		IncreaseCameraDistance();
		ClearTimer();
	}
	else
	{
		if (!IsTimerActive())
		{
			//reset camera distance with a delay
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARPGCharacter::ResetCameraDistance, 1.0f, false);
		}
	}

	float TargetLength = CanAdjustCameraDistance ? MaxTargetArmLength : MinTargetArmLength;
	float CurrentLength = CameraBoom->TargetArmLength;
	CameraBoom->TargetArmLength = FMath::Lerp(CurrentLength, TargetLength, InterpSpeed * GetWorld()->DeltaTimeSeconds);
}

void ARPGCharacter::IncreaseCameraDistance()
{
	CanAdjustCameraDistance = true;
}

void ARPGCharacter::ResetCameraDistance()
{
	CanAdjustCameraDistance = false;
}

void ARPGCharacter::ClearTimer()
{
	if (IsTimerActive())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

bool ARPGCharacter::IsTimerActive()
{
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle);
}
 

void ARPGCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Neutral;
}

void ARPGCharacter::CalculateMovementSpeed()
{
	FVector Velocity = GetCharacterMovement()->Velocity;
	CurrentSpeed = UKismetMathLibrary::VSizeXY(Velocity);

	if (MoveForwardValue < 0 || CurrentSpeed == 0)
	{
		IsSprinting = false;
	}

	bUseControllerRotationYaw = CurrentSpeed == 0 ? false : true;

	AdjustSpeedMultiplier();
	AdjustCameraDistance();
}

void ARPGCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

#pragma endregion
