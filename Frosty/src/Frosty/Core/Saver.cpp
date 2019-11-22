#include <fypch.hpp>
#include "Saver.hpp"
#include "Frosty/Core/Application.hpp"

#include <PugiXML/pugixml.hpp>

namespace Frosty
{
	void Saver::SaveAll()
	{

		// Create neccessary files (XML)
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("");

		FY_CORE_ASSERT(result, "Error loading file {0}: {1}", "Weapons.xml", result.description());


		// Get all nodes
		pugi::xml_node weapons = doc.child("weapons");

		// Retrieve and save info
		size_t amount = std::distance(weapons.children().begin(), weapons.children().end());

		for (pugi::xml_node w = weapons.child("weapon"); w; w = w.next_sibling("weapon"))
		{


		}
	}
}