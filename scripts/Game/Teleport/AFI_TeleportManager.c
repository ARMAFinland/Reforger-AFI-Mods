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
	
	protected ref map<AFI_TeleportWaitingAreaEntity, ref array<int>> m_PlayersWaitingInZones = new map<AFI_TeleportWaitingAreaEntity, ref array<int>>();
	
	protected PlayerManager m_PlayerManager;
	
	protected const int m_iZoneCheckFrequency = 50;
	
	protected int m_iGridWidth = 5;
	
	//------------------------------------------------------------------------------------------------
	void RegisterWaitingArea(AFI_TeleportWaitingAreaEntity area, int frequency)
	{
		if (!Replication.IsServer())
			return;
		
		if (!m_PlayersWaitingInZones.Contains(area))
			m_PlayersWaitingInZones.Set(area, {});
		
		GetGame().GetCallqueue().CallLater(HandleTeleportForArea, frequency, true, area);
	}
	
	//------------------------------------------------------------------------------------------------
	OnPlayerEnterWaitingAreaInvoker GetOnPlayerEnterWaitingArea()
	{
		return m_OnPlayerEnterWaitingAreaInvoker;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerEnterWaitingAreaInvoked(IEntity player, AFI_TeleportWaitingAreaEntity area)
	{
		int playerId = m_PlayerManager.GetPlayerIdFromControlledEntity(player);
		
		if (!m_PlayersWaitingInZones.Contains(area))
			m_PlayersWaitingInZones.Set(area, {});
		
		array<int> playersWaiting = m_PlayersWaitingInZones.Get(area);
		
		if (playersWaiting.Contains(playerId))
			return;
		
		playersWaiting.Insert(playerId);
		
		DisablePlayerWeapons(player);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DisablePlayerWeapons(IEntity player)
	{
		EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(player.FindComponent(EventHandlerManagerComponent));
		if (!eventHandler) return;
		
		eventHandler.RegisterScriptHandler("OnProjectileShot", this, OnWeaponFired);
		eventHandler.RegisterScriptHandler("OnGrenadeThrown", this, OnGrenadeThrown);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void EnablePlayerWeapons(IEntity player)
	{
		EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(player.FindComponent(EventHandlerManagerComponent));
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
		if (!Replication.IsServer())
			return;
		
		vector areaPositionVector;
		vector playerPositionVector;
		float distanceSqrXZ;
		float areaRadius;
		
		foreach(AFI_TeleportWaitingAreaEntity area, array<int> playersWaiting : m_PlayersWaitingInZones)
		{
			areaPositionVector = area.GetOrigin();
			
			foreach (int waitingPlayerId : playersWaiting)
			{
				PlayerController waitingPlayer = m_PlayerManager.GetPlayerController(waitingPlayerId);
				
				playerPositionVector = waitingPlayer.GetOrigin();
				
				distanceSqrXZ = vector.DistanceSqXZ(playerPositionVector, areaPositionVector);
				areaRadius = area.GetZoneRadiusSq();
				
				if (distanceSqrXZ > areaRadius)
				{
					waitingPlayer.SetOrigin(areaPositionVector);
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
		
		array<int> playersToTeleport = m_PlayersWaitingInZones.Get(area);
		
		vector newPosition;
		int i = 0;
		foreach (int playerId : playersToTeleport)
		{
			PlayerController player = m_PlayerManager.GetPlayerController(playerId);
			
			vector offsetVector = GetOffsetVectorFromIndex(i++);		
			newPosition = targetAreaOrigin + offsetVector;
			
			IEntity playerControlledEntity = player.GetControlledEntity();
			
			EnablePlayerWeapons(playerControlledEntity);
			
			playerControlledEntity.SetOrigin(newPosition);
		}
		
		m_PlayersWaitingInZones.Set(area, {});
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
	override void OnPostInit(IEntity owner)
	{
		if (!Replication.IsServer())
			return;
		
		m_PlayerManager = GetGame().GetPlayerManager();
		
		m_OnPlayerEnterWaitingAreaInvoker.Insert(OnPlayerEnterWaitingAreaInvoked);
		
		GetGame().GetCallqueue().CallLater(AreaCheckUpdate, m_iZoneCheckFrequency, true);
	}
}