#include "ZHMPrimitives.h"
#include "ZHMTypeInfo.h"

void ZObjectRef::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZObjectRef*>(p_Object);
	
	if (s_Object->m_pTypeID == nullptr)
	{
		p_Stream << "null";
		return;
	}

	p_Stream << "{" << JsonStr("$type") << ":" << JsonStr(s_Object->m_pTypeID->TypeName()) << "," << JsonStr("$val") << ":";

	s_Object->m_pTypeID->WriteJson(s_Object->m_pData, p_Stream);

	p_Stream << "}";
}

void ZObjectRef::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZObjectRef*>(p_Object);
	
	if (s_Object->m_pTypeID == nullptr)
	{
		p_Stream << "null";
		return;
	}

	return s_Object->m_pTypeID->WriteSimpleJson(s_Object->m_pData, p_Stream);
}

void ZHMTypeInfo::RegisterPrimitiveTypes()
{
	new ZHMTypeInfo("int8",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << static_cast<int>(*static_cast<int8_t*>(p_Data));
		});

	new ZHMTypeInfo("uint8",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << static_cast<int>(*static_cast<uint8_t*>(p_Data));
		});

	new ZHMTypeInfo("int16",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<int16_t*>(p_Data);
		});

	new ZHMTypeInfo("uint16",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<uint16_t*>(p_Data);
		});

	new ZHMTypeInfo("int32",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<int32_t*>(p_Data);
		});

	new ZHMTypeInfo("uint32",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<uint32_t*>(p_Data);
		});

	new ZHMTypeInfo("int64",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<int64_t*>(p_Data);
		});

	new ZHMTypeInfo("uint64",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<uint64_t*>(p_Data);
		});

	new ZHMTypeInfo("float32",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<float*>(p_Data);
		});

	new ZHMTypeInfo("float64",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<double*>(p_Data);
		});

	new ZHMTypeInfo("bool",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << *static_cast<bool*>(p_Data);
		});

	new ZHMTypeInfo("ZString",
		[](void* p_Data, std::ostream& p_Stream)
		{
			p_Stream << JsonStr(*static_cast<ZString*>(p_Data));
		});
}
