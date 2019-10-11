#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include"Frosty/API/Transform.h"
namespace Frosty
{

	class PrefabInstance
	{
	public:		//Variables


	private:	//Variables
		
		uint32_t m_InstanceID = 0;
		static uint32_t s_Unique_ID;
		std::string m_PrefabName;

		Transform m_Transform;

	public:		//Functions
		PrefabInstance(std::string pefabName);
		PrefabInstance(PrefabInstance& other);

		~PrefabInstance();

		Transform* GetTransform();

		void Destroy();

		PrefabInstance& operator = (PrefabInstance& other);
		bool operator == (PrefabInstance& other);
	private:	//Functions


	};
}
#endif // !GAMEOBJECT_H
