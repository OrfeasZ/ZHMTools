#include <fstream>
#include <filesystem>

#include <Generated/ZHMGen.h>
#include <ZHM/ZHMCustomTypes.h>
#include <Util/BinaryStream.h>

class IResourceConverter
{
public:
	virtual std::string ToJson(void* p_ResourceData, bool p_Simple) = 0;
};

template<typename T>
class ResourceConverter : public IResourceConverter
{
public:
	std::string ToJson(void* p_ResourceData, bool p_Simple) override
	{
		auto* s_Resource = static_cast<T*>(p_ResourceData);

		if (p_Simple)
		{
			const auto s_Json = T::ToSimpleJson(s_Resource);
			return s_Json.dump();
		}

		const auto s_Json = T::ToJson(s_Resource);
		return s_Json.dump();
	}
};

// Supported resource types need to be registered here.
static std::unordered_map<std::string, IResourceConverter*> g_ResourceConverters = {
	{ "TEMP", new ResourceConverter<STemplateEntityFactory>() },
	{ "TBLU", new ResourceConverter<STemplateEntityBlueprint>() },
	//{ "AIRG", new ResourceConverter<SReasoningGrid>() },
	//{ "ASVA", new ResourceConverter<TArray<SPackedAnimSetEntry>>() },
	{ "ATMD", new ResourceConverter<ZAMDTake>() },
	//{ "CBLU", new ResourceConverter<SCppEntityBlueprint>() },
	{ "CPPT", new ResourceConverter<SCppEntity>() },
	//{ "CRMD", new ResourceConverter<SCrowdMapData>() },
	{ "DSWB", new ResourceConverter<SAudioSwitchBlueprintData>() },
	{ "ECPB", new ResourceConverter<SExtendedCppEntityBlueprint>() },
	{ "GFXF", new ResourceConverter<SScaleformGFxResource>() },
	{ "GIDX", new ResourceConverter<SGlobalResourceIndex>() },
	{ "VIDB", new ResourceConverter<SVideoDatabaseData>() },
	{ "WSGB", new ResourceConverter<SAudioSwitchBlueprintData>() },
};

void ProcessRelocations(BinaryStream& p_SegmentStream, BinaryStream& p_ResourceStream)
{
	const auto s_RelocationCount = p_SegmentStream.Read<uint32_t>();

	for (uint32_t i = 0; i < s_RelocationCount; ++i)
	{
		const auto s_RelocationOffset = p_SegmentStream.Read<uint32_t>();

		p_ResourceStream.Seek(s_RelocationOffset);

		const auto s_RelocValue = p_ResourceStream.Read<int64_t>();

		uintptr_t s_FinalValue = 0;

		if (s_RelocValue != -1)
			s_FinalValue = reinterpret_cast<uintptr_t>(p_ResourceStream.Buffer()) + s_RelocValue;

		p_ResourceStream.Seek(s_RelocationOffset);
		p_ResourceStream.Write(s_FinalValue);
	}
}

void ProcessTypeIds(BinaryStream& p_SegmentStream, BinaryStream& p_ResourceStream)
{
	std::unordered_map<uint32_t, uint32_t> s_TypeIdsToPatch;

	const auto s_TypeIdsToPatchCount = p_SegmentStream.Read<uint32_t>();

	for (uint32_t i = 0; i < s_TypeIdsToPatchCount; ++i)
	{
		const auto s_TypeIdOffset = p_SegmentStream.Read<uint32_t>();

		p_ResourceStream.Seek(s_TypeIdOffset);

		const auto s_TypeIdIndex = p_ResourceStream.Read<uint32_t>();

		s_TypeIdsToPatch[s_TypeIdOffset] = s_TypeIdIndex;
	}

	const auto s_TypeIdCount = p_SegmentStream.Read<uint32_t>();

	std::vector<ZHMTypeInfo*> s_Types(s_TypeIdCount);

	for (uint32_t i = 0; i < s_TypeIdCount; ++i)
	{
		//p_SegmentStream.AlignReadTo(4);

		const auto s_Index = p_SegmentStream.Read<uint32_t>();
		const auto s_Unknown = p_SegmentStream.Read<int32_t>();
		const auto s_TypeName = p_SegmentStream.ReadString();
		
		if (s_Unknown != -1)
			printf("[WARNING] Found TypeIdInfo with an unknown value that wasn't -1 (is %d).\n", s_Unknown);

		auto* s_Type = ZHMTypeInfo::GetTypeByName(s_TypeName);

		if (s_Type == nullptr)
			printf("[WARNING] Could not find TypeInfo for type '%s'.\n", s_TypeName.c_str());

		s_Types[s_Index] = s_Type;
	}

	for (auto& s_Pair : s_TypeIdsToPatch)
	{
		const auto* s_Type = s_Types[s_Pair.second];

		p_ResourceStream.Seek(s_Pair.first);
		p_ResourceStream.Write(reinterpret_cast<uintptr_t>(s_Type));
	}
}

