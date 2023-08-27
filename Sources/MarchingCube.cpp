#include "MarchingCube.h"
#include "Utils.h"

MarchingCube::MarchingCube(float size) :
	GameObject(),  m_size(size), m_center(0.0f), m_grid_size(0.1f), m_threshold(1.0f), 
	m_vertices({}), m_normals({})
{
}

MarchingCube::~MarchingCube()
{
}

glm::vec3 MarchingCube::interpolate(glm::vec3 grid1, glm::vec3 grid2,
								    float gridValue1, float gridValue2, float threshold)
{
	float newX;
	float newY;
	float newZ;
	float t = (threshold - gridValue1) / (gridValue2 - gridValue1);;

	newX = grid1.x + t * (grid2.x - grid1.x);
	newY = grid1.y + t * (grid2.y - grid1.y);
	newZ = grid1.z + t * (grid2.z - grid1.z);

	return { newX, newY, newZ };
}

void MarchingCube::polygonize(vector<glm::vec3> grids, vector<float> gridValues)
{
	vector<glm::vec3> vertexList(12, { 0, 0, 0 });

	int vertexIndex = 0;
	
	//cout << "Gridvalues : " << gridValues[0] << endl;
	//cout << "Threshold : " << m_threshold << endl;

	if (gridValues[0] <= m_threshold) vertexIndex |= 1;
	if (gridValues[1] <= m_threshold) vertexIndex |= 2;
	if (gridValues[2] <= m_threshold) vertexIndex |= 4;
	if (gridValues[3] <= m_threshold) vertexIndex |= 8;
	if (gridValues[4] <= m_threshold) vertexIndex |= 16;
	if (gridValues[5] <= m_threshold) vertexIndex |= 32;
	if (gridValues[6] <= m_threshold) vertexIndex |= 64;
	if (gridValues[7] <= m_threshold) vertexIndex |= 128;

	// If the vertex is outside or inside the surface
	if (table::edgeTable[vertexIndex] == 0) return;

	if (table::edgeTable[vertexIndex] & 1)
		vertexList.at(0) = interpolate(grids[0], grids[1], gridValues[0], gridValues[1], m_threshold);
	if (table::edgeTable[vertexIndex] & 2)
		vertexList.at(1) = interpolate(grids[1], grids[2], gridValues[1], gridValues[2], m_threshold);
	if (table::edgeTable[vertexIndex] & 4)
		vertexList.at(2) = interpolate(grids[2], grids[3], gridValues[2], gridValues[3], m_threshold);
	if (table::edgeTable[vertexIndex] & 8)
		vertexList.at(3) = interpolate(grids[3], grids[0], gridValues[3], gridValues[0], m_threshold);
	if (table::edgeTable[vertexIndex] & 16)
		vertexList.at(4) = interpolate(grids[4], grids[5], gridValues[4], gridValues[5], m_threshold);
	if (table::edgeTable[vertexIndex] & 32)
		vertexList.at(5) = interpolate(grids[5], grids[6], gridValues[5], gridValues[6], m_threshold);
	if (table::edgeTable[vertexIndex] & 64)
		vertexList.at(6) = interpolate(grids[6], grids[7], gridValues[6], gridValues[7], m_threshold);
	if (table::edgeTable[vertexIndex] & 128)
		vertexList.at(7) = interpolate(grids[7], grids[4], gridValues[7], gridValues[4], m_threshold);
	if (table::edgeTable[vertexIndex] & 256)
		vertexList.at(8) = interpolate(grids[0], grids[4], gridValues[0], gridValues[4], m_threshold);
	if (table::edgeTable[vertexIndex] & 512)
		vertexList.at(9) = interpolate(grids[1], grids[5], gridValues[1], gridValues[5], m_threshold);
	if (table::edgeTable[vertexIndex] & 1024)
		vertexList.at(10) = interpolate(grids[2], grids[6], gridValues[2], gridValues[6], m_threshold);
	if (table::edgeTable[vertexIndex] & 2048)
		vertexList.at(11) = interpolate(grids[3], grids[7], gridValues[3], gridValues[7], m_threshold);

	// Create triangles with vertices on edges
	for (int i = 0; table::triTable[vertexIndex][i] != -1; i += 3)
	{

		glm::vec3 vertex1 = vertexList[table::triTable[vertexIndex][i]];
		glm::vec3 vertex2 = vertexList[table::triTable[vertexIndex][i + 1]];
		glm::vec3 vertex3 = vertexList[table::triTable[vertexIndex][i + 2]];

		glm::vec3 edge1 = { vertex2.x - vertex1.x, vertex2.y - vertex1.y, vertex2.z - vertex1.z };
		glm::vec3 edge2 = { vertex3.x - vertex1.x, vertex3.y - vertex1.y, vertex3.z - vertex1.z };
		
		glm::vec3 n = glm::cross(edge1, edge2);

		m_vertices.push_back(vertex1);
		m_vertices.push_back(vertex2);
		m_vertices.push_back(vertex3);

		m_normals.push_back(n);
		m_normals.push_back(n); 
		m_normals.push_back(n);
	}
}

