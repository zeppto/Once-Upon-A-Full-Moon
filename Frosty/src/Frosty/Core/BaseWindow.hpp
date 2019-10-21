#ifndef BASE_WINDOW_HPP
#define BASE_WINDOW_HPP

#include <fypch.hpp>
#include "Frosty/Core/Base.hpp"

namespace Frosty
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		unsigned int PositionX;
		unsigned int PositionY;

		WindowProps(const std::string& title = "Frosty Engine", unsigned int width = 1280, unsigned int height = 720, unsigned int posx = 100, unsigned int posy = 200)
			: Title(title), Width(width), Height(height), PositionX(posx), PositionY(posy) { }
	};

	class BaseWindow
	{
	public:
		virtual ~BaseWindow() = default;

		virtual std::string GetTitle() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetPositionX() const = 0;
		virtual unsigned int GetPositionY() const = 0;
		virtual std::pair<unsigned int, unsigned int> GetPosition() const = 0;
		virtual const glm::vec4& GetViewport() const = 0;

		virtual void ActivateEditorMode() = 0;
		virtual void ActivateGameMode() = 0;

		virtual bool IsMaximized() const = 0;
		virtual void Maximize() = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() = 0;

		virtual void OnEvent(BaseEvent& e) = 0;
		virtual void OnUpdate() = 0;

		virtual void* GetNativeWindow() const = 0;

		static BaseWindow* Create();

	private:
		virtual void Init(const WindowProps& props = WindowProps()) = 0;

		virtual void Shutdown() = 0;

		friend class Application;
	};
}

#endif // !BASE_WINDOW_HPP
