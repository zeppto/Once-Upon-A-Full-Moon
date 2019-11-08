#ifndef LEVEL_HARD_CODED_HPP
#define LEVEL_HARD_CODED_HPP

namespace Level
{
	glm::vec3 Room(bool up, bool down, bool right, bool left, std::string texture = "brown_mud_diffuse", int rotation = 0, int entrensSide = 3, bool onlyPos = false);
	glm::vec3 BorderTrees(int treeNr);
}


#endif

