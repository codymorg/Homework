#include "..\Include\ParentedObject.h"
#include "ObjectManager.h"

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

using glm::mat4x4; 
using glm::vec3;

Parented::Parented(std::string name, int segments, float width) : Object(name)
{
  loadChain(segments, width);

  transforms_.reserve(segments);
  accelerations_.reserve(segments);
  velocities_.reserve(segments);
  masses_.reserve(segments);

  for(auto i = 0; i < segments; i++)
  {
    transforms_.push_back(mat4x4(1.0f));
    accelerations_.push_back(vec3(0));
    velocities_.push_back(vec3(0));
    masses_.push_back(1);
  }



  // add a little mass on the end to prevent whipping
}

void Parented::draw()
{
  Parented::update();
  Object::draw();
}

void Parented::update()
{
  auto objectmgr = ObjectManager::getObjectManager();
  float dt = objectmgr->dt;
  auto floorPos = glm::vec4(objectmgr->getFirstObjectByName("floor")->getWorldPosition(),1);
  auto floorScale = glm::vec4(objectmgr->getFirstObjectByName("floor")->getWorldScale(),1);
  auto floorLocal = glm::inverse(modelToWorld_) * floorPos + floorScale.y;
  
  // update velocity v`=v+at
  for (auto i = 0; i < velocities_.size(); i++)
  {
    velocities_[i] += masses_[i] * accelerations_[i] * dt;
  }

  // update transforms
  for (auto i = 0; i < transforms_.size(); i++)
  {
    transforms_[i] = glm::translate(transforms_[i], velocities_[i]);
  }

  // update verts
  for (auto i = 1; i < vertices_.size(); i++)
  {
    // find vector to previous
    vertices_[i].position = vec3(transforms_[i] * glm::vec4(vertices_[i].position, 1));
    if(vertices_[i].position.y < floorLocal.y)
    {
      energyConservation_ *= 0.95;
    }
    vertices_[i].position.y = std::max(vertices_[i].position.y, floorLocal.y);
    vec3 vp = vertices_[i-1].position - vertices_[i].position;

    // find distance
    float dist = glm::length(vp);
    vp /= dist;

    // translate (d-1)vp
    vec3 newpoint = vertices_[i].position + (dist - 1) * vp;
    velocities_[i] = energyConservation_ * (newpoint - vertices_[i].position);
    vertices_[i].position = newpoint;
  }
  initBuffers();
}

void Parented::addAcceleration(glm::vec3 acceleration, int node)
{
  // set to all nodes
  if( node == -1)
  {
    // set gravity constant acceleration
    for (auto& acc : accelerations_)
    {
      acc += acceleration;
    }
  }
  else
  {
    accelerations_[node] = acceleration;
  }
}

void Parented::addVelocity(glm::vec3 vel, int node)
{
  // set to all nodes
  if (node == -1)
  {
    // set gravity constant acceleration
    for (auto& velocity : velocities_)
    {
      velocity += vel;
    }
  }
  else
  {
    velocities_[node] = vel;
  }
}
