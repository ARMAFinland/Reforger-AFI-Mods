class AFI_CaptureGameManagerClass : ScriptComponentClass { }

class AFI_CaptureGameManager : ScriptComponent
{
	protected ref map<string, ref AFI_CapturePointData> m_pointsData = new map<string, ref AFI_CapturePointData>();
	protected ref map<string, AFI_CapturePoint> m_points = new map<string, AFI_CapturePoint>;

	//------------------------------------------------------------------------------------------------
	static AFI_CaptureGameManager m_instance;
	static AFI_CaptureGameManager GetInstance()
	{
		if (!m_instance)
			m_instance = AFI_CaptureGameManager.Cast(GetGame().GetGameMode().FindComponent(AFI_CaptureGameManager));
		
		return m_instance;
	}
		
	//------------------------------------------------------------------------------------------------
	void RegisterCapturePoint(string name, AFI_CapturePoint capturePoint)
	{		
		m_pointsData.Insert(name, new AFI_CapturePointData(capturePoint.GetCapturePointName()));
		m_points.Insert(name, capturePoint);
		
		//Replication.BumpMe();
	}
	
	AFI_CapturePointData GetCapturePointData(string name)
	{
		return m_pointsData.Get(name);
	}
	
	AFI_CapturePoint GetCapturePoint(string name)
	{
		return m_points.Get(name);
	}
}

class AFI_CapturePointData
{
	string Name;
	string ControlFaction;
	
	void AFI_CapturePointData(string name)
	{
		Name = name;
	}
	
	static bool Extract(AFI_CapturePointData instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.SerializeString(instance.Name);
		snapshot.SerializeString(instance.ControlFaction);
		
		return true;
	}
	
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, AFI_CapturePointData instance)
	{
		snapshot.SerializeString(instance.Name);
		snapshot.SerializeString(instance.ControlFaction);
		
		return true;
	}
	
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		snapshot.EncodeString(packet);
		snapshot.EncodeString(packet);
	}
	
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		snapshot.DecodeString(packet);
		snapshot.DecodeString(packet);
		
		return true;
	}
	
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
	{
		return lhs.CompareStringSnapshots(lhs) // Name
			&& lhs.CompareStringSnapshots(lhs); //ControlFaction
	}
	
	static bool PropCompare(AFI_CapturePointData instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		return snapshot.CompareString(instance.Name)
			&& snapshot.CompareString(instance.ControlFaction);
	}
}