class AFI_TeleportWaitingAreaInfo
{
	protected ref array<int> m_playerIds = new array<int>();
	
	protected float m_iNextTeleportTime = 0;
	
	protected int m_iAreaId = 0;
	
	//------------------------------------------------------------------------------------------------
	array<int> GetPlayerIds()
	{
		return m_playerIds;
	}
	
	//------------------------------------------------------------------------------------------------
	void InsertPlayerId(int id)
	{
		if (m_playerIds.Contains(id))
			return;
		
		m_playerIds.Insert(id);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetPlayerIds()
	{
		m_playerIds = new array<int>();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetNextTeleportTime()
	{
		return m_iNextTeleportTime;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetNextTeleportTime(float nextTeleportTime)
	{
		m_iNextTeleportTime = nextTeleportTime;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAreaId()
	{
		return m_iAreaId;
	}
	
	//------------------------------------------------------------------------------------------------
	void AFI_TeleportWaitingAreaInfo(int areaId)
	{
		m_iAreaId = areaId;
	}
}