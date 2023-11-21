#pragma once

#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H

#include <limits.h>
#include <math.h>
#include <random>
#include <unordered_map>

#include "FastNoiseLite.h"
#include "GameObject.h"

class Tri
{
public:
    Tri(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

    glm::vec3 getClosest(glm::vec3 pos);

    bool intersectWithRay(
        glm::vec3 ray_dir,
        glm::vec3 ray_pos,
        float& t
    );

    bool intersectWithBox(
        glm::vec3 box_center,
        glm::vec3 box_r
    );

    glm::vec3 m_a;
    glm::vec3 m_b;
    glm::vec3 m_c;
private:
};

class MarchingCube : public GameObject
{
public:
    MarchingCube();
    MarchingCube(float size);
    ~MarchingCube();

    virtual void createVertex() = 0;
    virtual void updateVertex();

    virtual glm::vec3 interpolate(glm::vec3, glm::vec3, float, float, float);
    virtual void polygonize(vector<glm::vec3>, vector<float> gridValues);

    inline virtual float getSize() { return m_size; };
    inline virtual float getGridSize() { return m_grid_size; };
    inline virtual float getThreshold() { return m_threshold; };

    inline virtual void setSize(float size) { m_size = size; };
    inline virtual void setGridSize(float grid_size) { m_grid_size = grid_size; };
    inline virtual void setThreshold(float threshold) { m_threshold = threshold; };

protected:
    float m_size;
    float m_grid_size;
    float m_threshold;

    vector<glm::vec3> m_vertices;
    vector<glm::vec3> m_normals;
    vector<shared_ptr<Tri>> m_trimeshes;
    vector<float> m_weights;
};

class Metaball : public MarchingCube
{
public:
    Metaball(float size);

    virtual float getGridValue(glm::vec3 grid_point);
    virtual void createVertex();

private:
    glm::vec3 m_center;
};

class Terrain : public MarchingCube
{
public:
    Terrain(float size);
    virtual float getGridValue(glm::vec3 grid_point);
    virtual void createVertex();
    virtual void createWeights();
    void updateWeights(glm::vec3 ray_dir, glm::vec3 ray_pos);

    inline int getNoiseScale() { return m_noise_scale; };
    inline int getOctave() { return m_octaves; };
    inline float getFrequency() { return m_frequency; };
    inline float getStrength() { return m_strength; };
    inline bool getIsEdit() { return m_is_edit; };

    inline void setNoiseScale(int noise_scale) { m_noise_scale = noise_scale; };
    inline void setOctave(int octaves) { m_octaves = octaves; };
    inline void setFrequency(float frequency) { m_frequency = frequency; };
    inline void setStrength(float strength) { m_strength = strength; };
    inline void setIsEdit(bool edit) { m_is_edit = edit; };

    virtual void renderProperty();

private:
    int m_noise_scale;
    int m_octaves;

    float m_frequency;
    float m_brush_size;
    float m_strength;

    bool m_is_edit;
};

#endif // !MARCHINGCUBE_H