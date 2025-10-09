[ComponentEditorProps(category: "AFI/GameMode/Components", description: "Base for gamemode scripted component.")]
class AFI_JIPGameModeComponentClass : SCR_BaseGameModeComponentClass
{
}

class AFI_JIPGameModeComponent : SCR_BaseGameModeComponent
{	
	//------------------------------------------------------------------------------------------------	
	override void OnGameModeStart()
	{
		if (!Replication.IsServer())
			return;
		
		PS_GameModeCoop gameMode = PS_GameModeCoop.	Cast(GetGameMode());
		if (gameMode == null)
			return;
		
		if (!gameMode.GetRemoveRedundantUnits())
			return;
		
		GetGame().GetCallqueue().CallLater(CheckIfFreezeTimeHasEnded, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------	
	protected void CheckIfFreezeTimeHasEnded()
	{
		PS_GameModeCoop gameMode = PS_GameModeCoop.	Cast(GetGameMode());
		if (gameMode == null)
			return;
		
		if (!gameMode.IsFreezeTimeEnd())
		{
			GetGame().GetCallqueue().CallLater(CheckIfFreezeTimeHasEnded, 1000, false);
			return;
		}
		
		GetGame().GetCallqueue().CallLater(PS_PlayableManager.GetInstance().RemoveRedundantUnits, 1000, false);
	}
}