void MarchingCube::draw(glm::mat4& P, glm::mat4& V, Light& light,
					glm::vec3& view_pos, ShadowMap& shadow,
					IrradianceMap& irradiance, PrefilterMap& prefilter, LUTMap& lut)
{
	//cout << "Draw metaball" << endl;

	m_prefilter = prefilter.getCubemapBuffer()->getCubemapTexture();
	m_irradiance = irradiance.getCubemapBuffer()->getCubemapTexture();
	m_lut = lut.getFrameBuffer()->getTextureID();

	if (!m_click)
	{
		m_move_axis = -1;
	}

	m_shader->load();
	glm::mat4 shadow_proj = (*shadow.getProj()) * (*shadow.getView());
	m_shader->setMat4("light_matrix", shadow_proj);
	m_shader->setVec3("light_pos", *shadow.getPosition());
	m_shader->setFloat("animation", 0);
	m_shader->setVec3("view_pos", view_pos);
	m_shader->setLight(light);

	//m_shader->setFloat("width", m_screen_w);
	//m_shader->setFloat("height", m_screen_h);

	m_shader->setInt("preview", 0);

	// Load shadow map as texture
	m_shader->setInt("shadow_map", 0);
	glActiveTexture(GL_TEXTURE0);
	shadow.getBuffer().bindFrameTexture();
	m_shader->setInt("irradiance_map", 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	irradiance.getCubemapBuffer()->bindCubemapTexture();
	m_shader->setInt("prefilter_map", 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	prefilter.getCubemapBuffer()->bindCubemapTexture();
	m_shader->setInt("lut_map", 3);
	glActiveTexture(GL_TEXTURE0 + 3);
	lut.getFrameBuffer()->bindFrameTexture();

	m_mesh->draw(P, V, *m_shader, true);
}

Metaball::Metaball(float size) : MarchingCube(size)
{
	//m_center = glm::vec3(0.5f);
	//m_threshold = 1.0f;

	cout << "Metaball Constructor" << endl;
	cout << "Radius : " << m_size << endl;
	cout << "Center : " << m_center << endl;
	m_name = "Metaball";
	
	getVertex();

	vector<string> shader_path = { "Shaders/BRDF.vert", "Shaders/BRDF.frag" };
	m_shader = make_shared<Shader>(shader_path);
	loadShader();

	for (int axis = 0; axis < 3; ++axis)
	{
		shared_ptr<Gizmo> gizmo = make_shared<Gizmo>(*this, axis);
		m_gizmos.push_back(gizmo);
	}

	cout << "Metaball Constructor successfullly loaded" << endl;
	cout << endl;
}

float Metaball::calculateGridValue(glm::vec3 gridPoint)
{
	float x = gridPoint.x - m_center.x;
	float y = gridPoint.y - m_center.y;
	float z = gridPoint.z - m_center.z;

	return glm::sqrt(glm::pow(x, 2) + glm::pow(y, 2) + glm::pow(z, 2));
}

void Metaball::getVertex()
{
	vector<glm::vec3> gridPoints;
	vector<float> gridValues;

	for (float z = -m_size; z < m_size; z += m_grid_size)
	{
		for (float y = -m_size; y < m_size; y += m_grid_size)
		{
			for (float x = -m_size; x < m_size; x += m_grid_size)
			{
				gridPoints.push_back({ x, y, z });
				gridValues.push_back(calculateGridValue(gridPoints.at(0)));

				gridPoints.push_back({ x + m_grid_size, y, z });
				gridValues.push_back(calculateGridValue({ x + m_grid_size, y, z }));

				gridPoints.push_back({ x + m_grid_size, y, z + m_grid_size });
				gridValues.push_back(calculateGridValue({ x + m_grid_size, y, z + m_grid_size }));

				gridPoints.push_back({ x, y, z + m_grid_size });
				gridValues.push_back(calculateGridValue({ x, y, z + m_grid_size }));

				gridPoints.push_back({ x, y + m_grid_size, z });
				gridValues.push_back(calculateGridValue({ x, y + m_grid_size, z }));

				gridPoints.push_back({ x + m_grid_size, y + m_grid_size, z });
				gridValues.push_back(calculateGridValue({ x + m_grid_size, y + m_grid_size, z }));

				gridPoints.push_back({ x + m_grid_size, y + m_grid_size, z + m_grid_size });
				gridValues.push_back(calculateGridValue({ x + m_grid_size, y + m_grid_size, z + m_grid_size }));

				gridPoints.push_back({ x, y + m_grid_size, z + m_grid_size });
				gridValues.push_back(calculateGridValue({ x, y + m_grid_size, z + m_grid_size }));

				polygonize(gridPoints, gridValues);

				gridPoints.clear();
				gridValues.clear();
			}
		}
	}

	vector<info::VertexLayout> layouts;
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		info::VertexLayout layout;
		layout.position = m_vertices[i];
		layout.normal = m_normals[i];
		layouts.push_back(layout);
	}

	m_mesh = make_shared<Mesh>(m_name, layouts);
}

Terrain::Terrain(float size) : MarchingCube(size)
{
	cout << "Terrain Constructor" << endl;

	m_name = "Terrain";
	m_size = 10.0f;
	m_grid_size = 1.0f;
	m_threshold = 0.5f;
	m_frequency = 0.02f;
	m_octaves = 8;
	m_noise_scale = 1;

	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	noise.SetFrequency(m_frequency);
	noise.SetFractalOctaves(m_octaves);

	float num_weights = pow(m_size, 3.0);
	m_weights = vector<float>(num_weights);
	cout << "Num weights : " << num_weights << endl;
	for (float z = 0.0f; z < m_size; z += m_grid_size)
	{
		for (float y = 0.0f; y < m_size; y += m_grid_size)
		{
			for (float x = 0.0f; x < m_size; x += m_grid_size)
			{
				glm::vec3 pos = { x, y, z };
				pos *= m_noise_scale;

				float ground = -pos.y + (0.5f * m_size);
				float n = ground + noise.GetNoise(pos.x, pos.y, pos.z) * 10;
				int index = x + m_size * (y + m_size * z);
				m_weights[index] = n;
			}
		}
	}

	getVertex();

	vector<string> shader_path = { "Shaders/BRDF.vert", "Shaders/BRDF.frag" };
	m_shader = make_shared<Shader>(shader_path);
	loadShader();

	for (int axis = 0; axis < 3; ++axis)
	{
		shared_ptr<Gizmo> gizmo = make_shared<Gizmo>(*this, axis);
		m_gizmos.push_back(gizmo);
	}

	cout << "Terrain Constructor successfullly loaded" << endl;
	cout << endl;
}

void Terrain::getVertex()
{
	cout << "Get vertex" << endl;
	for (float z = 0.0f; z < m_size-1; z += m_grid_size)
	{
		for (float y = 0.0f; y < m_size-1; y += m_grid_size)
		{
			for (float x = 0.0f; x < m_size - 1; x += m_grid_size)
			{
				vector<glm::vec3> points(8);
				vector<float> values(8);

				points[0] = { x, y, z };
				values[0] = calculateGridValue(points[0]);

				points[1] = { x + m_grid_size, y, z };
				values[1] = calculateGridValue(points[1]);

				points[2] = { x + m_grid_size, y, z + m_grid_size };
				values[2] = calculateGridValue(points[2]);

				points[3] = { x, y, z + m_grid_size };
				values[3] = calculateGridValue(points[3]);

				points[4] = { x, y + m_grid_size, z };
				values[4] = calculateGridValue(points[4]);

				points[5] = { x + m_grid_size, y + m_grid_size, z };
				values[5] = calculateGridValue(points[5]);

				points[6] = { x + m_grid_size, y + m_grid_size, z + m_grid_size };
				values[6] = calculateGridValue(points[6]);

				points[7] = { x, y + m_grid_size, z + m_grid_size };
				values[7] = calculateGridValue(points[7]);

				polygonize(points, values);
			}
		}
	}

	vector<info::VertexLayout> layouts;
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		info::VertexLayout layout;
		layout.position = m_vertices[i];
		layout.normal = m_normals[i];
		layouts.push_back(layout);
	}

	m_mesh = make_shared<Mesh>(m_name, layouts);
}

float Terrain::calculateGridValue(glm::vec3 grid_point)
{
	int index = grid_point.x + m_size * (grid_point.y + m_size * grid_point.z);
	//cout << "Weights at " << grid_point << " : " << m_weights[index] << endl;
	return m_weights[index];
}