void PrintHelp()
{
#if _WIN32
	printf("Usage: ResourceParser.exe <resource-path> <resource-type> [options]\n");
#else
	printf("Usage: ./ResourceParser <resource-path> <resource-type> [options]\n");
#endif
	
	printf("\n");
	printf("resource-type can be one of:\n");

	for (auto& s_Pair : g_ResourceConverters)
		printf("\t%s\n", s_Pair.first.c_str());
	
	printf("\n");
	printf("Options:\n");
	printf("\t--simple\tGenerates simpler JSON output, omitting any type metadata.\n");
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		PrintHelp();
		return 1;
	}

	const std::string s_ResourcePathStr(argv[1]);
	const std::string s_ResourceTypeStr(argv[2]);
	bool s_SimpleJson = argc >= 4 && std::string(argv[3]) == "--simple";

	IResourceConverter* s_ResourceConverter = nullptr;

	auto s_ResourceConverterIt = g_ResourceConverters.find(s_ResourceTypeStr);

	if (s_ResourceConverterIt != g_ResourceConverters.end())
		s_ResourceConverter = s_ResourceConverterIt->second;
	
	if (s_ResourceConverter == nullptr)
	{
		PrintHelp();
		return 1;
	}
	
	auto s_FilePath = std::filesystem::path(s_ResourcePathStr);

	if (!is_regular_file(s_FilePath))
	{
		printf("Could not find the file you specified.\n");
		return 1;
	}

	// Read the entire file to memory.
	const auto s_FileSize = file_size(s_FilePath);
	std::ifstream s_FileStream(s_FilePath, std::ios::in | std::ios::binary);

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	// Parse the resource header.
	BinaryStream s_Stream(s_FileData, s_FileSize);

	// We expect the first 4 bytes to be the magic value "BIN1".
	if (s_Stream.Read<uint32_t>() != '1NIB')
	{
		printf("The file you specified is not a binary resource.\n");
		return 1;
	}

	s_Stream.Skip(1);

	const auto s_Alignment = s_Stream.Read<uint8_t>();
	const auto s_SegmentCount = s_Stream.Read<uint8_t>();

	s_Stream.Skip(1);

	// For some reason this size is encoded in big endian.
	const auto s_DataSize = c_byteswap_ulong(s_Stream.Read<uint32_t>());

	s_Stream.Skip(4);

	void* s_ResourceData = c_aligned_alloc(s_DataSize, s_Alignment);
	s_Stream.ReadBytes(s_ResourceData, s_DataSize);

	// TODO: Fix. This is actually wrong because size could be bigger due to alignment.
	BinaryStream s_ResourceStream(s_ResourceData, s_DataSize);

	// Process segments.
	for (uint8_t i = 0; i < s_SegmentCount; ++i)
	{
		const auto s_SegmentType = s_Stream.Read<uint32_t>();
		const auto s_SegmentSize = s_Stream.Read<uint32_t>();

		switch (s_SegmentType)
		{
		case 0x12EBA5ED:
			ProcessRelocations(s_Stream, s_ResourceStream);
			break;

		case 0x3989BF9F:
			ProcessTypeIds(s_Stream, s_ResourceStream);
			break;

		// TODO: Resource ID segments
		// TODO: Runtime Resource ID and Resource Ptr

		default:
			printf("[WARNING] Found unrecognized segment (%x). Skipping.\n", s_SegmentType);
			s_Stream.Skip(s_SegmentSize);
			break;
		}
	}

	// Everything should be properly reconstructed in memory by now
	// so just cast and convert this type to json.
	std::string s_JsonDump = s_ResourceConverter->ToJson(s_ResourceData, s_SimpleJson);
	
	printf(s_JsonDump.c_str());
	printf("\n");

	free(s_FileData);
	c_aligned_free(s_ResourceData);

	return 0;
}
