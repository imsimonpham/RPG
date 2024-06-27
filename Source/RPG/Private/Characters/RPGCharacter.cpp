#include "Characters/RPGCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharactermovementComponent.h"

ARPGCharacter::ARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Controller Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//CameraBoom and Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Match character rotation with moving direction
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region PlayerInput
void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ARPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ARPGCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ARPGCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ARPGCharacter::MoveRight);
}

void ARPGCharacter::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		//FRotator3d(double InPitch, double InYaw, double InRoll)
		const FRotator  ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //X is forward vector
		AddMovementInput(Direction, Value);
	}
}
void ARPGCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}
void ARPGCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ARPGCharacter::MoveRight(float Value)
{
	if (Controller && (Value != 0.f))
	{
		//FRotator3d(double InPitch, double InYaw, double InRoll)
		const FRotator  ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //Y is right vector
		AddMovementInput(Direction, Value);
	}
}


#pragma endregion
