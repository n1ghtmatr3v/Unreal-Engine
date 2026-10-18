// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "ThirdPersonGame.h"

AThirdPersonGameCharacter::AThirdPersonGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Stamina = MaxStamina;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AThirdPersonGameCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Create the homework input actions and mapping context in native code. This
	// keeps the template's original binary assets untouched and safe to load.
	SprintAction = NewObject<UInputAction>(this, TEXT("IA_Sprint"));
	SprintAction->ValueType = EInputActionValueType::Boolean;

	InteractionAction = NewObject<UInputAction>(this, TEXT("IA_Interaction"));
	InteractionAction->ValueType = EInputActionValueType::Boolean;

	HomeworkMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Homework"));
	HomeworkMappingContext->MapKey(SprintAction, EKeys::LeftShift);
	HomeworkMappingContext->MapKey(InteractionAction, EKeys::E);
}

void AThirdPersonGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (HomeworkMappingContext)
			{
				Subsystem->AddMappingContext(HomeworkMappingContext, 1);
			}
		}
	}

}

void AThirdPersonGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsSprinting)
	{
		Stamina = FMath::Max(0.0f, Stamina - StaminaDrainRate * DeltaSeconds);

		if (Stamina <= 0.0f)
		{
			bStaminaExhausted = true;
			SetSprinting(false);
		}
	}
	else
	{
		Stamina = FMath::Min(MaxStamina, Stamina + StaminaRecoveryRate * DeltaSeconds);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			0.0f,
			FColor::Cyan,
			FString::Printf(TEXT("Stamina: %.0f / %.0f"), Stamina, MaxStamina));
	}
}

void AThirdPersonGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonGameCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AThirdPersonGameCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonGameCharacter::Look);

		// Sprinting
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AThirdPersonGameCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AThirdPersonGameCharacter::StopSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AThirdPersonGameCharacter::StopSprint);
		}

		// Interaction
		if (InteractionAction)
		{
			EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AThirdPersonGameCharacter::Interact);
		}
	}
	else
	{
		UE_LOG(LogThirdPersonGame, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThirdPersonGameCharacter::StartSprint()
{
	if (!bStaminaExhausted && Stamina > 0.0f)
	{
		SetSprinting(true);
	}
}

void AThirdPersonGameCharacter::StopSprint()
{
	bStaminaExhausted = false;
	SetSprinting(false);
}

void AThirdPersonGameCharacter::Interact()
{
	OnInteractionTriggered.Broadcast();
}

void AThirdPersonGameCharacter::SetSprinting(bool bNewIsSprinting)
{
	bIsSprinting = bNewIsSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AThirdPersonGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AThirdPersonGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AThirdPersonGameCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AThirdPersonGameCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AThirdPersonGameCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AThirdPersonGameCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
