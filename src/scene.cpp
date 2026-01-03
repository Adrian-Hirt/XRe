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

void Scene::setNodeGrabbable(SceneNode *node, bool grabbable) {
  if (grabbable) {
    m_grabbable_scene_nodes.insert(node);
  } else {
    m_grabbable_scene_nodes.erase(node);
  }
}

std::unordered_set<SceneNode *> Scene::getGrabbableNodeInstances() {
  std::unordered_set<SceneNode *> result;

  for (SceneNode *current_node : m_grabbable_scene_nodes) {
    if (current_node->isActive()) {
      result.insert(current_node);
    }
  }

  return result;
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
  for (SceneNode *current_node : m_grabbable_scene_nodes) {
    current_node->m_intersected_in_current_frame = false;
    current_node->m_grabbed = false;
  }
}

void Scene::addButton(Button* button) {
  m_button_instances.insert(button);
}

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
