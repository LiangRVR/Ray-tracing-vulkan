#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"

using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f)
	{
		Material &pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = {1.0f, 0.0f, 1.0f};
		pinkSphere.Roughness = 0.0f;

		Material &blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = {0.2f, 0.3f, 1.0f};
		blueSphere.Roughness = 0.1f;

		Material &orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = {0.8f, 0.5f, 0.2f};
		orangeSphere.Roughness = 0.1f;
		orangeSphere.EmissionColor = orangeSphere.Albedo;
		orangeSphere.EmissionPower = 1.0f;

		{
			Sphere sphere;
			sphere.Position = {0.0f, 0.0f, 0.0f};
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = {2.0f, 0.0f, 0.0f};
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = {0.0f, -101.0f, 0.0f};
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		int optionsChanged = 0;
		ImGui::Begin("Settings");
		ImGui::Text("Last render time: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}
		optionsChanged += ImGui::SliderInt("Bounces", &m_Renderer.m_Bounces, 1, 64);

		ImGui::End();

		ImGui::Begin("Scene");
		if (ImGui::Button("Add Sphere"))
		{
			Sphere sphere;
			sphere.Position = {0.0f, 0.0f, 0.0f};
			sphere.Radius = 0.5f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			Sphere &sphere = m_Scene.Spheres[i];
			ImGui::PushID(static_cast<int>(i));
			ImGui::Text("Sphere %d", static_cast<int>(i + 1));
			optionsChanged += ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			optionsChanged += ImGui::DragFloat("Radius", &sphere.Radius, 0.1f, 0.0f);
			optionsChanged += ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);
			if (ImGui::Button("Delete"))
			{
				m_Scene.Spheres.erase(m_Scene.Spheres.begin() + i);
				i--; // Decrement i to account for the erased element
			}
			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("Materials");
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::Text("Materials %d", static_cast<int>(i));

			Material &material = m_Scene.Materials[i];
			ImGui::PushID(static_cast<int>(i));
			optionsChanged += ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			optionsChanged += ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			optionsChanged += ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
			optionsChanged += ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
			optionsChanged += ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX);

			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		if (optionsChanged)
			m_Renderer.ResetFrameIndex();

		auto image = m_Renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(),
						 {(float)image->GetWidth(), (float)image->GetHeight()},
						 ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Camera m_Camera;
	Renderer m_Renderer;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

	Walnut::Application *app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
							{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		} });
	return app;
}
