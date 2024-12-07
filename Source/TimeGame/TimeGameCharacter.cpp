// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeGameCharacter.h"
#include "TimeGameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimeManager.h" // custom class for time manipulation
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerIdle.h"
#include "PlayerRun.h"
#include "TimerManager.h"
//ai
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATimeGameCharacter

ATimeGameCharacter::ATimeGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	maxWalkSpeedReset = GetCharacterMovement()->MaxWalkSpeed;

	//ai code
	SetupStimulusSource();
	//health code
	playerHealth = 10;

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Dash Variables
	DashDistance = 1122;
	DashCooldown = .66f;
	bCanDash = true;

	// Crouch Variables
	bIsCrouching = false;
	CrouchEyeOffset = FVector(0);
	CrouchSpeed = 12;

	// Matling Variables
	bIsMantling = false;
	bIsSprinting = false;
	MoveVector = FVector2D(0);
}

void ATimeGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	timeManager = FindTimeManager();

	// Initialize the Character's state machine
	StateMachine = NewObject<UBasicStateMachine>(this);
	InitializeStateMachine();

	CurrentStateTag = FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle"));
	StateMachine->RequestState(CurrentStateTag);
}

void ATimeGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//CrouchTimeline.TickTimeline(DeltaTime);

	float CrouchInterpTime = FMath::Min(1, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1 - CrouchInterpTime) * CrouchEyeOffset;

	if (bIsMantling)
		HandleMantle(DeltaTime);
	else if (DetectLedge(LedgeLocation, LedgeNormal))
		StartMantle();

	// Monitor for ground exit
	if (!GetCharacterMovement()->IsMovingOnGround() && !bIsInCoyoteTime)
		EnterCoyoteTime();

	// Update the current state
	/*
	if (UStateBase* CurrentState = StateMachine->GetStateFromCache(CurrentStateTag))
		CurrentState->UpdateState(DeltaTime);*/
}

//////////////////////////////////////////////////////////////////////////// Input

void ATimeGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATimeGameCharacter::Move);

		// Sprinting
		PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATimeGameCharacter::ToggleSprint);

		// Dashing
		PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATimeGameCharacter::Dash);

		// Crouching
		PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATimeGameCharacter::ToggleCrouch);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATimeGameCharacter::Look);
	

		PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ATimeGameCharacter::slowTime);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATimeGameCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0)
		return;

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust * .66, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + (HalfHeightAdjust * .66);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, BaseEyeHeight), false);
}

void ATimeGameCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0)
		return;

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust * .66, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - (HalfHeightAdjust * .66);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, BaseEyeHeight), false);
}

void ATimeGameCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}


void ATimeGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>().GetSafeNormal();

	/*if (!MovementVector.IsZero() && CurrentStateTag == FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle")))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Transitioning from Idle to Run"));
		CurrentStateTag = FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Run"));
		StateMachine->RequestState(CurrentStateTag);
	}
	else if (MovementVector.IsZero() && CurrentStateTag == FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Run")))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Transitioning from Run to Idle"));
		CurrentStateTag = FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle"));
		StateMachine->RequestState(CurrentStateTag);
	}*/

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}

	if (Value.Get<FVector2D>().GetSafeNormal().IsZero() && bIsSprinting)
	{
		StopSprinting();
		bIsSprinting = false;
	}
}

void ATimeGameCharacter::ToggleSprint()
{
	if (bIsSprinting)
		StopSprinting();
	else
		Sprint();
	bIsSprinting = !bIsSprinting;
}

void ATimeGameCharacter::Sprint()
{
	if (bIsCrouching)
	{
		UnCrouch();
		bIsCrouching = !bIsCrouching;
	}
	GetCharacterMovement()->MaxWalkSpeed *= 1.75;
}

void ATimeGameCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeedReset;
}

