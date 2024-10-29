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

	PrimaryActorTick.bCanEverTick = true;
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

	// Dash Variables
	DashDistance = 1234.56f;
	DashCooldown = .75f;
	bCanDash = true;
}

void ATimeGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATimeGameCharacter::Sprint);
		PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATimeGameCharacter::StopSprinting);

		// Dashing
		PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ATimeGameCharacter::Dash);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATimeGameCharacter::Look);
	

		PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ATimeGameCharacter::slowTime);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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
}

void ATimeGameCharacter::Sprint()
{
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
		FVector DashDirection = (GetLastMovementInputVector().GetSafeNormal() * 1.25) + (GetActorUpVector() * .25);
		LaunchCharacter(DashDirection * DashDistance, true, true);

		bCanDash = false;
		GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &ATimeGameCharacter::ResetDash, DashCooldown, false);
	}
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

void ATimeGameCharacter::InitializeStateMachine()
{
	StateMachine->AddStateToCache(FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Idle")), NewObject<UPlayerIdle>(this));
	StateMachine->AddStateToCache(FGameplayTag::RequestGameplayTag(FName("PlayerState.Ground.Run")), NewObject<UPlayerRun>(this));
}

void ATimeGameCharacter::ResetDash()
{
	bCanDash = true;
}
