//
// Created by mclen on 01/08/2024.
//

#ifndef PARTICLES_COMPONENT_H
#define PARTICLES_COMPONENT_H

#include "glm/vec3.hpp"
namespace engine::component {
    struct physics {
      glm::vec3 velocity;
      float mass;
      bool isStatic;
    };

} // namespace engine::component

#endif // PARTICLES_COMPONENT_H
