#include <xre/scene.h>
#include <xre/scene_manager.h>
#include <xre/resource_manager.h>

std::shared_ptr<SceneNode> Scene::node() {
  auto scene_node = std::make_shared<SceneNode>();
  scene_node->setScene(this);
  return scene_node;
}

std::shared_ptr<SceneNode> Scene::node(std::shared_ptr<Model> model) {
  auto scene_node = std::make_shared<SceneNode>(model);
  scene_node->setScene(this);
  return scene_node;
}

void Scene::setNodeIsTerrain(SceneNode *node, bool is_terrain) {
  if (is_terrain) {
    m_terrain_scene_nodes.insert(node);
  } else {
    m_terrain_scene_nodes.erase(node);
  }
}

std::unordered_set<SceneNode *> Scene::getTerrainNodeInstances() {
  std::unordered_set<SceneNode *> result;

  for (SceneNode *current_node : m_terrain_scene_nodes) {
    if (current_node->isActive()) {
      result.insert(current_node);
    }
  }

  return result;
}

void Scene::registerComponent(Component &component) {
  if (auto *grabbable = dynamic_cast<GrabbableComponent *>(&component)) {
    m_grabbable_components.push_back(grabbable);
  } else if (auto *button = dynamic_cast<ButtonComponent *>(&component)) {
    m_button_components.push_back(button);
  } else if (auto *teleport_target = dynamic_cast<TeleportTargetComponent *>(&component)) {
    m_teleport_target_components.push_back(teleport_target);
  }
}

void Scene::unregisterComponent(Component &component) {
  if (auto *grabbable = dynamic_cast<GrabbableComponent *>(&component)) {
    std::erase(m_grabbable_components, grabbable);
  } else if (auto *button = dynamic_cast<ButtonComponent *>(&component)) {
    std::erase(m_grabbable_components, grabbable);
  } else if (auto *teleport_target = dynamic_cast<TeleportTargetComponent *>(&component)) {
    std::erase(m_teleport_target_components, teleport_target);
  }
}

const std::vector<GrabbableComponent *> &Scene::getGrabbableComponents() const { return m_grabbable_components; };

const std::vector<ButtonComponent *> &Scene::getButtonComponents() const { return m_button_components; };

const std::vector<TeleportTargetComponent *> &Scene::getTeleportTargetComponents() const { return m_teleport_target_components; };
