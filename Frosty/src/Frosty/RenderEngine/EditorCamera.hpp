#ifndef EDITOR_CAMERA_HPP
#define EDITOR_CAMERA_HPP

namespace Frosty
{
	struct EditorCameraProps
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		float FieldOfView{ 0.0f };
		glm::vec2 ClippingPlanes{ 0.0f };

		EditorCameraProps(const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), float fov = 60.0f, const glm::vec2& nearFar = glm::vec2(0.03f, 1000.0f))
			: Position(pos), Rotation(rotation), FieldOfView(fov), ClippingPlanes(nearFar) { }
	};

	class EditorCamera
	{
	public:
		EditorCamera() = default;
		EditorCamera(const EditorCamera& org) { FY_CORE_ASSERT(false, "Copy constructor in EditorCamera called."); }
		virtual ~EditorCamera() = default;

		// Operators
		EditorCamera& operator=(const EditorCamera& org) { FY_CORE_ASSERT(false, "Assignment operator in EditorCamera called."); return *this; }

		void Init(const EditorCameraProps& props = EditorCameraProps());
		void OnUpdate();

		bool* ActiveStatus() { return &m_Active; }
		bool IsActive() const { return m_Active; }

		//void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		//void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateMatrix(); }

		const glm::vec3& GetRotation() const { return m_Rotation; }
		//void SetRotation(float rotation) { m_Rotation = rotation; RecalculateMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		void Translate(const glm::vec3& offset);

	private:
		void RecalculateMatrix();

	private:
		bool m_Active{ true };

		glm::mat4 m_ProjectionMatrix{ 0.0f };
		glm::mat4 m_ViewMatrix{ 0.0f };
		glm::mat4 m_ViewProjectionMatrix{ 0.0f };

		glm::vec3 m_Position{ 0.0f };
		glm::vec3 m_Rotation{ 0.0f, 0.0f, 0.0f };		// Yaw, Pitch, Roll
		glm::vec3 m_Front{ 0.0f };

		// Settings
		bool m_ResetMouse{ true };
		float m_LastX{ 0.0f };
		float m_LastY{ 0.0f };
		float m_TranslationSpeed{ 10.0f };
		float m_RotationSpeed{ 0.5f };

	};
}

#endif // !EDITOR_CAMERA_HPP
