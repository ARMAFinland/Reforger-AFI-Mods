class AFI_CapturePointAction : ScriptedUserAction
{
	protected AFI_CapturePoint m_capturePoint;
	
	protected bool m_bActionInProgress = false;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init();
		
		m_capturePoint = AFI_CapturePoint.Cast(pOwnerEntity);
	}
	
	override void OnActionStart(IEntity pUserEntity)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		IEntity controlledEntity = playerController.GetControlledEntity();
		if (!controlledEntity)
			return;
		
		if (pUserEntity != controlledEntity)
			m_bActionInProgress = true;
	}
	
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_bActionInProgress = false;
	}
	
	override bool CanBePerformedScript(IEntity user)
	{		
		if (!m_capturePoint)
			return false;
		
		if (m_bActionInProgress)
			return false;
		
		return !IsCapturePointControlled();
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		m_bActionInProgress = false;
		
		if (!Replication.IsServer())
			return;
		
		if (!m_capturePoint)
			return;
		
		FactionAffiliationComponent fcomp = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));		
		Faction faction = fcomp.GetDefaultAffiliatedFaction();
		FactionKey factionKey = faction.GetFactionKey();
		
		if (m_capturePoint.GetControlledBy() == factionKey)
			return;
		
		m_capturePoint.SetControlledBy(factionKey);
	}
	
	override bool GetActionNameScript(out string outName)
	{		
		if (!m_capturePoint)
			return false;
		
		if (IsCapturePointControlled())
			outName = "Captured";
		else
			outName = "Capture";
		
		if (m_bActionInProgress)
			outName += " (In Progress)";
		
		return true;
	}
	
	protected bool IsCapturePointControlled()
	{		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		IEntity player = playerController.GetControlledEntity();
		FactionAffiliationComponent fcomp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));		
		Faction faction = fcomp.GetDefaultAffiliatedFaction();
		FactionKey factionKey = faction.GetFactionKey();
		
		FactionKey current = m_capturePoint.GetControlledBy();
		
		return current == factionKey;
	}
}