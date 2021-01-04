#include "DebugRenderer.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include <type_traits>
#include <vector>
#include <iostream>

namespace jle
{
	class DebugRendererImpl
	{
	public:
		DebugRendererImpl(void* glfwNative)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(glfwNative), true);
			ImGui_ImplOpenGL3_Init("#version 330 core");
			// Setup Dear ImGui style
			ImGui::StyleColorsClassic();
		}

		~DebugRendererImpl()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		
		template <typename T>
		void AddText(T text)
		{
			outputables.push_back(std::make_unique<OutputableTemplated<T>>(text));
		}

		void AddText(const std::string& text)
		{
			outputables.push_back(std::make_unique<OutputableString>(text));
		}

		void Render()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			constexpr ImGuiWindowFlags window_flags =
				ImGuiWindowFlags_NoDecoration		|
				ImGuiWindowFlags_AlwaysAutoResize	|
				ImGuiWindowFlags_NoSavedSettings	|
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav				|
				ImGuiWindowFlags_NoMove;
			if (ImGui::Begin("Debug info", nullptr, window_flags))
			{
				for (auto &outputable : outputables)
				{
					ImGui::TextUnformatted(outputable->GetOutput().c_str());
				}
			}
			ImGui::End();


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:

		struct iOutputable
		{
		public:
			virtual std::string GetOutput() = 0;
		};


		struct OutputableString : public iOutputable
		{
			OutputableString(const std::string& text)
			{
				str = text;
			}

			std::string GetOutput() override
			{
				return str;
			}

		private:
			std::string str;
		};


		template <typename T>
		struct OutputableTemplated : public iOutputable
		{
			OutputableTemplated(T value)
			{
				this->value = value;
			}

			// dereference is used such that T (which can also be for example an int*) is acted upon
			// as if it is the type T without pointers, for example int* becomes int.
			template<typename T>
			std::enable_if_t<std::is_pointer<T>::value, std::remove_pointer_t<T>&> dereference(T& t) {
				return *t;
			}

			template<typename T>
			std::enable_if_t<!std::is_pointer<T>::value, T&> dereference(T& t) {
				return t;
			}

			std::string GetOutput() override
			{
				return std::string{ std::to_string(dereference(value)) };
			}
		private:
			T value;
		};


		std::vector<std::unique_ptr<iOutputable>> outputables;	

	};

	DebugRenderer::DebugRenderer(void* glfwNative) : pImpl{ std::make_unique<DebugRendererImpl>(glfwNative) } {}
	DebugRenderer::~DebugRenderer() = default;

	template <typename T>
	void DebugRenderer::AddText(T text)
	{
		Pimpl()->AddText(text);
	}

	void DebugRenderer::AddText(const std::string& text)
	{
		Pimpl()->AddText(text);
	}

	void DebugRenderer::AddText(const char* text)
	{
		std::string str{ text };
		Pimpl()->AddText(str);
	}

	void DebugRenderer::Render()
	{
		Pimpl()->Render();
	}


	// We need to create versions of templated function
	// inside .cpp file because we use the Pimpl idiom
	#define instantiate_template_function(type)\
	template void DebugRenderer::AddText(type);

	// List of available versions of AddText templated function
	instantiate_template_function(int)
	instantiate_template_function(int*)
	instantiate_template_function(float)
	instantiate_template_function(float*)
	instantiate_template_function(double)
	instantiate_template_function(double*)
	instantiate_template_function(long)
	instantiate_template_function(long*)
	instantiate_template_function(long long)
	instantiate_template_function(long long*)
	instantiate_template_function(char)
	instantiate_template_function(char*)
}

