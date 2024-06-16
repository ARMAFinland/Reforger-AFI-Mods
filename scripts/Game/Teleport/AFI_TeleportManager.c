void OnPlayerEnterWaitingArea(IEntity player, AFI_TeleportWaitingAreaEntity area);
typedef func OnPlayerEnterWaitingArea;
typedef ScriptInvokerBase<OnPlayerEnterWaitingArea> OnPlayerEnterWaitingAreaInvoker;

[ComponentEditorProps(category: "AFI/Systems/Teleport", description: "AFI Teleport Manager")]
class AFI_TeleportManagerClass : ScriptComponentClass
{
}

class AFI_TeleportManager : ScriptComponent
{
	protected ref OnPlayerEnterWaitingAreaInvoker m_OnPlayerEnterWaitingAreaInvoker = new OnPlayerEnterWaitingAreaInvoker();
	
	protected ref map<AFI_TeleportWaitingAreaEntity, ref AFI_TeleportWaitingAreaInfo> m_TeleportWaitingAreaInfos = new map<AFI_TeleportWaitingAreaEntity, ref AFI_TeleportWaitingAreaInfo>();
	
	protected PlayerManager m_PlayerManager;
	
	protected BaseWorld m_BaseWorld;
	
	protected const int m_iZoneCheckFrequency = 50;
	
	protected int m_iGridWidth = 5;
	
	protected int m_iMyAreaId = 0;
	
	protected AFI_TimerWidget m_TimerWidget;
	
	//------------------------------------------------------------------------------------------------
	void RegisterWaitingArea(AFI_TeleportWaitingAreaEntity area)
	{
		if (!Replication.IsServer())
			return;
		
		AFI_TeleportWaitingAreaInfo teleportWaitingAreaInfo = EnsureTeleportWaitingAreaInfo(area);
		
		float nextTeleportForAreaTime = QueueNextTeleportForArea(area);
		
		if (nextTeleportForAreaTime > 0)
			teleportWaitingAreaInfo.SetNextTeleportTime(nextTeleportForAreaTime);
	}
	
	//------------------------------------------------------------------------------------------------
	AFI_TeleportWaitingAreaInfo EnsureTeleportWaitingAreaInfo(AFI_TeleportWaitingAreaEntity area)
	{
		if (!m_TeleportWaitingAreaInfos.Contains(area))
		{
			m_TeleportWaitingAreaInfos.Set(area, new AFI_TeleportWaitingAreaInfo(m_TeleportWaitingAreaInfos.Count() + 1));
		}
		
		return m_TeleportWaitingAreaInfos.Get(area);
	}
	
	//------------------------------------------------------------------------------------------------
	float QueueNextTeleportForArea(AFI_TeleportWaitingAreaEntity area)
	{
		GetGame().GetCallqueue().CallLater(HandleTeleportForArea, area.GetTeleportTime(), false, area);
		
		if (m_BaseWorld == null)
			return -1;
		
		float nextTeleportTime = m_BaseWorld.GetWorldTime() + area.GetTeleportTime();
		return nextTeleportTime;
	}
	
