#ifndef LEVEL_HARD_CODED_HPP
#define LEVEL_HARD_CODED_HPP

namespace Level
{
	void Room(bool up, bool down, bool right, bool left, std::string texture = "Brown Mud Diffuse", int rotation = 0);
	glm::vec3 BorderTrees(int treeNr);
}


#endif

