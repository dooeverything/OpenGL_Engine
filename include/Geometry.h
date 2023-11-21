#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "GameObject.h"

class Geometry : public GameObject
{
public:
	Geometry();
	~Geometry();

private:
	virtual vector<info::VertexLayout> calculateVertex() = 0;
	virtual vector<unsigned int> calculateIndex() = 0;
};

class Sphere : public Geometry
{
public:
	Sphere(bool is_create_gizmo);
	~Sphere();

private:
	virtual vector<info::VertexLayout> calculateVertex();
	virtual vector<unsigned int> calculateIndex();

	float m_division;
	float m_radius;
};

#endif // !GEOMETRY_H

