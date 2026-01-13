#pragma once

#include <fstream>
#include <map>
#include <unordered_map>
#include <set>
#include <filesystem>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

class ZTypeRegistry;
class STypeID;
struct IEnumType;

class CodeGen
{
public:
	void Generate(ZTypeRegistry* p_Registry, const std::filesystem::path& p_OutputPath);

private:
	struct TreeNode
	{
		std::string Name;
		enum class ENodeType { Namespace, Type } Type = ENodeType::Namespace;
		STypeID* TypeData = nullptr;
		bool ShouldSkip = false;
		std::shared_ptr<TreeNode> Parent;
		std::map<std::string, std::shared_ptr<TreeNode>> Children;
		std::vector<std::shared_ptr<TreeNode>> SortedChildren;
		std::unordered_set<std::string> Dependencies;
		bool ResourceLibType = false;

		std::string FullName() const {
			if (Parent == nullptr || Parent->Name.empty())
				return Name;

			return Parent->FullName() + "::" + Name;
		}

		std::string TypeName() const {
			if (Parent == nullptr || Parent->Name.empty())
				return Name;

			return Parent->TypeName() + "." + Name;
		}

		std::unordered_set<std::string> AllDependencies() const {
			std::unordered_set<std::string> s_Dependencies = Dependencies;
			for (const auto& s_Child : Children)
				for (const auto& s_Dependency : s_Child.second->AllDependencies())
					s_Dependencies.insert(s_Dependency);
			return s_Dependencies;
		}
	};

	void CollectAllRttiTypes();
	void BuildTypeTree(ZTypeRegistry* p_Registry);
	void SortTypeTree(const std::shared_ptr<TreeNode>& p_Node, std::unordered_set<std::shared_ptr<TreeNode>>& p_Visited);
	void PrintTypeTree(const std::shared_ptr<TreeNode>& p_Node, int p_Depth = 0);
	std::pair<std::unordered_set<std::string>, bool> CollectDependencies(STypeID* p_Type);

	enum class EOutputTarget
	{
		Both,
		SdkOnly,
		RlOnly
	};

	void GenerateCode(const std::shared_ptr<TreeNode>& p_Node, const std::string& p_Indent = "", EOutputTarget p_Target = EOutputTarget::Both);
	void GenerateDummyClass(const std::shared_ptr<TreeNode>& p_Node, const std::string& p_Indent, EOutputTarget p_Target);
	void GenerateRlClassHeader(const std::shared_ptr<TreeNode>& p_Node, const std::string& p_Indent);
	void GenerateRlClassSource(const std::shared_ptr<TreeNode>& p_Node);
	void GenerateEnum(const std::shared_ptr<TreeNode>& p_Node, const std::string& p_Indent, std::ofstream& p_Stream);
	void GenerateSdkClass(const std::shared_ptr<TreeNode>& p_Node, const std::string& p_Indent);
	void GeneratePropertyNamesFiles();
	void GenerateEnumsFiles();
	std::string DemangleRTTIName(const std::string& p_MangledName);
	static void WriteFileHeader(std::ostream& p_Stream);

private:
	std::set<std::string> m_PropertyNames;
	std::map<std::string, std::map<int, std::string>> m_Enums;

	std::ofstream m_ReflectiveClassesHeaderFile;
	std::ofstream m_ReflectiveClassesSourceFile;

	std::ofstream m_PropertyNamesHeaderFile;
	std::ofstream m_PropertyNamesSourceFile;
	std::ofstream m_PropertyNamesTextFile;

	std::ofstream m_EnumsHeaderFile;
	std::ofstream m_EnumsSourceFile;

	std::ofstream m_SDKHeader;

	std::unordered_map<std::string, struct VTableMsvc*> m_RttiByTypeName;
	std::unordered_set<std::string> m_RttiTypes;
	std::shared_ptr<TreeNode> m_TypeTreeRoot;
	std::unordered_map<std::string, std::shared_ptr<TreeNode>> m_TypeNodesByName;
	std::unordered_map<std::string, STypeID*> m_TypeDataByName;
};
