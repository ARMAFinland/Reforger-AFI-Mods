typedef func AFI_OnQueryFinished_WithResult;
void AFI_OnQueryFinished_WithResult(bool result);

class AFI_CapturePointTriggerClass: SCR_FactionControlTriggerEntityClass { }

class AFI_CapturePointTrigger: SCR_FactionControlTriggerEntity
{
	protected ref ScriptInvokerBase<AFI_OnQueryFinished_WithResult> Event_OnQueryFinished_WithResult;
	
	//------------------------------------------------------------------------------------------------
	ScriptInvokerBase<AFI_OnQueryFinished_WithResult> GetQueryFinishedWithResult()
	{
		if (Event_OnQueryFinished_WithResult == null)
			Event_OnQueryFinished_WithResult = new ScriptInvokerBase<AFI_OnQueryFinished_WithResult>();
		
		return Event_OnQueryFinished_WithResult;
	}
	
	
	//------------------------------------------------------------------------------------------------
	override protected void OnQueryFinished(bool bIsEmpty)
	{
		super.OnQueryFinished(bIsEmpty);
		
		if (!Replication.IsServer())
			return;
		
		if (Event_OnQueryFinished_WithResult != null)
		{
			Event_OnQueryFinished_WithResult.Invoke(m_bResult);
		}
	}
}