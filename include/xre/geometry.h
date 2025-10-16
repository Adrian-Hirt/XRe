#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

#define GLM_ENABLE_EXPERIMENTAL

// GLM includes
#include <glm/glm/mat4x4.hpp>
#include <glm/glm/gtx/quaternion.hpp>

namespace Geometry {
  inline XrPosef XrPoseIdentity() {
    return {{0, 0, 0, 1}, {0, 0, 0}};
  };

  inline glm::mat4 poseToMatrix(const XrPosef& pose){
    const glm::mat4 translation =
      glm::translate(glm::mat4(1.0f), glm::vec3(pose.position.x, pose.position.y, pose.position.z));

    const glm::mat4 rotation =
      glm::toMat4(glm::quat(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z));

    return glm::inverse(translation * rotation);
  }

  inline glm::mat4 createProjectionMatrix(XrFovf fov, float nearClip, float farClip){
    const float l = glm::tan(fov.angleLeft);
    const float r = glm::tan(fov.angleRight);
    const float d = glm::tan(fov.angleDown);
    const float u = glm::tan(fov.angleUp);

    const float w = r - l;
    const float h = d - u;

    glm::mat4 projectionMatrix;
    projectionMatrix[0] = { 2.0f / w, 0.0f, 0.0f, 0.0f };
    projectionMatrix[1] = { 0.0f, 2.0f / h, 0.0f, 0.0f };
    projectionMatrix[2] = { (r + l) / w, (u + d) / h, -(farClip + nearClip) / (farClip - nearClip), -1.0f };
    projectionMatrix[3] = { 0.0f, 0.0f, -(farClip * (nearClip + nearClip)) / (farClip - nearClip), 0.0f };
    return projectionMatrix;
  }

  inline glm::mat4 composeWorldMatrix(const glm::vec3& translation,
                                      const glm::quat& rotation,
                                      const glm::vec3& scale) {
    glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 R = glm::mat4_cast(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
    return T * R * S;
}
}
