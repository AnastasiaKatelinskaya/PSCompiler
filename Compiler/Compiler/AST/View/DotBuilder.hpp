#pragma once

#include <fstream>

class DotBuilder
{
public:
	DotBuilder()
		:m_strm("ast.dot")
	{
		m_strm << "digraph PROGRAM {" << std::endl;
	}

	void AddEdge(size_t from, size_t to, std::string label = "")
	{
		m_strm << '\t' << from << " -> " << to << (label.empty() ? "" : (" [label=\"" + label + "\"]")) << ";" << std::endl;
	}

	size_t AddVertex(std::string label = "")
	{
		return AddVertex(label, VertexType::Normal);
	}

	size_t AddAlertVertex(std::string label = "")
	{
		return AddVertex(label, VertexType::Alert);
	}

	size_t AddWarningVertex(std::string label = "")
	{
		return AddVertex(label, VertexType::Warning);
	}

	~DotBuilder()
	{
		m_strm << "}" << std::endl;
	}

private:
	enum class VertexType
	{
		Normal,
		Warning,
		Alert,
	};

	size_t AddVertex(std::string label, VertexType type)
	{
		const auto fn = [](const std::string& str) {
			return str + (str.empty() ? "" : ",");
		};

		const auto dotLabel = label.empty() ? "" : "label=\"" + label + "\"";
		const auto dotStyling = VertexTypeToStyling(type);
		m_strm << '\t' << m_lastVertexId << " [" << fn(dotLabel) << fn(dotStyling) << "];" << std::endl;
		return m_lastVertexId++;
	}

	std::string VertexTypeToStyling(VertexType type)
	{
		switch (type)
		{
		case VertexType::Normal:
			return "";
		case VertexType::Warning:
			return "fillcolor=yellow, style=filled";
		case VertexType::Alert:
			return "fillcolor=red, style=filled";
		default:
			throw std::exception("Unknown VertexType");
		}
	}

	size_t m_lastVertexId = 0;
	std::ofstream m_strm;
};
