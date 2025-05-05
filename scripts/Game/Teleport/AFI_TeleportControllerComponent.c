class AFI_TeleportControllerComponentClass : ScriptComponentClass
{
}

class AFI_TeleportControllerComponent : ScriptComponent
{
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
				
		ActionManager actionManager = playerController.GetActionManager();
		if (!actionManager)
			return;
		
		actionManager.SetActionValue("CharacterFire", 0);
		actionManager.SetActionValue("CharacterThrowGrenade", 0);
		actionManager.SetActionValue("CharacterMelee", 0);
		actionManager.SetActionValue("CharacterFireStatic", 0);
		actionManager.SetActionValue("TurretFire", 0);
		actionManager.SetActionValue("VehicleFire", 0);
		actionManager.SetActionValue("VehicleHorn", 0);
	}
	
	override protected void OnPostInit(IEntity owner)
	{
		//SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
}