#include "light.h"
#include "GLFW/glfw3.h"

#include "config.h"
#include "camera.h"
#include "meshObjectManager.h"
#include "dataCollection.h"

LightManager* LightManager::instance = nullptr;
int LightManager::lightCount;


PointLight::PointLight(vec3 pos, vec3 color, float intensity)
	: position(pos), color(color), intensity(intensity)
{
	//Create the light source and register it to the vector
	sphereModel = MeshObject::Get()->LoadModel("../projects/GraphicV2/asset/icosphere.obj");
	lightID = LightManager::Get()->GetLightCount();
	LightManager::Get()->RegisterLight(this);
	LightManager::Get()->pLights.push_back(this);
	
	lightString = std::string("pointLight.");
	Spos = lightString + std::string("position");
	Scolor = lightString + std::string("color");
	Sintensity = lightString + std::string("intensity");
	Sconstant = lightString + std::string("constant");
	Slinear = lightString + std::string("linear");
	Squadratic = lightString + std::string("quadratic");
}

void PointLight::Update(ShaderResource& program, Object::Camera& cam)
{
	if (bOrbit)
		Orbit(glfwGetTime());

	//calculate the maxintenstity
	intensityMax = std::max(std::max(color.x, color.y), color.z) * intensity;

	//calculate the radius (light volume)
	radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - 256/4 * intensityMax))) / (2 * quadratic);

	program.SetUniformVec3("camPos", cam.GetPosition());
	program.SetUniformMat4f("projView", cam.GetProjView());

	mat4 modelMatrix = mat4(
		vec4(radius, 0, 0, 0),
		vec4(0, radius, 0, 0),
		vec4(0, 0, radius, 0),
		vec4(position.x, position.y, position.z, 1));

	program.SetUniformMat4f("model", modelMatrix);

	program.SetUniform1f("pointLight.radius", radius);
	program.SetUniformVec3(Spos, position);
	program.SetUniformVec3(Scolor, color);
	program.SetUniform1f(Sintensity, intensity);
	program.SetUniform1f(Sconstant, constant);
	program.SetUniform1f(Slinear, linear);
	program.SetUniform1f(Squadratic, quadratic);

	sphereModel->SimpleDraw();

}

void PointLight::DisableLight(ShaderResource& program)
{
	program.SetUniformVec3(Spos, vec3());
	program.SetUniformVec3(Scolor, vec3());
	program.SetUniform1f(Sintensity, 0);
}

void PointLight::Orbit(float totalTime)
{
	float spinSpeed = 1;
	float radius = 4;
	position.x = sin(totalTime * spinSpeed) * radius;
	position.z = cos(totalTime * spinSpeed) * radius;
}

DirectionalLight::DirectionalLight(vec3 dir, vec3 color, float intensity)
	: direction(dir), color(color), intensity(intensity)
{
	lightID = LightManager::Get()->GetLightCount();
	LightManager::Get()->RegisterLight(this);
}

void DirectionalLight::Update(ShaderResource& program, Object::Camera& cam)
{
	program.SetUniformVec3("dirLight.direction", direction);
	program.SetUniformVec3("dirLight.color", color);
	program.SetUniform1f("dirLight.intensity", intensity);
}

void DirectionalLight::DisableLight(ShaderResource& program)
{
	program.SetUniformVec3("dirLight.direction", vec3());
	program.SetUniformVec3("dirLight.color", vec3());
	program.SetUniform1f("dirLight.intensity", 0);
}
