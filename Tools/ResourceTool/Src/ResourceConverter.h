#pragma once

#include "Resources.h"

template<typename T>
class ResourceConverter : public IResourceConverter
{
public:
	void WriteJson(void* p_ResourceData, bool p_Simple, std::ostream& p_Stream) override
	{
		auto* s_Resource = static_cast<T*>(p_ResourceData);

		if (p_Simple)
		{
			T::WriteSimpleJson(s_Resource, p_Stream);
			return;
		}

		T::WriteJson(s_Resource, p_Stream);
	}
};

bool ResourceToJson(const std::filesystem::path& p_InputFilePath, const std::filesystem::path& p_OutputFilePath, IResourceConverter* p_Converter, bool p_SimpleOutput);