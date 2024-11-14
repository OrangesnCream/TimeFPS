// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BasicStateMachine.h"
#include <Components/TimelineComponent.h>
#include "TimeGameCharacter.generated.h"
//#include "TimeManager.h"
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class ATimeManager;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATimeGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
public:
	ATimeGameCharacter();

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// State Machine for managing character states
	UPROPERTY() UBasicStateMachine* StateMachine;

	// Dash variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash") float DashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash") float DashCooldown;

	// Crouch variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Crouch) FVector CrouchEyeOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Crouch) float CrouchSpeed;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// Time Dilation
	ATimeManager* FindTimeManager();
	ATimeManager* timeManager;
	void slowTime();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Allows Character to Sprint */
	void ToggleSprint();
	void Sprint();
	void StopSprinting();

	// Dash
	void Dash();

	// Crouch
	void ToggleCrouch();

	// Slide
	void StartSlide();
	void StopSlide();
	void HandleSlide(float DeltaTime);

	// Mantling
	void StartMantle();
	void StopMantle();
	void HandleMantle(float DeltaTime);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

private:
	void InitializeStateMachine();
	void ResetDash();
	bool IsGrounded() const;
	bool DetectLedge(FVector& OutLedgeLocation, FVector& OutLedgeNormal);

	bool bCanDash, bIsCrouching, bIsSprinting, bIsSliding, bIsMantling;
	double maxWalkSpeedReset;
	FGameplayTag CurrentStateTag;

	// Timer handle for dash cooldown
	FTimerHandle DashCooldownTimerHandle;

	FVector InitialVelocity, LedgeLocation, LedgeNormal;
	float DecelerationRate;

};

