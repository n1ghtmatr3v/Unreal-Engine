// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThirdPersonGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionTriggered);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AThirdPersonGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Sprint Input Action */
	UPROPERTY(Transient)
	UInputAction* SprintAction;

	/** Interaction Input Action */
	UPROPERTY(Transient)
	UInputAction* InteractionAction;

	/** Runtime mapping context containing the homework input actions */
	UPROPERTY(Transient)
	UInputMappingContext* HomeworkMappingContext;

	/** Normal movement speed */
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float WalkSpeed = 500.0f;

	/** Movement speed while sprinting */
	UPROPERTY(EditDefaultsOnly, Category="Sprint")
	float SprintSpeed = 850.0f;

	/** True while the character is sprinting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sprint")
	bool bIsSprinting = false;

	/** Maximum stamina value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stamina")
	float MaxStamina = 100.0f;

	/** Current stamina value */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stamina")
	float Stamina = 100.0f;

	/** Stamina consumed per second while sprinting */
	UPROPERTY(EditDefaultsOnly, Category="Stamina")
	float StaminaDrainRate = 20.0f;

	/** Stamina restored per second while not sprinting */
	UPROPERTY(EditDefaultsOnly, Category="Stamina")
	float StaminaRecoveryRate = 15.0f;

	/** Prevents sprint from restarting before the key is released after exhaustion */
	bool bStaminaExhausted = false;

public:

	/** Constructor */
	AThirdPersonGameCharacter();

	/** Creates the runtime Enhanced Input assets before possession */
	virtual void PostInitializeComponents() override;

	/** Called every frame to update stamina */
	virtual void Tick(float DeltaSeconds) override;

	/** Fired when the interaction input is pressed */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionTriggered OnInteractionTriggered;

protected:

	/** Initializes runtime delegate bindings */
	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Starts sprinting if stamina is available */
	void StartSprint();

	/** Stops sprinting when the key is released */
	void StopSprint();

	/** Broadcasts the interaction delegate */
	void Interact();

	/** Applies the sprint state and movement speed */
	void SetSprinting(bool bNewIsSprinting);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

