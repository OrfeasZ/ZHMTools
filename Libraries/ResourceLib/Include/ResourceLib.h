#include "ResourceConverter.h"
#include "ResourceGenerator.h"
#include "ResourceLibCommon.h"

#if defined(_MSC_VER)
#	define RESOURCELIB_EXPORT __declspec(dllexport)
#	define RESOURCELIB_IMPORT __declspec(dllimport)
#else
#	define RESOURCELIB_EXPORT __attribute__((visibility("default")))
#	define RESOURCELIB_IMPORT
#endif

#if RESOURCELIB_EXPORTS
#	define RESOURCELIB_API RESOURCELIB_EXPORT
#else
#	define RESOURCELIB_API RESOURCELIB_IMPORT
#endif

#define ZHM_TARGET_FUNC_FINAL(X, Y) HM ## X ## _ ## Y
#define ZHM_TARGET_FUNC_EVAL(X, Y) ZHM_TARGET_FUNC_FINAL(X, Y)
#define ZHM_TARGET_FUNC(FunctionName) ZHM_TARGET_FUNC_EVAL(ZHM_TARGET, FunctionName)

#ifdef __cplusplus
extern "C"
{
#endif

	RESOURCELIB_API ResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType);
	RESOURCELIB_API ResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType);
	RESOURCELIB_API ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)();
	RESOURCELIB_API void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array);
	RESOURCELIB_API bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType);

#ifdef __cplusplus
}
#endif
