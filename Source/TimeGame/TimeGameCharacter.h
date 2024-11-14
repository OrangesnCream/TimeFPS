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
	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

public:
		
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

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Allows Character to Sprint */
	void Sprint();
	void StopSprinting();

	// Dash
	void Dash();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	ATimeManager* FindTimeManager();
	ATimeManager* timeManager;
	void slowTime();

	// Crouch
	void ToggleCrouch();

private:
	void InitializeStateMachine();
	void ResetDash();

	bool bCanDash, bIsCrouching;
	double maxWalkSpeedReset;
	FGameplayTag CurrentStateTag;

	// Timer handle for dash cooldown
	FTimerHandle DashCooldownTimerHandle;

	UPROPERTY(EditAnywhere) float StandingCamHeight;
	UPROPERTY(EditAnywhere) float CrouchedCamHeight;
	UPROPERTY(EditAnywhere) float StandingCapsuleHeight;
	UPROPERTY(EditAnywhere) float CrouchedCapsuleHeight;
	FTimeline CrouchTimeline;
	UFUNCTION() void UpdateCamHeight(float value);

};

