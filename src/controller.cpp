#include <xre/controller.h>

Controller::Controller(std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Create the model for visualizing the controllers
  m_model = ModelFactory::createCube({0.67f, 0.84f, 0.9f}, material, vulkan_handler);
  m_root_node = SceneNode();
  m_model_node = std::make_shared<SceneNode>(m_model);
  m_model_node->scale(0.03f, 0.03f, 0.075f);

  // Create the model for visualizing intersections of the aim line
  m_aim_indicator_sphere = ModelFactory::createSphere(material, {0.0f, 0.75f, 1.0f}, vulkan_handler);
  m_intersection_sphere_node = std::make_shared<SceneNode>(m_aim_indicator_sphere);
  m_intersection_sphere_node->scale(0.05f, 0.05f, 0.05f);

  // Create the line for the aim direction
  m_aim_line = std::make_shared<Line>(0.003f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f), material, vulkan_handler);

  // Setup the scene nodes
  m_root_node.addChildNode(m_model_node);
  m_root_node.addChildNode(m_intersection_sphere_node);
  m_root_node.addChildNode(m_aim_line);

  // Add the collider component
  m_model_node->addComponent(ColliderComponent::fromPointGroups(m_model->getVectorPositionsPerMesh()));
}

void Controller::render(RenderContext &ctx) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  // Color the model a different color depending on the "grab"
  // state of the controller
  if (m_grabbing) {
    m_model->setColor({1.0f, 0.0f, 0.0f});
  } else {
    m_model->resetColor();
  }

  m_root_node.render(ctx);
}

void Controller::updatePosition(glm::vec3 current_origin) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  glm::vec3 controller_position = Utils::toVec3(m_pose.position);
  glm::quat controller_orientation = Utils::toQuat(m_pose.orientation);

  // Apply the global teleport translation from moving the origin
  controller_position = controller_position + current_origin;

  // Set position and orientation of the scene node
  m_model_node->setPosition(controller_position);
  m_model_node->setRotation(controller_orientation);

  // Update the aim line
  m_aim_line->updateAimLineFromControllerPose(controller_position, Utils::toQuat(m_aim.orientation), m_aim_line_render_length);

  // Update the root node
  m_root_node.updateTransformation();
}

void Controller::updateAimIndicators(float aim_line_length, std::optional<glm::vec3> aim_sphere_position, bool terrain_intersection) {
  m_aim_line_render_length = aim_line_length;

  if (aim_sphere_position.has_value()) {
    m_intersection_sphere_node->setActive(true);
    m_intersection_sphere_node->setPosition(aim_sphere_position.value());

    if (terrain_intersection) {
      m_aim_indicator_sphere->setColor({1.0f, 1.0f, 0.0f});
    } else {
      m_aim_indicator_sphere->setColor({0.0f, 0.75f, 1.0f});
    }
  } else {
    m_intersection_sphere_node->setActive(false);
  }
}

std::shared_ptr<SceneNode> Controller::getSceneNodeForSceneNodeUpdate() { return m_model_node; }

std::vector<std::shared_ptr<SceneNode>> Controller::getSceneNodeForInteractionQuery() { return {m_model_node}; }

std::shared_ptr<Line> Controller::getAimLine() {
  return m_aim_line;
};
