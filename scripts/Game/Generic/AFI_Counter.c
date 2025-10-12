class AFI_CounterClass : ScriptComponentClass {}

class AFI_Counter : ScriptComponent
{
	[RplProp()]
	private int m_iCounter = 0;
	
	//------------------------------------------------------------------------------------------------
	int GetCount()
	{
		return m_iCounter;
	}
	
	//------------------------------------------------------------------------------------------------
	void IncreaseByOne()
	{
		m_iCounter = m_iCounter + 1;
		
		if (m_iCounter >= 5)
        	m_iCounter = 0;
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		writer.Write(m_iCounter, 3);

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		if (!reader.Read(m_iCounter, 3))
			return false;
		
		return true;
	}
}