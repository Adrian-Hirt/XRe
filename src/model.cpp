#include <xre/model.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor
//------------------------------------------------------------------------------------------------------
Model::Model() {}

//------------------------------------------------------------------------------------------------------
// Initialize the model.
// Arguments:
//  1) Vector of meshes for this model
//  2) Color of the model
//------------------------------------------------------------------------------------------------------
Model::Model(std::vector<Mesh> meshes, std::shared_ptr<Material> material) : Model::Model(meshes, glm::vec3(0.8f, 0.8f, 0.8f), material) {}
Model::Model(std::vector<Mesh> meshes, glm::vec3 color, std::shared_ptr<Material> material) {
  m_meshes = meshes;
  m_original_model_color = color;
  m_model_color = color;
  // TODO: check that we haven't reached the max number of models
  m_model_index = s_model_index++;
  m_material = material;
}

Model::Model(const char *model_path, std::shared_ptr<Material> material) : Model::Model(model_path, glm::vec3(0.8f, 0.8f, 0.8f), material) {}
Model::Model(const char *model_path, glm::vec3 color, std::shared_ptr<Material> material) {
  loadObj(model_path);
  m_model_color = color;
  m_original_model_color = color;
  m_model_index = s_model_index++;
  m_material = material;
}

void Model::render(RenderContext &ctx) {
  // Prepare model uniform buffer
  ModelUniformBufferObject uniform_buffer_object{};
  uniform_buffer_object.world = m_world_matrix;
  uniform_buffer_object.color = m_model_color;

  // Render a different color if the model is interacted with.
  if (m_interacted) {
    uniform_buffer_object.color = ColorUtils::lighten(m_model_color, 0.5f);
  }

  // Update context
  ctx.descriptor_set = m_material->getDescriptorset();
  ctx.model_uniform_buffer = m_material->getUniformBuffer();

  // Update uniform buffer
  const uint32_t offset = m_model_index * ctx.aligned_size;
  ctx.model_uniform_buffer->loadData(uniform_buffer_object, offset);

  // Bind descriptor set
  vkCmdBindDescriptorSets(
    ctx.command_buffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    ctx.pipeline_layout,
    1u,
    1u,
    &ctx.descriptor_set,
    1,
    &offset
  );

  // Bind the graphics pipeline of the material
  m_material->bind();
                  
  // Render meshes of this model
  for (Mesh mesh : m_meshes) {
    mesh.render(ctx);

    if (m_render_bounding_boxes) {
      mesh.renderBoundingBox(ctx);
    }
  }
}

void Model::toggleRenderBoundingBoxes() { m_render_bounding_boxes = !m_render_bounding_boxes; }

// void Model::renderWithTransparency() {
//   // Render the model twice, once with Counterclockwise
//   // cull mode, once with the normal clockwise cull mode, such
//   // that the transparency works correctly.
//   s_dx11_handler->useDefaultRasterizer(false);
//   renderMeshes();
//   s_dx11_handler->useDefaultRasterizer(true);
//   renderMeshes();
// }

void Model::setColor(glm::vec3 color) { m_model_color = color; }

void Model::resetColor() { m_model_color = m_original_model_color; }

glm::vec3 Model::getColor() { return m_model_color; }

void Model::loadObj(const char *model_path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string error_output;

  // Load the object
  bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, NULL, &error_output, model_path, NULL, true);
  Utils::checkBoolResult(result, error_output.c_str());

  // Loop over the shapes (i.e. meshes) of the loaded model
  for (tinyobj::shape_t shape : shapes) {
    std::vector<Vertex> mesh_vertices;
    std::vector<uint16_t> mesh_indices;

    size_t index_offset = 0;

    // Loop over the faces of the mesh, where each face is a number containing
    // the number of vertices in that face
    for (unsigned int face_vertices_count : shape.mesh.num_face_vertices) {
      // Loop over the vertices of the face
      for (unsigned int vertex_index = 0; vertex_index < face_vertices_count; vertex_index++) {
        tinyobj::index_t index = shape.mesh.indices[index_offset + vertex_index];
        Vertex current_vertex;

        // Load the X, Y, Z coordinates of the current vertex
        tinyobj::real_t vertex_x = attrib.vertices[3 * index.vertex_index + 0];
        tinyobj::real_t vertex_y = attrib.vertices[3 * index.vertex_index + 1];
        tinyobj::real_t vertex_z = attrib.vertices[3 * index.vertex_index + 2];
        current_vertex.position = glm::vec3(vertex_x, vertex_y, vertex_z);

        // Check whether we can load the normals, if not we set unit normals (please note
        // that lighting will not work correctly in that case).
        if (index.normal_index >= 0) {
          tinyobj::real_t normal_x = attrib.normals[3 * index.normal_index + 0];
          tinyobj::real_t normal_y = attrib.normals[3 * index.normal_index + 1];
          tinyobj::real_t normal_z = attrib.normals[3 * index.normal_index + 2];
          current_vertex.normal = glm::vec3(normal_x, normal_y, normal_z);
        } else {
          current_vertex.normal = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        // Check whether we can load the texture coordinates, if not we set 0,0 (please note
        // that textures will not work correctly in that case).
        if (index.texcoord_index >= 0) {
          tinyobj::real_t texture_x = attrib.texcoords[2 * index.texcoord_index + 0];
          tinyobj::real_t texture_y = attrib.texcoords[2 * index.texcoord_index + 1];
          current_vertex.texture_coord = glm::vec2(texture_x, texture_y);
        } else {
          current_vertex.texture_coord = glm::vec2(0.0f, 0.0f);
        }

        // Add the newly created vertex to the vector of vertices
        mesh_vertices.push_back(current_vertex);
      }

      // It's nessecary to "reverse" the order of the indices, as otherwise
      // the models will be rendered inside out
      for (int i = face_vertices_count - 1; i >= 0; i--) {
        mesh_indices.push_back(static_cast<uint16_t>(index_offset + i));
      }

      index_offset += face_vertices_count;
    }

    Mesh mesh = Mesh(mesh_vertices, mesh_indices);
    m_meshes.push_back(mesh);
  };
}

void Model::setWorldMatrix(glm::mat4 world_matrix) { m_world_matrix = world_matrix; }

// TODO: Build "outer" bounding box containing all meshes such that we first only
// need to check the outer bounding box and then only if we have a hit there
// we check the inner meshes. Currently, as most models only have one mesh,
// this should be enough.
bool Model::intersects(Model other) {
  for (auto mesh : m_meshes) {
    auto this_OOBB = mesh.getObjectOrientedBoundingBox().transformed(m_world_matrix);
    for (auto other_mesh : other.m_meshes) {
      auto other_OOBB = other_mesh.getObjectOrientedBoundingBox().transformed(other.m_world_matrix);
      if (this_OOBB.intersects(other_OOBB)) {
        return true;
      }
    }
  }

  return false;
}

bool Model::intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance) {
  for (auto mesh : m_meshes) {
    auto this_OOBB = mesh.getObjectOrientedBoundingBox().transformed(m_world_matrix);
    if (this_OOBB.intersects(line_start, line_direction, out_distance)) {
      return true;
    }
  }

  return false;
}

void Model::printBouindingBoxes() {
  for (auto mesh : m_meshes) {
    mesh.getObjectOrientedBoundingBox().print();
  }
}

void Model::setInteractedState(bool interacted) { m_interacted = interacted; }
