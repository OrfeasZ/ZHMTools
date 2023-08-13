#include <cstdint>

#include "ResourceConverter.h"
#include "ResourceGenerator.h"
#include "ResourceLibCommon.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

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

	/**
	 * Get a binary -> json converter for the given resource type (eg. TEMP).
	 * If a converter for this resource type doesn't exist, this function will return [nullptr].
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType);

	/**
	 * Get a json -> binary generator for the given resource type (eg. TEMP).
	 * If a generator for this resource type doesn't exist, this function will return [nullptr].
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType);

	/**
	 * Get a list of resource types that this library supports converting between json and binary forms.
	 * After using the result of this function it must be cleaned up by passing it to the
	 * [HMX_FreeSupportedResourceTypes] function.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)();

	/**
	 * Clean up the supported resource types array. The [ResourceTypesArray] becomes invalid after a
	 * call to this function, and attempting to use it results in undefined behavior.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array);

	/**
	 * Checks if this library supports converting between the json and binary forms of the specified resource types
	 * and returns [true] if it does, or [false] otherwise.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType);

	/**
	 * Converts the provided binary game structure to its JSON representation in an in-memory [JsonString].
	 * After using it, the [JsonString] must be cleaned up by passing it to the [FreeJsonString] function.
	 *
	 * If the operation fails, this function will return [nullptr].
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API JsonString* ZHM_TARGET_FUNC(GameStructToJson)(const char* p_StructureType, const void* p_Structure, size_t p_Size);

	/**
	 * Converts the provided JSON representation of a game structure, to its native in-memory representation.
	 * The [p_TargetMemory] variable must point to a writable buffer of enough size to fit the structure of the
	 * provided [p_StructureType] type, and [p_TargetMemorySize] must be set to the size of that buffer.
	 *
	 * If the conversion fails, this function will return [false], or [true] otherwise.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API bool ZHM_TARGET_FUNC(JsonToGameStruct)(const char* p_StructureType, const char* p_JsonStr, size_t p_JsonStrLength, void* p_TargetMemory, size_t p_TargetMemorySize);

	/**
	 * Clean up an in-memory json string. The [JsonString] becomes invalid after a call to
	 * this function, and attempting to use it results in undefined behavior.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API void ZHM_TARGET_FUNC(FreeJsonString)(JsonString* p_JsonString);

	/**
	 * Try to get the name of a property from its CRC32 id.
	 * If the property name cannot be found, the resulting [StringView] will contain
	 * a [nullptr] data pointer and a size of 0.
	 */
	EMSCRIPTEN_KEEPALIVE RESOURCELIB_API StringView ZHM_TARGET_FUNC(GetPropertyName)(uint32_t p_PropertyId);

#ifdef __cplusplus
}
#endif

#include "Embind.h"