#include "ResourceLibCommon.h"

#if RESOURCELIB_EXPORTS
#	define RESOURCELIB_API __declspec(dllexport)
#else
#	define RESOURCELIB_API __declspec(dllimport)
#endif

#define ZHM_TARGET_FUNC_FINAL(X, Y) HM ## X ## _ ## Y
#define ZHM_TARGET_FUNC_EVAL(X, Y) ZHM_TARGET_FUNC_FINAL(X, Y)
#define ZHM_TARGET_FUNC(FunctionName) ZHM_TARGET_FUNC_EVAL(ZHM_TARGET, FunctionName)

RESOURCELIB_API IResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType);
RESOURCELIB_API IResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType);
RESOURCELIB_API ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)();
RESOURCELIB_API void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array);
RESOURCELIB_API bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType);

