#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"
#include "Sphere.h"

using namespace Walnut;
using std::make_shared;
using std::shared_ptr;
class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f)
	{
		auto materialGround = make_shared<Lambertian>("Ground");
		materialGround->Albedo = {0.8f, 0.8f, 0.0f};
		materialGround->Roughness = 1.0f;
		m_Scene.Materials.push_back(materialGround);
		m_MaterialNames.push_back(materialGround->Name);

		/* auto materialCenter = make_shared<Lambertian>("center sphere");
		materialCenter->Albedo = {0.7f, 0.3f, 0.3f};
		materialCenter->Roughness = 1.0f;
		m_Scene.Materials.push_back(materialCenter);
		m_MaterialNames.push_back(materialCenter->Name);

		auto materialLeft = make_shared<Metal>("left sphere");
		materialLeft->Albedo = {0.8f, 0.8f, 0.8f};
		materialLeft->Fuzz = 0.0f;
		m_Scene.Materials.push_back(materialLeft);
		m_MaterialNames.push_back(materialLeft->Name); */

		auto materialCenter = make_shared<Lambertian>("center sphere");
		materialCenter->Albedo = {0.7f, 0.3f, 0.3f};
		materialCenter->Roughness = 1.0f;
		m_Scene.Materials.push_back(materialCenter);
		m_MaterialNames.push_back(materialCenter->Name);

		auto materialLeft = make_shared<Dielectric>("left sphere");
		materialLeft->IndexOfRefraction = 1.5f;
		m_Scene.Materials.push_back(materialLeft);
		m_MaterialNames.push_back(materialLeft->Name);

		auto materialRight = make_shared<Metal>("right sphere");
		materialRight->Albedo = {0.8f, 0.6f, 0.2f};
		materialRight->Fuzz = 0.0f;
		m_Scene.Materials.push_back(materialRight);
		m_MaterialNames.push_back(materialRight->Name);

		{
			auto sphere = make_shared<Sphere>();
			sphere->Position = {0.0f, -100.5f, -1.0f};
			sphere->Radius = 100.0f;
			sphere->MaterialIndex = 0;
			m_Scene.Hittables.add(sphere);
		}
		{
			auto sphere = make_shared<Sphere>();
			sphere->Position = {0.0f, 0.0f, -1.0f};
			sphere->Radius = 0.5f;
			sphere->MaterialIndex = 1;
			m_Scene.Hittables.add(sphere);
		}
		{
			auto sphere = make_shared<Sphere>();
			sphere->Position = {-1.0f, 0.0f, -1.0f};
			sphere->Radius = 0.5f;
			sphere->MaterialIndex = 2;
			m_Scene.Hittables.add(sphere);
		}
		{
			auto sphere = make_shared<Sphere>();
			sphere->Position = {1.0f, 0.0f, -1.0f};
			sphere->Radius = 0.5f;
			sphere->MaterialIndex = 3;
			m_Scene.Hittables.add(sphere);
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
		optionsChanged += ImGui::DragInt("Bounces", &m_Renderer.m_Bounces, 0.5f, 1, 64);

		optionsChanged += ImGui::Checkbox("Antialiasing", &m_Renderer.GetSettings().EnableAntialiasing);
		if (m_Renderer.GetSettings().EnableAntialiasing)
		{
			optionsChanged += ImGui::DragInt("Samples", &m_Renderer.m_Samples, 0.5f, 1, 100);
		}

		ImGui::End();
		ImGui::Begin("Camera");
		optionsChanged += m_Camera.RenderCameraOptions();
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Separator();
		optionsChanged += ImGui::ColorEdit3("Sky Color", &m_Scene.SkyColor.x);
		ImGui::Separator();
		ImGui::Text("Objects");
		ImGui::Separator();
		if (ImGui::Button("Add Sphere"))
		{
			auto sphere = make_shared<Sphere>();
			sphere->Position = {0.0f, 0.0f, 0.0f};
			sphere->Radius = 1.0f;
			sphere->MaterialIndex = 0;
			m_Scene.Hittables.add(sphere);
		}

		for (size_t i = 0; i < m_Scene.Hittables.objects.size(); i++)
		{
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::TreeNode(("Sphere " + std::to_string(i + 1)).c_str()))
			{
				optionsChanged += m_Scene.Hittables.objects[i]->RenderObjectOptions(m_MaterialNames);
				if (ImGui::Button("Delete"))
				{
					m_Scene.Hittables.objects.erase(m_Scene.Hittables.objects.begin() + i);
					i--; // Decrement i to account for the erased element
					optionsChanged++;
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("Materials");
		ImGui::Separator();
		if (ImGui::Button("Add Lambertian Material"))
		{
			auto material = make_shared<Lambertian>("Lambertian " + std::to_string(m_Scene.Materials.size() + 1));
			m_Scene.Materials.push_back(material);
			m_MaterialNames.push_back(material->Name);
		}

		if (ImGui::Button("Add Metal Material"))
		{
			auto material = make_shared<Metal>("Metal " + std::to_string(m_Scene.Materials.size() + 1));
			m_Scene.Materials.push_back(material);
			m_MaterialNames.push_back(material->Name);
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(static_cast<int>(i));
			if (ImGui::TreeNode((m_MaterialNames.at(i).c_str())))
			{
				auto material = m_Scene.Materials[i];
				ImGui::PushID(static_cast<int>(i));
				optionsChanged += material->RenderObjectOptions();

				ImGui::TreePop();
			}
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
	std::vector<std::string> m_MaterialNames;

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
