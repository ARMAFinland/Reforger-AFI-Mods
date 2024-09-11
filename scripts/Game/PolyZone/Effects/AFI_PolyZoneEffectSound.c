[BaseContainerProps()]
class AFI_PolyZoneEffectSound : PS_PolyZoneEffect
{
	protected bool m_bIsActive = false;
	
	override void OnActivate(PS_PolyZoneEffectHandler handler, IEntity ent)
	{	
		m_bIsActive = true;
		PlaySound(ent);
	}
	
	override void OnDeactivate(PS_PolyZoneEffectHandler handler, IEntity ent)
	{
		m_bIsActive = false;
	}
	
	override PS_PolyZoneEffect CreateCopyObject()
	{
		return new AFI_PolyZoneEffectSound();
	}
	
	override void CopyFields(PS_PolyZoneEffect effect)
	{

	}
	
	override PS_PolyZoneEffect Copy()
	{
		PS_PolyZoneEffect copy = CreateCopyObject();
		CopyFields(copy);
		return copy;	
	}
	
	protected void PlaySound(IEntity ent)
	{
		if (!m_bIsActive)
			return;
		
		SCR_CommunicationSoundComponent soundComponent = SCR_CommunicationSoundComponent.Cast(ent.FindComponent(SCR_CommunicationSoundComponent));		
		soundComponent.SoundEventPriority("SOUND_CP_MOVEMENT_RETREAT", 50);
		
		GetGame().GetCallqueue().CallLater(PlaySound, 5000, false, ent);
	}
}