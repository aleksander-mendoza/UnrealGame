//MIT 
//Based on https://dev.epicgames.com/community/snippets/Qm2/unreal-engine-enhanced-input-double-tap-input-trigger
//Thanks to user colorindarkness 
#include "TapOrHold.h"


ETriggerState UTapOrHold::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{

	float LastHeldDuration = HeldDuration;

	// Transition to Ongoing on actuation. Update the held duration.
	if (IsActuated(ModifiedValue))
	{
		
		HeldDuration += DeltaTime;
		
		
	}
	else
	{
		// Reset duration
		HeldDuration = 0.0f;

		// Only trigger if pressed then released quickly enough
		if (IsActuated(LastValue) && LastHeldDuration < TapReleaseTimeThreshold)
		{
			return ETriggerState::Triggered;
		}
	}
	return ETriggerState::Ongoing;

	
}