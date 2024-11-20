[ComponentEditorProps(category: "AFI/GameMode/Components", description: "Base for gamemode scripted component.")]
class AFI_JIPGameModeComponentClass : SCR_BaseGameModeComponentClass
{
}

class AFI_JIPGameModeComponent : SCR_BaseGameModeComponent
{
	protected PS_PlayableManager m_PlayableManager;
	protected ref map<RplId, PS_PlayableComponent> m_mPlayables = new map<RplId, PS_PlayableComponent>;
	
	//------------------------------------------------------------------------------------------------
	void StartDeleteTimer(int delay)
	{
		if (Replication.IsServer())
			GetGame().GetCallqueue().CallLater(DoDeleteRedundantUnits, delay, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DoDeleteRedundantUnits()
	{
		m_PlayableManager = PS_PlayableManager.GetInstance();
		
		m_mPlayables = m_PlayableManager.GetPlayables();
		GetGame().GetCallqueue().CallLater(DeleteRedundantUnitsSlowly, 94, false, 0);
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteRedundantUnitsSlowly(int i) 
	{
		if (i >= m_mPlayables.Count())
			return;
		
		PS_PlayableComponent playable = m_mPlayables.GetElement(i);
		if(playable && m_PlayableManager.GetPlayerByPlayable(playable.GetId()) <= 0)
		{
			SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(playable.GetOwner());
			SCR_EntityHelper.DeleteEntityAndChildren(character);
		} else
		{
			i = i + 1;
		}
			
		GetGame().GetCallqueue().CallLater(DeleteRedundantUnitsSlowly, 94, false, i);
	}
}