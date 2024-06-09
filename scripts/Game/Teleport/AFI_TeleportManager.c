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
	
	protected ref map<AFI_TeleportWaitingAreaEntity, ref array<IEntity>> m_PlayersWaitingInZones = new map<AFI_TeleportWaitingAreaEntity, ref array<IEntity>>();
	
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
		if (!m_PlayersWaitingInZones.Contains(area))
			m_PlayersWaitingInZones.Set(area, {});
		
		array<IEntity> playersWaiting = m_PlayersWaitingInZones.Get(area);
		
		if (playersWaiting.Contains(player))
			return;
		
		playersWaiting.Insert(player);
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
		
		foreach(AFI_TeleportWaitingAreaEntity area, array<IEntity> playersWaiting : m_PlayersWaitingInZones)
		{
			areaPositionVector = area.GetOrigin();
			
			foreach (IEntity waitingPlayer : playersWaiting)
			{
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
		
		array<IEntity> playersToTeleport = m_PlayersWaitingInZones.Get(area);
		
		vector newPosition;
		int i = 0;
		foreach (IEntity player : playersToTeleport)
		{
			vector offsetVector = GetOffsetVectorFromIndex(i++);		
			newPosition = targetAreaOrigin + offsetVector;
			
			player.SetOrigin(newPosition);
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
		
		m_OnPlayerEnterWaitingAreaInvoker.Insert(OnPlayerEnterWaitingAreaInvoked);
		
		GetGame().GetCallqueue().CallLater(AreaCheckUpdate, m_iZoneCheckFrequency, true);
	}
}