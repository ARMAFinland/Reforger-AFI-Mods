modded class PS_PolyZoneTrigger
{
	[Attribute("")]
	bool m_bDisableForAircraft;
	
	//------------------------------------------------------------------------------------------------
	override bool ScriptedEntityFilterForQuery(IEntity ent)
	{
		bool result = super.ScriptedEntityFilterForQuery(ent);
		
		if (!result && m_bDisableForAircraft)
		{
			// If player is in vehicle, ignore
			SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(ent.FindComponent(SCR_CompartmentAccessComponent));
			if (compartmentAccess)
			{
				IEntity vehicle = compartmentAccess.GetVehicle();
				if (vehicle)
				{
					SCR_VehiclePerceivableComponent vehiclePerceivableComponent = SCR_VehiclePerceivableComponent.Cast(vehicle.FindComponent(SCR_VehiclePerceivableComponent));
					if (vehiclePerceivableComponent)
					{
						return vehiclePerceivableComponent.GetUnitType() == EAIUnitType.UnitType_Aircraft;
					}
				}
			}
		}
		
		return result;
	}
}