class AFI_CapturePointClass: GenericEntityClass { }


class AFI_CapturePoint : GenericEntity
{
	[Attribute("", UIWidgets.EditBox, "Name of the capture point")]
	protected string m_sCapturePointName;	
	
	[Attribute("", UIWidgets.EditBox, "Controlled by"), RplProp()]
	protected FactionKey m_sControlledBy;
	
	[RplProp()]
	protected bool m_bIsContested = false;
	
	protected AFI_CapturePointTrigger m_trigger;
	protected FactionManager m_factionManager;
	protected SCR_FlagComponent m_flag;
	
	//------------------------------------------------------------------------------------------------
	string GetCapturePointName()
	{
		return m_sCapturePointName;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetControlledBy(string factionKey)
	{
		if (m_sControlledBy == factionKey)
			return;
		
		m_sControlledBy = factionKey;
		Replication.BumpMe();
		
		m_trigger.AddOwnerFaction(factionKey);
		
		Rpc(ChangeFlag, factionKey);
		if (Replication.IsServer())
			ChangeFlag(factionKey);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void ChangeFlag(string factionKey)
	{
		Faction faction = m_factionManager.GetFactionByKey(factionKey);
		if (!faction)
			return;
		
		SCR_Faction factionCast = SCR_Faction.Cast(faction);
		if(m_flag && factionCast)
			m_flag.ChangeMaterial(factionCast.GetFactionFlagMaterial());
	}
	
	//------------------------------------------------------------------------------------------------
	FactionKey GetControlledBy()
	{
		return m_sControlledBy;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnQueryFinished_Callback(bool result)
	{		
		if (m_bIsContested == result)
			return;
		
		m_bIsContested = result;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanBeTeleported(Faction faction)
	{
		return IsControlledBy(faction) && !IsContested();
	}

	//------------------------------------------------------------------------------------------------
	bool IsControlledBy(Faction faction)
	{
		FactionKey factionKey = faction.GetFactionKey();
		
		return m_sControlledBy == factionKey;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsContested()
	{
		return m_bIsContested;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DelayedEOnInit()
	{
		m_factionManager = GetGame().GetFactionManager();
		if (!m_factionManager)
		{
			Print("Can't find faction manager", LogLevel.ERROR);
			return;
		}
		
		IEntity child = GetChildren();
		while (child != null && m_trigger == null)
		{
			m_trigger = AFI_CapturePointTrigger.Cast(child);
		}
		
		if (m_trigger == null)
		{
			Print("Can't find trigger", LogLevel.ERROR);
			return;
		}
		
		m_trigger.GetQueryFinishedWithResult().Insert(OnQueryFinished_Callback);
		
		Faction faction = m_factionManager.GetFactionByKey(m_sControlledBy);
		m_trigger.AddOwnerFaction(m_sControlledBy);
		
		SCR_Faction factionCast = SCR_Faction.Cast(faction);
		if(m_flag && factionCast)
			m_flag.ChangeMaterial(factionCast.GetFactionFlagMaterial());
		
		AFI_CaptureGameManager.GetInstance().RegisterCapturePoint(GetCapturePointName(), this);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode()) return;
		
		// Call later so that init has completed on everywhere.
		GetGame().GetCallqueue().CallLater(DelayedEOnInit, 1000, false);
		
		m_flag = SCR_FlagComponent.Cast(owner.FindComponent(SCR_FlagComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	void AFI_CapturePoint(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
}