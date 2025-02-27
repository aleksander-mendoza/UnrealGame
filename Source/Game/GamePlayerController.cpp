// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerController.h"
#include "InputMappingContext.h"
#include "ui/GameHUD.h"
#include "input/TapOrHold.h"
#include "Logging/StructuredLog.h"

UInputAction * AGamePlayerController::MapKey(FKey key, EInputActionValueType type, bool triggerWhenPaused) {
	UInputAction* act = NewObject<UInputAction>(this);
	act->bTriggerWhenPaused = triggerWhenPaused;
	act->ValueType = type;
	DefaultMappingContext->MapKey(act, key);
	return act;
}
UInputAction* AGamePlayerController::MapTapKey(FKey key) {
	UInputAction* act = NewObject<UInputAction>(this);
	FEnhancedActionKeyMapping& mapping = DefaultMappingContext->MapKey(act, key);
	UObject* outer = DefaultMappingContext->GetOuter();
	UTapOrHold* tap = NewObject<UTapOrHold>(outer);
	mapping.Triggers.Add(tap);
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
	MoveAction->ValueType = EInputActionValueType::Axis3D;
	
	MapKey(MoveAction, EKeys::W, false, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::S, true, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::A, true);
	MapKey(MoveAction, EKeys::D);
	MapKey(MoveAction, EKeys::SpaceBar, false, true, EInputAxisSwizzle::ZYX);
	MapKey(MoveAction, EKeys::LeftControl, true, true, EInputAxisSwizzle::ZYX);

	MapKey(MoveAction, EKeys::Up, false, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::Down, true, true, EInputAxisSwizzle::YXZ);
	MapKey(MoveAction, EKeys::Left, true);
	MapKey(MoveAction, EKeys::Right);

	LookAction = NewObject<UInputAction>(this);
	LookAction->ValueType = EInputActionValueType::Axis2D;
	MapKey(LookAction, EKeys::Mouse2D, false, true, false);

	SlowWalkAction = MapKey(EKeys::V);
	RunAction = MapKey(EKeys::LeftShift);
	JumpAction = MapKey(EKeys::SpaceBar);
	LeftHandedAttackAction = MapKey(EKeys::LeftMouseButton);
	RightHandedAttackAction = MapKey(EKeys::RightMouseButton);
	AttackCancelAction = MapKey(EKeys::R);
	InteractAction = MapTapKey(EKeys::E);
	LockAction = MapKey(EKeys::MiddleMouseButton);
	ZoomAction = MapKey(EKeys::MouseWheelAxis, EInputActionValueType::Axis1D);
	CrouchAction = MapKey(EKeys::LeftControl);
	OpenInventoryAction = MapKey(EKeys::C, EInputActionValueType::Boolean, true);
	OpenRaceMenuAction = MapKey(EKeys::P, EInputActionValueType::Boolean);
	OpenBuildingInventoryAction = MapKey(EKeys::B, EInputActionValueType::Boolean);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	// Set up action bindings
	
}

void AGamePlayerController::SetPawn(APawn* pawn)
{
	if (GameCharacter != nullptr) {
		GameCharacter->GameMovement->Inventory->Health.Widget = nullptr;
	}
	Super::SetPawn(pawn);
	GameCharacter = Cast<AGameCharacter>(pawn);
	if (GameCharacter) {
		GameCharacter->GameController = this;
		if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
			GameCharacter->GameMovement->Inventory->Health.setWidget(hud->StatusWidget);
		}
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

			EnhancedInputComponent->ClearActionBindings();
			// Jumping
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::StopJumping);

			// Moving
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::Move);

			// Looking
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGamePlayerController::Look);

			// Zoom in/out
			EnhancedInputComponent->BindAction(SlowWalkAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::ToggleSlowWalk);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::StartRun);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::EndRun);
			EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::CameraZoom);
			EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::LockOntoEnemy);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, GameCharacter, &AGameCharacter::InteractTriggered);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::InteractStart);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::InteractEnd);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled, GameCharacter, &AGameCharacter::InteractEnd);
			EnhancedInputComponent->BindAction(LeftHandedAttackAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::LeftHandedAttackStart);
			EnhancedInputComponent->BindAction(LeftHandedAttackAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::LeftHandedAttackEnd);
			EnhancedInputComponent->BindAction(RightHandedAttackAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::RightHandedAttackStart);
			EnhancedInputComponent->BindAction(RightHandedAttackAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::RightHandedAttackEnd);
			EnhancedInputComponent->BindAction(AttackCancelAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::AttackCancel);
			
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, GameCharacter, &AGameCharacter::crouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, GameCharacter, &AGameCharacter::uncrouch);
			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AGamePlayerController::TriggerInventory);
			EnhancedInputComponent->BindAction(OpenRaceMenuAction, ETriggerEvent::Started, this, &AGamePlayerController::TriggerRaceMenu);
			EnhancedInputComponent->BindAction(OpenBuildingInventoryAction, ETriggerEvent::Started, this, &AGamePlayerController::TriggerBuildingInventory);

			
		}
	}
}

