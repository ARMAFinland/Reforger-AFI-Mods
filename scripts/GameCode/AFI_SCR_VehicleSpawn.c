modded class SCR_VehicleSpawn : SCR_BasePrefabSpawner
{
	override void OnDestroyed(IEntity newEnt)
	{
		EventHandlerManagerComponent ehmc = EventHandlerManagerComponent.Cast (spawnedVehicle.FindComponent(EventHandlerManagerComponent));
		if (ehmc)
		{
			ehmc.RemoveScriptHandler("OnDestroyed", this, OnDestroyed);
		}
		
		spawnedVehicle = null;
	}
}