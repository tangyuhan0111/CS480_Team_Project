#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"
#include "skybox.h"

#define numVBOs 2;
#define numIBs 2;


class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void Render();
    Camera* getCamera() { return m_camera; }
	glm::vec3 getPlanetPosition(const std::string& planetName) const; 

  private:
    std::string ErrorString(GLenum error);
    bool InitializeSkybox();
    void RenderSkybox();
    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist, std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    void AddPlanet(const std::string& name, const char* texturePath, float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, float axialTilt);
	void GenerateAsteroidBelt(int count, float innerRadius, float outerRadius, std::vector<glm::mat4>& beltMatrices);
	void RenderAsteroidBelt(const glm::mat4& view, const glm::mat4& projection);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_positionAttrib;
    GLint m_colorAttrib;
    GLint m_tcAttrib;
    GLint m_hasTexture;

    GLint m_lightPos;
    GLint m_viewPos;
    GLint m_ambientStrength;
    GLint m_specularStrength;
    GLint m_shininess;
    GLint m_isSun;

    GLint m_isEmissive;
    GLint m_emissiveColor;
    GLint m_emissiveStrength;

    std::vector<glm::mat4> m_innerBeltMatrix;
	std::vector<glm::mat4> m_outerBeltMatrix;
	int innerBeltCount = 3000;
	int outerBeltCount = 5000;

    /*Sphere* m_sphere;
    Sphere* m_sphere2;
    Sphere* m_sphere3;*/
    struct Planet {
        Sphere* sphere;
        std::string name;
        float radius;
        float orbitRadius;
        float orbitSpeed;
        float rotationSpeed;
        float axialTilt;
    };
    std::vector<Planet> m_planets;
    Sphere* m_sun;
    Sphere* m_moon;
	Sphere* m_asteroidBelt;

    Mesh* m_mesh;
    Skybox* m_skybox;
    std::vector<Sphere*> m_engineGlows;
    std::vector<Sphere*> m_engineTrailParticles;
};

#endif /* GRAPHICS_H */
