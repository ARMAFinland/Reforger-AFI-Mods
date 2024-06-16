[EntityEditorProps(category: "AFI/Systems/Teleport", description: "Waiting area for teleporters")]
class AFI_TeleportWaitingAreaEntityClass : SCR_CharacterTriggerEntityClass
{
}

class AFI_TeleportWaitingAreaEntity : SCR_CharacterTriggerEntity
{	
	[Attribute("900000", category: "AFI", desc: "Time to teleport players in this waiting area")]
	protected int m_iTeleportTime;
	
	[Attribute("", category: "AFI", desc: "Name of the area where to teleport")]
	protected string m_sTeleportTargetArea;
	
	protected AFI_TeleportManager m_TeleportManagerEntity;
	
	protected float m_fZoneRadiusSq;
	
	//------------------------------------------------------------------------------------------------
	float GetZoneRadiusSq()
	{
		return m_fZoneRadiusSq;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetTeleportTargetArea()
	{
		return m_sTeleportTargetArea;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetTeleportTime()
	{
		return m_iTeleportTime;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		
		m_TeleportManagerEntity.GetOnPlayerEnterWaitingArea().Invoke(ent, this);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		m_fZoneRadiusSq = Math.Pow(GetSphereRadius(), 2);
		
		m_TeleportManagerEntity = AFI_TeleportManager.Cast(GetGame().GetGameMode().FindComponent(AFI_TeleportManager));

		if (m_TeleportManagerEntity == null)
			return;
		
		m_TeleportManagerEntity.RegisterWaitingArea(this);
	}		
}