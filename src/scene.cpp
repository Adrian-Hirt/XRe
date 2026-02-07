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

void Scene::resetInteractionStates() {
  for (auto component : m_grabbable_components) {
    component->getSceneNode()->m_grabbed = false;
    component->getSceneNode()->m_intersected_in_current_frame = false;
  }
}

void Scene::addButton(Button *button) { m_button_instances.insert(button); }

void Scene::processButtonInteractions() {
  for (auto button : getButtonInstances()) {
    button->processInteractions();
  }
}

void Scene::resetButtonInteractions() {
  for (auto button : m_button_instances) {
    button->resetInteractionState();
  }
}

std::unordered_set<Button *> Scene::getButtonInstances() {
  std::unordered_set<Button *> result;

  for (Button *button : m_button_instances) {
    if (button->isEnabled()) {
      result.insert(button);
    }
  }

  return result;
}

void Scene::registerComponent(Component& component) {
  if (auto* grabbable = dynamic_cast<GrabbableComponent*>(&component)) {
    m_grabbable_components.push_back(grabbable);
  }
}

void Scene::unregisterComponent(Component* component) {
  if (auto* grabbable = dynamic_cast<GrabbableComponent*>(component)) {
    std::erase(m_grabbable_components, grabbable);
  }
}

const std::vector<GrabbableComponent*>& Scene::getGrabbableComponents() const {
  return m_grabbable_components;
};
