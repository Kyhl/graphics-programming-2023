#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGL/asset/ShaderLoader.h>


class TextureCubemapObject;
class Material;
class Texture2DObject;
class SolarSystemSimulation : public Application
{
public:
    SolarSystemSimulation();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeMaterial();
    void InitializeModels();
    void InitializeRenderer();
    std::shared_ptr<Texture2DObject> LoadTexture(const char* path);
    void RenderGUI();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;

    // Global scene
    Scene m_scene;

    // Renderer
    Renderer m_renderer;

    bool m_enablePlanetRotation;

    float m_planetRotationSpeed;

    // Skybox texture
    std::shared_ptr<TextureCubemapObject> m_skyboxTexture;

    std::shared_ptr<Material> m_planetMaterial;

    std::shared_ptr<Material> m_sunMaterial;

};
