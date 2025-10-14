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
Model::Model(std::vector<Mesh*> meshes) : Model::Model(meshes, glm::vec3(0.8f, 0.8f, 0.8f)) {}
Model::Model(std::vector<Mesh*> meshes, glm::vec3 color)  {
  m_meshes = meshes;
  m_original_model_color = color;
  m_model_color = color;
}

Model::Model(const char *model_path) : Model::Model(model_path, glm::vec3(0.8f, 0.8f, 0.8f)) {}
Model::Model(const char *model_path, glm::vec3 color) {
  loadObj(model_path);
  m_model_color = color;
  m_original_model_color = color;
}

void Model::render(RenderContext& ctx) {
  for (Mesh* mesh : m_meshes) {
    mesh->render(ctx);
  }
}

// void Model::renderWithTransparency() {
//   // Render the model twice, once with Counterclockwise
//   // cull mode, once with the normal clockwise cull mode, such
//   // that the transparency works correctly.
//   s_dx11_handler->useDefaultRasterizer(false);
//   renderMeshes();
//   s_dx11_handler->useDefaultRasterizer(true);
//   renderMeshes();
// }

// void Model::setColor(DirectX::XMFLOAT4 color) {
//   m_model_color = color;
// }

// void Model::resetColor() {
//   m_model_color = m_original_model_color;
// }

// DirectX::XMFLOAT4 Model::getColor() {
//   return m_model_color;
// }

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

        // Assign the model color for now
        current_vertex.color = m_model_color;

        // // Check whether we can load the normals, if not we set unit normals (please note
        // // that lighting will not work correctly in that case).
        // if (index.normal_index >= 0) {
        //   tinyobj::real_t normal_x = attrib.normals[3 * index.normal_index + 0];
        //   tinyobj::real_t normal_y = attrib.normals[3 * index.normal_index + 1];
        //   tinyobj::real_t normal_z = attrib.normals[3 * index.normal_index + 2];
        //   current_vertex.normal = DirectX::XMFLOAT3(normal_x, normal_y, normal_z);
        // }
        // else {
        //   current_vertex.normal = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
        // }

        // Check whether we can load the texture coordinates, if not we set 0,0 (please note
        // that textures will not work correctly in that case).
        if (index.texcoord_index >= 0) {
          tinyobj::real_t texture_x = attrib.texcoords[2 * index.texcoord_index + 0];
          tinyobj::real_t texture_y = attrib.texcoords[2 * index.texcoord_index + 1];
          current_vertex.texture_coord = glm::vec2(texture_x, texture_y);
        }
        else {
          current_vertex.texture_coord = glm::vec2(0.0f, 0.0f);
        }

        // Add the newly created vertex to the vector of vertices
        mesh_vertices.push_back(current_vertex);

        // It's nessecary to "reverse" the order of the indices, as otherwise
        // the models will be rendered inside out
        mesh_indices.push_back(index_offset + (face_vertices_count - 1) - vertex_index);
      }

      index_offset += face_vertices_count;
    }

    auto mesh = new Mesh(mesh_vertices, mesh_indices);
    m_meshes.push_back(mesh);
  };
}

// std::vector<DirectX::XMFLOAT3> Model::getMeshBoundingBoxCorners() {
//   size_t points_count = m_meshes.size() * 8;
//   std::vector<DirectX::XMFLOAT3> all_corners;

//   // Get all bounding boxes of all meshes, and put the
//   // corners into the previously defined array
//   for(Mesh mesh : m_meshes) {
//     DirectX::XMFLOAT3 corners[8];
//     mesh.getBoundingBox().GetCorners(corners);

//     for (size_t j = 0; j < 8; j++) {
//      all_corners.push_back(corners[j]);
//     }
//   }

//   return all_corners;
// }
