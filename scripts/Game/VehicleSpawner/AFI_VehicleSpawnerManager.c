[ComponentEditorProps(category: "AFI/Systems/VehicleSpawner", description: "AFI Vechile Spawner Manager")]
class AFI_VehicleSpawnerManagerClass : ScriptComponentClass
{
}

class AFI_VehicleSpawnerManager : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	void RequestVehicleToSpawn(ResourceName prefabToSpawn, string targetArea)
	{
		//Rpc(RpcAsk_VehicleToSpawn, prefabToSpawn, targetArea)
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_VehicleToSpawn(ResourceName prefabToSpawn, string targetArea)
	{		
		
	}
}