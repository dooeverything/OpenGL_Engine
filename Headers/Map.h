#pragma once

#ifndef MAP_H
#define MAP_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

using namespace std;

class GameObject;

class Map
{
public:
	Map();
	Map(int width, int height, string name);
	~Map();

protected:
	int m_width;
	int m_height;
	string m_name;

	shared_ptr<Shader> m_shader;
	shared_ptr<Mesh> m_mesh;
};

class ShadowMap : protected Map
{
public:
	ShadowMap(int width, int height);
	ShadowMap(int width, int height, glm::vec3 m_position);
	~ShadowMap();

	void draw(vector<shared_ptr<GameObject>>& gameobjects);
	inline ShadowBuffer& getBuffer() { return *m_shadow_buffer; };
	inline glm::mat4* getProj() { return &m_proj; };
	inline glm::mat4* getView() { return &m_view; };
	inline glm::vec3* getPosition() { return &m_light_position; };

private:
	shared_ptr<ShadowBuffer> m_shadow_buffer;
	glm::mat4 m_proj;
	glm::mat4 m_view;
	glm::vec3 m_light_position;
};

class CubeMap : public Map
{
public:
	CubeMap(int width, int height);
	~CubeMap();

	void drawMap();
	void draw(glm::mat4& P, glm::mat4& V);
	inline shared_ptr<CubemapBuffer> getCubemapBuffer() { return m_cubemap_buffer; };

private:
	shared_ptr<CubemapBuffer> m_cubemap_buffer;
	unique_ptr<Shader> m_shader_background;
	shared_ptr<Texture> m_hdr_texture;
};

class IrradianceMap : protected Map
{
public:
	IrradianceMap(int width, int height);
	~IrradianceMap();

	void drawMap(CubemapBuffer& cubemap);
	void draw(glm::mat4& P, glm::mat4& V);
	inline shared_ptr<CubemapBuffer> getCubemapBuffer() { return m_irradiance_buffer; };

private:
	shared_ptr<CubemapBuffer> m_irradiance_buffer;
	unique_ptr<Shader> m_shader_background;
};

class PrefilterMap : public Map
{
public:
	PrefilterMap();
	PrefilterMap(int width, int height);
	~PrefilterMap();

	void drawMap(CubemapBuffer& cubemap);
	//void draw(glm::mat4& P, glm::mat4& V);
	inline shared_ptr<CubemapBuffer> getCubemapBuffer() { return m_prefilter_buffer; };

private:
	shared_ptr<CubemapBuffer> m_prefilter_buffer;
};

class LUTMap : protected Map
{
public:
	LUTMap();
	LUTMap(int width, int height);
	~LUTMap();

	void drawMap();
	inline shared_ptr<FrameBuffer> getFrameBuffer() { return m_fb; };
private:
	shared_ptr<FrameBuffer> m_fb;
};

class EnvironmentMap : public Map
{
public:
	EnvironmentMap(glm::vec3 position);
	~EnvironmentMap();

	void draw(vector<shared_ptr<GameObject>>& gameobjects, Light& light);
private:
	const unsigned int WIDTH = 512;
	const unsigned int HEIGHT = 512;

	shared_ptr<CubemapBuffer> m_cubemap_buffer;
	glm::vec3 m_eye_position;
};

#endif