	//------------------------------------------------------------------------------------------------
	OnPlayerEnterWaitingAreaInvoker GetOnPlayerEnterWaitingArea()
	{
		return m_OnPlayerEnterWaitingAreaInvoker;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerEnterWaitingAreaInvoked(IEntity player, AFI_TeleportWaitingAreaEntity area)
	{
		if (!Replication.IsServer())
			return;
		
		int playerId = m_PlayerManager.GetPlayerIdFromControlledEntity(player);
		
		AFI_TeleportWaitingAreaInfo teleportWaitingAreaInfo = EnsureTeleportWaitingAreaInfo(area);
	
		teleportWaitingAreaInfo.InsertPlayerId(playerId);
		
		DisablePlayerWeapons(playerId);
		
		float timeleft = m_BaseWorld.GetWorldTime() - teleportWaitingAreaInfo.GetNextTeleportTime();
		Rpc(HandleClientPlayerEnterWaitingArea, playerId, teleportWaitingAreaInfo.GetAreaId(), timeleft);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void HandleClientPlayerEnterWaitingArea(int playerId, int areaId, float timeleft)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (playerController == null || playerController.GetPlayerId() != playerId)
			return;
		
		m_iMyAreaId = areaId;
		
		Widget widget = GetGame().GetWorkspace().CreateWidgets("{89A2ACBC9433CEF2}UI/layouts/Timer.layout");
		m_TimerWidget = AFI_TimerWidget.Cast(widget.FindHandler(AFI_TimerWidget));
		m_TimerWidget.SetTime(timeleft);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateTimeleft()
	{
		foreach(AFI_TeleportWaitingAreaEntity area, AFI_TeleportWaitingAreaInfo teleportWaitingAreaInfo : m_TeleportWaitingAreaInfos)
		{
			float timeleft = teleportWaitingAreaInfo.GetNextTeleportTime() - m_BaseWorld.GetWorldTime();
			Rpc(HandleClientRemainingUpdate, teleportWaitingAreaInfo.GetAreaId(), timeleft);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void HandleClientRemainingUpdate(int areaId, float timeleft)
	{
		if (m_iMyAreaId != areaId)
			return;
		
		m_TimerWidget.SetTime(timeleft);
	}
	
	//------------------------------------------------------------------------------------------------
	PlayerController GetPlayerController(int playerId)
	{
		PlayerController playerController = m_PlayerManager.GetPlayerController(playerId);
		return playerController;
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetPlayerControlledEntity(int playerId)
	{
		PlayerController playerController = GetPlayerController(playerId);
		if (playerController == null)
			return null;		
		
		IEntity playerControlledEntity = playerController.GetControlledEntity();
		return playerControlledEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	protected EventHandlerManagerComponent GetPlayerEventHandlerManagerComponent(int playerId)
	{
		IEntity playerControlledEntity = GetPlayerControlledEntity(playerId);
		if (playerControlledEntity == null)
			return null;
		
		EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(playerControlledEntity.FindComponent(EventHandlerManagerComponent));
		return eventHandler;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DisablePlayerWeapons(int playerId)
	{
		EventHandlerManagerComponent eventHandler = GetPlayerEventHandlerManagerComponent(playerId);
		if (!eventHandler) return;
		
		eventHandler.RegisterScriptHandler("OnProjectileShot", this, OnWeaponFired);
		eventHandler.RegisterScriptHandler("OnGrenadeThrown", this, OnGrenadeThrown);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void EnablePlayerWeapons(int playerId)
	{
		EventHandlerManagerComponent eventHandler = GetPlayerEventHandlerManagerComponent(playerId);
		if (!eventHandler) return;
		
		eventHandler.RemoveScriptHandler("OnProjectileShot", this, OnWeaponFired);
		eventHandler.RemoveScriptHandler("OnGrenadeThrown", this, OnGrenadeThrown);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnWeaponFired(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{		
		// Get projectile and delete it
		delete entity;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnGrenadeThrown(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		if (!weapon)
			return;
		
		// Get grenade and delete it
		delete entity;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void AreaCheckUpdate()
	{
		vector areaOrigin;
		vector playerControllerEntityOrigin;
		float distanceSqrXZ;
		float areaRadius;
		
		foreach(AFI_TeleportWaitingAreaEntity area, AFI_TeleportWaitingAreaInfo teleportWaitingAreaInfo : m_TeleportWaitingAreaInfos)
		{
			areaOrigin = area.GetOrigin();
			
			foreach (int playerId : teleportWaitingAreaInfo.GetPlayerIds())
			{
				IEntity playerControllerEntity = GetPlayerControlledEntity(playerId);
				
				playerControllerEntityOrigin = playerControllerEntity.GetOrigin();
				
				distanceSqrXZ = vector.DistanceSqXZ(playerControllerEntityOrigin, areaOrigin);
				areaRadius = area.GetZoneRadiusSq();
				
				if (distanceSqrXZ > areaRadius)
				{
					Rpc(HandleClientTeleporting, playerId, areaOrigin);
					HandleClientTeleporting(playerId, areaOrigin);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleTeleportForArea(AFI_TeleportWaitingAreaEntity area)
	{
		string targetArea = area.GetTeleportTargetArea();
		AFI_TeleportTargetAreaEntity targetAreaEntity = AFI_TeleportTargetAreaEntity.Cast(GetGame().FindEntity(targetArea));
		vector targetAreaOrigin = targetAreaEntity.GetOrigin();
		
		AFI_TeleportWaitingAreaInfo teleportWaitingAreaInfo = EnsureTeleportWaitingAreaInfo(area);
		
		vector newPosition;
		int i = 0;
		foreach (int playerId : teleportWaitingAreaInfo.GetPlayerIds())
		{
			vector offsetVector = GetOffsetVectorFromIndex(i++);		
			newPosition = targetAreaOrigin + offsetVector;
			
			EnablePlayerWeapons(playerId);
			
			Rpc(HandleClientTeleporting, playerId, newPosition);
			HandleClientTeleporting(playerId, newPosition);
			
			Rpc(HandleClientResetState, playerId);
			HandleClientResetState(playerId);
		}
		
		teleportWaitingAreaInfo.ResetPlayerIds();
		
		float nextTeleportForAreaTime = QueueNextTeleportForArea(area);
		
		if (nextTeleportForAreaTime > 0)
			teleportWaitingAreaInfo.SetNextTeleportTime(nextTeleportForAreaTime);
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector GetOffsetVectorFromIndex(int i)
    {	
        int x = (i - 1) % m_iGridWidth;
        int z = (i - 1) / m_iGridWidth;
		int y = 0;
        
        return x + y + z;
    }
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void HandleClientTeleporting(int playerId, vector newPosition)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (playerController == null || playerController.GetPlayerId() != playerId)
			return;
		
		IEntity playerControlledEntity = playerController.GetControlledEntity();
		
		if (playerControlledEntity == null)
			return;
		
		playerControlledEntity.SetOrigin(newPosition);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void HandleClientResetState(int playerId)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (playerController == null || playerController.GetPlayerId() != playerId)
			return;
		
		m_TimerWidget.GetRootWidget().RemoveFromHierarchy();
		delete m_TimerWidget;
		
		m_iMyAreaId = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		m_PlayerManager = GetGame().GetPlayerManager();
		
		m_BaseWorld = GetGame().GetWorld();
		
		m_OnPlayerEnterWaitingAreaInvoker.Insert(OnPlayerEnterWaitingAreaInvoked);
		
		GetGame().GetCallqueue().CallLater(AreaCheckUpdate, m_iZoneCheckFrequency, true);
		GetGame().GetCallqueue().CallLater(UpdateTimeleft, 100, true);
	}
}