void AGamePlayerController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (LookAxisVector.X != 0.f )
	{
		AddYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.f)
	{
		AddPitchInput(LookAxisVector.Y);
	}
}

void AGamePlayerController::TriggerBuildingInventory(const FInputActionValue& Value) {
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->isRaceMenuOpen() || hud->isInventoryOpen())return;
		if (hud->canOpenBuildingInventory()) {
			const bool openInv = hud->isBuildingInventoryOpen();
			if (openInv) {
				hud->hideBuildingInventory();
				FInputModeGameOnly Mode;
				SetInputMode(Mode);
			}
			else {
				FInputModeGameAndUI Mode;
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
				Mode.SetHideCursorDuringCapture(false);
				Mode.SetWidgetToFocus(hud->showBuildingInventory(GameCharacter)->TakeWidget());
				SetInputMode(Mode);
			}
			SetShowMouseCursor(!openInv);
			//SetPause(!openInv);
		}
	}
}
void AGamePlayerController::TriggerInventory(const FInputActionValue& Value)
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->isRaceMenuOpen()||hud->isBuildingInventoryOpen())return;
		if(hud->canOpenInventory()){
			const bool openInv = hud->isInventoryOpen();
			if (openInv) {
				hud->hideInventory();
				FInputModeGameOnly Mode;
				SetInputMode(Mode);
			}
			else {
				FInputModeUIOnly Mode;
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
				Mode.SetWidgetToFocus(hud->showInventory(GameCharacter, this)->ItemListView->TakeWidget());
				SetInputMode(Mode);
				
			}
			SetShowMouseCursor(!openInv);
			//SetPause(!openInv);
		}
	}

}

void AGamePlayerController::CloseInventory()
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->isInventoryOpen()) {
			hud->hideInventory();
			FInputModeGameOnly Mode;
			SetInputMode(Mode);
			SetShowMouseCursor(false);
		}
	}
}

void AGamePlayerController::OpenDialogue(AGameCharacter * npc, AGameCharacter* player, const UDialogueStage * stage)
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->canOpenDialogue()) {
			FInputModeUIOnly Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			Mode.SetWidgetToFocus(hud->showDialogue(npc, player, stage)->TakeWidget());
			SetInputMode(Mode);
			SetShowMouseCursor(true);
		}
	}

}

void AGamePlayerController::ShowDialogueLine(FText name, FText text)
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->canOpenDialogue()) {
			hud->showLine(name, text);
		}
	}

}
void AGamePlayerController::CloseDialogue()
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		hud->hideDialogue();
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		SetShowMouseCursor(false);
	}
}

void AGamePlayerController::TriggerRaceMenu(const FInputActionValue& Value)
{
	if (AGameHUD* hud = Cast<AGameHUD>(GetHUD())) {
		if (hud->isInventoryOpen() || hud->isBuildingInventoryOpen())return;
		if (hud->canOpenRaceMenu()) {
			const bool openInv = hud->isRaceMenuOpen();
			if (openInv) {
				hud->hideRaceMenu();
				FInputModeGameOnly Mode;
				SetInputMode(Mode);
			}
			else {
				FInputModeGameAndUI Mode;
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
				Mode.SetHideCursorDuringCapture(false);
				Mode.SetWidgetToFocus(hud->showRaceMenu(GameCharacter->GameMovement->Inventory)->TakeWidget());
				SetInputMode(Mode);

			} 
			SetShowMouseCursor(!openInv);
		}
	}

}