void ATimeGameCharacter::Dash()
{
	if (bCanDash)
	{
		if (GetLastMovementInputVector().IsZero())
		{
			const FVector ForwardDodge = (this->GetActorRotation().Vector() * 1.25) + (GetActorUpVector() * .33);
			LaunchCharacter(ForwardDodge * DashDistance, true, true);
		}
		else
		{
			FVector DashDirection = (GetLastMovementInputVector().GetSafeNormal() * 1.33) + (GetActorUpVector() * .3);
			LaunchCharacter(DashDirection * DashDistance, true, true);
		}
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			bCanDash = false;
			GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ATimeGameCharacter::ResetDash, DashCooldown * 1.75, false);
		}
	}

	if (!bIsSprinting)
	{
		Sprint();
		bIsSprinting = true;
	}
}

void ATimeGameCharacter::ToggleCrouch()
{
	if (bIsCrouching) UnCrouch();
	else Crouch();
	bIsCrouching = !bIsCrouching;
}

void ATimeGameCharacter::StartMantle()
{
	if (!bIsMantling)
	{
		bIsMantling = true;
		LedgeLocation = GetActorLocation() + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		Crouch();
		// TODO: Play matling anim here
	}
}

void ATimeGameCharacter::StopMantle()
{
	bIsMantling = false;
	UnCrouch();
}

void ATimeGameCharacter::HandleMantle(float DeltaTime)
{
	FVector
		CurrentLocation = GetActorLocation(),
		TargetLocation = LedgeLocation,
		NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 300);
	SetActorLocation(NewLocation);

	if (FVector::Dist(NewLocation, TargetLocation) < .33333333333333333333333333333333333333333333333333333333f)
		StopMantle();
}

void ATimeGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

ATimeManager* ATimeGameCharacter::FindTimeManager()
{
	for (TActorIterator<ATimeManager> It(GetWorld()); It; ++It)
	{
		return *It;  //find the timemanager
	}

	// If no TimeManager is found return nullptr
	return nullptr;
}
void ATimeGameCharacter::slowTime() {
	if (timeManager) {
		timeManager->GlobalActorSlowdown();
	}
}

//health functions
int ATimeGameCharacter::setHealth(int newHealth)
{
	if (newHealth <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("PLAYER DEAD, ADD NEW FUNCTIONALITY WITH THIS TRIGGER LATER "));
		return 0;
	}
	playerHealth = newHealth;
	return playerHealth;
}

int ATimeGameCharacter::getHealth()
{
	return playerHealth;
}

void ATimeGameCharacter::InitializeStateMachine()
{
	StateMachine->AddStateToCache(FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle")), NewObject<UPlayerIdle>(this));
	StateMachine->AddStateToCache(FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Run")), NewObject<UPlayerRun>(this));
}

void ATimeGameCharacter::ResetDash() { bCanDash = true; }

void ATimeGameCharacter::EnterCoyoteTime()
{
	bIsInCoyoteTime = true;

	// Start the coyote time timer
	GetWorld()->GetTimerManager().SetTimer(CoyoteTimeTimerHandle, this, &ATimeGameCharacter::ExitCoyoteTime, CoyoteTimeDuration, false);
}

void ATimeGameCharacter::ExitCoyoteTime() { bIsInCoyoteTime = false; }

bool ATimeGameCharacter::CanJump() const
{
	// Allow jumping during coyote time or while on the ground
	return Super::CanJump() || bIsInCoyoteTime;
}

void ATimeGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	// Reset coyote time when landing
	bIsInCoyoteTime = false;
	GetWorld()->GetTimerManager().ClearTimer(CoyoteTimeTimerHandle);
}

bool ATimeGameCharacter::DetectLedge(FVector& OutLedgeLocation, FVector& OutLedgeNormal)
{
	FVector
		Start = GetActorLocation(),
		ForwardVector = GetActorForwardVector(),
		End = Start + (ForwardVector * 66.66666666666666666666666666666666666666666666666666f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		OutLedgeLocation = HitResult.ImpactPoint;
		OutLedgeNormal = HitResult.ImpactNormal;
		return true;
	}
	return false;
}

//**** code for AI detection ****
void ATimeGameCharacter::SetupStimulusSource() {
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource) {
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}