// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include "InputMappingContext.h"
#include "ui/GameHUD.h"
#include "Logging/StructuredLog.h"

UInputAction * AGamePlayerController::MapKey(FKey key, EInputActionValueType type, bool triggerWhenPaused) {
	UInputAction* act = NewObject<UInputAction>(this);
	act->bTriggerWhenPaused = triggerWhenPaused;
	act->ValueType = type;
	DefaultMappingContext->MapKey(act, key);
	return act;
}
void AGamePlayerController::MapKey(UInputAction* act, FKey key, bool negateX, bool negateY, bool negateZ) {
	FEnhancedActionKeyMapping& mapping = DefaultMappingContext->MapKey(act, key);
	UObject* outer = DefaultMappingContext->GetOuter();

	UInputModifierNegate* neg = NewObject<UInputModifierNegate>(outer);
	neg->bX = negateX;
	neg->bY = negateY;
	neg->bZ = negateZ;
	mapping.Modifiers.Add(neg);
}
void AGamePlayerController::MapKey(UInputAction * act, FKey key, bool negate, bool swizzle,  EInputAxisSwizzle order) {
	FEnhancedActionKeyMapping& mapping = DefaultMappingContext->MapKey(act, key);
	UObject* outer = DefaultMappingContext->GetOuter();
	if (negate) {
		UInputModifierNegate* neg = NewObject<UInputModifierNegate>(outer);
		mapping.Modifiers.Add(neg);
	}
	if (swizzle) {
		UInputModifierSwizzleAxis* sw = NewObject<UInputModifierSwizzleAxis>(outer);
		sw->Order = order;
		mapping.Modifiers.Add(sw);
	}
}
void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	DefaultMappingContext = NewObject<UInputMappingContext>(this);

	MoveAction = NewObject<UInputAction>(this);
	MoveAction->ValueType = EInputActionValueType::Axis2D;

	MapKey(MoveAction, EKeys::W, false, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::S, true, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::A, true);
	MapKey(MoveAction, EKeys::D);

	MapKey(MoveAction, EKeys::Up, false, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::Down, true, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::Left, true);
	MapKey(MoveAction, EKeys::Right);

	LookAction = NewObject<UInputAction>(this);
	LookAction->ValueType = EInputActionValueType::Axis2D;
	MapKey(LookAction, EKeys::Mouse2D, false, true, false);

	JumpAction = MapKey(EKeys::SpaceBar);
	AttackAction = MapKey(EKeys::LeftMouseButton);
	InteractAction = MapKey(EKeys::E);
	LockAction = MapKey(EKeys::MiddleMouseButton);
	ZoomAction = MapKey(EKeys::MouseWheelAxis, EInputActionValueType::Axis1D);
	OpenInventoryAction = MapKey(EKeys::C, EInputActionValueType::Boolean, true);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	// Set up action bindings
	
}

void AGamePlayerController::SetPawn(APawn* pawn)
{
	Super::SetPawn(pawn);
	GameCharacter = Cast<AGameCharacter>(pawn);
	if (GameCharacter) {

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

			EnhancedInputComponent->ClearActionBindings();
			// Jumping
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, GameCharacter, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::StopJumping);

			// Moving
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::Move);

			// Looking
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::Look);

			// Zoom in/out
			EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::CameraZoom);
			EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::LockOntoEnemy);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::Interact);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::Attack);

			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AGamePlayerController::TriggerInventory);

		}
	}
}


void AGamePlayerController::TriggerInventory(const FInputActionValue& Value)
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if(hud->canOpenInventory()){
			const bool openInv = hud->isInventoryOpen();
			if (openInv) {
				hud->hideInventory();
				FInputModeGameOnly Mode;
				SetInputMode(Mode);
			}
			else {
				FInputModeGameAndUI Mode;
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
				Mode.SetHideCursorDuringCapture(false);
				Mode.SetWidgetToFocus(hud->showInventory(GameCharacter)->TakeWidget());
				SetInputMode(Mode);
				
			}
			SetShowMouseCursor(!openInv);
			SetPause(!openInv);
		}
	}

}