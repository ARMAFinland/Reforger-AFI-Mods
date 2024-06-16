class AFI_TimerWidget : SCR_ScriptedWidgetComponent
{
	TextWidget m_wTimerText;
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		m_wTimerText = TextWidget.Cast(w.FindAnyWidget("TimerText"));
	}
	
	void SetTime(float timeToNextOccurance)
	{
		float timeSeconds = timeToNextOccurance / 1000;
		int seconds = Math.Mod(timeSeconds, 60);
		int minutes = timeSeconds / 60;
		m_wTimerText.SetTextFormat("%1:%2", minutes.ToString(2), seconds.ToString(2));
	}
}