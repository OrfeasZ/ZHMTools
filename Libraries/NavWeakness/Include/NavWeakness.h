#pragma once

#if defined(_MSC_VER)
#	define NAVWEAKNESS_EXPORT __declspec(dllexport)
#	define NAVWEAKNESS_IMPORT __declspec(dllimport)
#else
#	define NAVWEAKNESS_EXPORT __attribute__((visibility("default")))
#	define NAVWEAKNESS_IMPORT
#endif

#if NAVWEAKNESS_EXPORTS
#	define NAVWEAKNESS_API NAVWEAKNESS_EXPORT
#else
#	define NAVWEAKNESS_API NAVWEAKNESS_IMPORT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	NAVWEAKNESS_API void OutputNavMesh_HUMAN(const char* p_NavMeshPath, bool b_SourceIsJson);
	NAVWEAKNESS_API void OutputNavMesh_VIEWER(const char* p_NavMeshPath, bool b_SourceIsJson);
	NAVWEAKNESS_API void OutputNavMesh_NAVP(const char* p_NavMeshPath, const char* p_NavMeshOutputPath, bool p_SourceIsJson);
	NAVWEAKNESS_API void OutputNavMesh_JSON(const char* p_NavMeshPath, const char* p_NavMeshOutputPath, bool p_SourceIsJson);

#ifdef __cplusplus
}
#endif
