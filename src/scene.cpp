#include <xre/scene.h>

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
