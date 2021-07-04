#pragma once

#include <fstream>
#include <map>
#include <set>
#include <filesystem>
#include <unordered_set>

class ZTypeRegistry;
class STypeID;

struct GeneratedType
{
	std::string TypeName;
	std::set<std::string> Dependencies;
	std::string Source;
	std::string Header;
	bool ShouldSkip;
};

class CodeGen
{
public:
	void Generate(ZTypeRegistry* p_Registry, const std::filesystem::path& p_OutputPath);

private:
	void GenerateClass(STypeID* p_Type);
	void GenerateEnum(STypeID* p_Type);

	void GenerateReflectiveClass(STypeID* p_Type);
	void GenerateReflectiveEnum(STypeID* p_Type);

	void GenerateReflectiveClasses();
	void GeneratePropertyNamesFiles();
	void GenerateEnumsFiles();

	void Visit(GeneratedType* p_Type, std::unordered_set<GeneratedType*>& p_Visited, std::vector<GeneratedType*>& p_Sorted);

private:
	std::set<std::string> m_PropertyNames;
	std::map<std::string, std::map<int, std::string>> m_Enums;

	std::map<std::string, GeneratedType*> m_ReflectiveClasses;

	std::ofstream m_ClassesFile;
	std::ofstream m_EnumsFile;

	std::ofstream m_SDKEnumsHeader;

	std::ofstream m_ReflectiveClassesHeaderFile;
	std::ofstream m_ReflectiveClassesSourceFile;

	std::ofstream m_PropertyNamesHeaderFile;
	std::ofstream m_PropertyNamesSourceFile;

	std::ofstream m_EnumsHeaderFile;
	std::ofstream m_EnumsSourceFile;
};
