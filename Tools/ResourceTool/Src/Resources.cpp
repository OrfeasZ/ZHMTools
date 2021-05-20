/*
	5/19/2021 - [REDACTED] Manually added resource type TEMPH2 and classes STemplateEntityFactoryH2 and STemplateFactorySubEntityH2 for H2 STemplateEntityFactory (TEMP) file support
*/
#include "Resources.h"

#include "ResourceConverter.h"
#include "ResourceGenerator.h"

#include <Generated/ZHMGen.h>
#include <ZHM/ZHMCustomTypes.h>

// Register all supported resource types here.
std::unordered_map<std::string, Resource> g_Resources = {
	REGISTER_RESOURCE(TEMP, STemplateEntityFactory)
	REGISTER_RESOURCE(TEMPH2, STemplateEntityFactoryH2)
	REGISTER_RESOURCE(TBLU, STemplateEntityBlueprint)
	REGISTER_RESOURCE(AIRG, SReasoningGrid)
	//REGISTER_RESOURCE(ASVA, SPackedAnimSetEntry)
	REGISTER_RESOURCE(ATMD, ZAMDTake)
	REGISTER_RESOURCE(CBLU, SCppEntityBlueprint)
	REGISTER_RESOURCE(CPPT, SCppEntity)
	REGISTER_RESOURCE(CRMD, SCrowdMapData)
	REGISTER_RESOURCE(DSWB, SAudioSwitchBlueprintData)
	REGISTER_RESOURCE(ECPB, SExtendedCppEntityBlueprint)
	REGISTER_RESOURCE(GFXF, SScaleformGFxResource)
	REGISTER_RESOURCE(GIDX, SGlobalResourceIndex)
	REGISTER_RESOURCE(VIDB, SVideoDatabaseData)
	REGISTER_RESOURCE(WSGB, SAudioSwitchBlueprintData)
};
