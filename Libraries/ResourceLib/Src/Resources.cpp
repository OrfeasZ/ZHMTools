#include "Resources.h"

#include "ResourceConverterImpl.h"
#include "ResourceGeneratorImpl.h"

#include <ZHM/ZHMCustomTypes.h>
#include <ZHM/ZHMCustomProperties.h>

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMGen.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMGen.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMGen.h>
#endif

#define REGISTER_RESOURCE(ResourceName, ResourceType) { #ResourceName, Resource(CreateResourceConverter<ResourceType>(), CreateResourceGenerator<ResourceType>()) },

// Register all supported resource types here.
std::unordered_map<std::string, Resource> g_Resources = {
#if ZHM_TARGET == 2016 
	REGISTER_RESOURCE(TEMP, STemplateEntity)
#else
	REGISTER_RESOURCE(TEMP, STemplateEntityFactory)
#endif
	
	REGISTER_RESOURCE(TBLU, STemplateEntityBlueprint)
	REGISTER_RESOURCE(AIRG, SReasoningGrid)
	//REGISTER_RESOURCE(ASVA, SPackedAnimSetEntry)
	REGISTER_RESOURCE(ATMD, ZAMDTake)
	REGISTER_RESOURCE(CBLU, SCppEntityBlueprint)
	REGISTER_RESOURCE(CPPT, SCppEntity)
	REGISTER_RESOURCE(CRMD, SCrowdMapData)
	REGISTER_RESOURCE(DSWB, SAudioSwitchBlueprintData)
	
#if ZHM_TARGET != 2016
	REGISTER_RESOURCE(ECPB, SExtendedCppEntityBlueprint)
#endif
	
	REGISTER_RESOURCE(GFXF, SScaleformGFxResource)
	REGISTER_RESOURCE(GIDX, SGlobalResourceIndex)
	REGISTER_RESOURCE(VIDB, SVideoDatabaseData)
	REGISTER_RESOURCE(WSGB, SAudioSwitchBlueprintData)
};
