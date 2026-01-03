class AFI_TeleportAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.EditBox, desc: "Name of the target capture point")]
    protected string m_sTargetCapturePoint;
	
	protected ref array<int> m_waitingPlayerIds = new array<int>();
	
	protected AFI_CapturePoint GetCapturePoint()
	{
		return AFI_CaptureGameManager.GetInstance().GetCapturePoint(m_sTargetCapturePoint);
	}
	
	override bool CanBePerformedScript(IEntity user)
	{
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		if (!capturePoint)
			return false;
		
		return IsCapturePointControlled();
	}
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer()) // Only runs on server
			return;
		
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		if (!capturePoint)
			return;
	
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		int playerId = playerManager.GetPlayerIdFromControlledEntity(pUserEntity);
		if (playerId == 0)
			return;
		
		GetGame().GetCallqueue().CallLater(DelayAddToWaiting, AFI_PlayerControllerComponent.FADE_DURATION * 1000, false, playerId);
		
		PlayerController playerController = playerManager.GetPlayerController(playerId);
		if (!playerController)
			return;
		
		AFI_PlayerControllerComponent afiComponent = AFI_PlayerControllerComponent.Cast(playerController.FindComponent(AFI_PlayerControllerComponent));
		if (!afiComponent)
			return;
		
		afiComponent.BlackScreen();
		afiComponent.ShowHint("Teleport", "Please stand by for teleportation");
	}
	
	protected void DelayAddToWaiting(int playerId)
	{
		m_waitingPlayerIds.Insert(playerId);
	}
	
	protected void DoTeleportation()
	{
		if (m_waitingPlayerIds.Count() == 0) // Nothing to teleport
			return;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		// Get the capture points's position
		vector objectivePos = capturePoint.GetOrigin();
		if (objectivePos == vector.Zero)
			return;
		
		array<vector> availablePositions = new array<vector>();
		int positionCount = SCR_WorldTools.FindAllEmptyTerrainPositions(availablePositions, objectivePos, 50, 2, 2, m_waitingPlayerIds.Count());
		
		array<int> skippedPlayerIds = new array<int>();
		
		foreach (int i, int playerId : m_waitingPlayerIds)
		{
			if (i >= availablePositions.Count())
			{
				skippedPlayerIds.Insert(playerId);
				continue;
			}
			
			PlayerController playerController = playerManager.GetPlayerController(playerId);
			if (!playerController)
				return;
			
			AFI_PlayerControllerComponent afiComponent = AFI_PlayerControllerComponent.Cast(playerController.FindComponent(AFI_PlayerControllerComponent));
			if (!afiComponent)
				return;
			
			afiComponent.Teleport(availablePositions[i], i * 250); // Delay 250ms per player
		}
		
		m_waitingPlayerIds = skippedPlayerIds;
	}
	
	override bool CanBeShownScript(IEntity user) {
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		return capturePoint != null;
	}
	
	// Display the action in the interaction menu
	override bool GetActionNameScript(out string outName)
	{
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		if (!capturePoint)
			return false;
		
		outName = "Teleport to " + capturePoint.GetCapturePointName();
		
		if (!IsCapturePointControlled())
			outName += " (Not Controlled)"; 
		
		return true;
	}
	
	protected bool IsCapturePointControlled()
	{
		AFI_CapturePoint capturePoint = GetCapturePoint();
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		IEntity player = playerController.GetControlledEntity();
		FactionAffiliationComponent fcomp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));		
		Faction faction = fcomp.GetDefaultAffiliatedFaction();
		
		return capturePoint.CanBeTeleported(faction);
	}
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		if (!Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(DoTeleportation, 5000, true);
	}
}