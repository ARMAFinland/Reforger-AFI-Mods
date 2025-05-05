class AFI_TimerWidget : SCR_ScriptedWidgetComponent
{
	TextWidget m_wTimerText;
	protected BaseWorld m_BaseWorld;
	protected float m_fNextOccurance
	
	protected BaseWorld GetBaseWorld()
	{
		if (m_BaseWorld == null)
			m_BaseWorld = GetGame().GetWorld();
		
		return m_BaseWorld;
	}
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		m_wTimerText = TextWidget.Cast(w.FindAnyWidget("TimerText"));
	}
	
	void SetTime(float timeToNextOccurance)
	{
		m_fNextOccurance = timeToNextOccurance + GetBaseWorld().GetWorldTime();
		
		GetGame().GetCallqueue().CallLater(UpdateTime, 1000, false);
	}
	
	protected void UpdateTime()
	{
		float timeToNextOccurance = m_fNextOccurance - GetBaseWorld().GetWorldTime();
		
		if (timeToNextOccurance > 0 ) {
			float timeSeconds = timeToNextOccurance / 1000;
			int seconds = Math.Mod(timeSeconds, 60);
			int minutes = timeSeconds / 60;
			m_wTimerText.SetTextFormat("%1:%2", minutes.ToString(2), seconds.ToString(2));
			
			GetGame().GetCallqueue().CallLater(UpdateTime, 1000, false);
		}
	}
}