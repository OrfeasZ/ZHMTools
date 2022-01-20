#pragma once

#if __EMSCRIPTEN__

#include <emscripten/bind.h>

using namespace emscripten;

#define ZHM_MODULE_NAME_FOR_TARGET(TARGET) HM ## _ ## TARGET
#define ZHM_MODULE_NAME ZHM_MODULE_NAME_FOR_TARGET(ZHM_TARGET)

EMSCRIPTEN_BINDINGS(ZHM_MODULE_NAME)
{
	function("GetConverterForResource", &ZHM_TARGET_FUNC(GetConverterForResource), allow_raw_pointers());
	function("GetGeneratorForResource", &ZHM_TARGET_FUNC(GetGeneratorForResource), allow_raw_pointers());
	function("GetSupportedResourceTypes", &ZHM_TARGET_FUNC(GetSupportedResourceTypes), allow_raw_pointers());
	function("FreeSupportedResourceTypes", &ZHM_TARGET_FUNC(FreeSupportedResourceTypes), allow_raw_pointers());
	function("IsResourceTypeSupported", &ZHM_TARGET_FUNC(IsResourceTypeSupported), allow_raw_pointers());
}

#endif