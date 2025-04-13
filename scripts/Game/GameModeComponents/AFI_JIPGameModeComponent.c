[ComponentEditorProps(category: "AFI/GameMode/Components", description: "Base for gamemode scripted component.")]
class AFI_JIPGameModeComponentClass : SCR_BaseGameModeComponentClass
{
}

class AFI_JIPGameModeComponent : SCR_BaseGameModeComponent
{	
	//------------------------------------------------------------------------------------------------
	void StartDeleteTimer(int delay)
	{
		if (Replication.IsServer())
			GetGame().GetCallqueue().CallLater(PS_PlayableManager.GetInstance().RemoveRedundantUnits, delay, false);
	}
}