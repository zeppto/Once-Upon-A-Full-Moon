#ifndef COLLISIONMAP_HPP
#define COLLISIONMAP_HPP
#include"..\..\Core\Camera\Camera.hpp"

namespace Frosty
{

	class CollisionMap
	{

		//var
	public:


		//var
	private:

		static CollisionMap* s_Instance;
		std::shared_ptr <Camera> m_Camera;

		const uint16_t MAP_WITDH_PIXELS = 1920;
		const uint16_t MAP_HEIGHT_PIXELS = 1080;

		friend class Application;
		//Func
	public:
		static CollisionMap* Get();
		virtual ~CollisionMap() {};

		void Initiate();

		std::shared_ptr<Camera>& GetCamera();



	//Func
private:






};


}

#endif