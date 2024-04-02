#include <xre/model.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Model::Model() {}

//------------------------------------------------------------------------------------------------------
// Initialize the model.
// Arguments:
//  1) Vector of meshes for this model
//  2) Color of the model
//------------------------------------------------------------------------------------------------------
Model::Model(std::vector<Mesh> meshes, DirectX::XMFLOAT4 color) {
  this->meshes = meshes;
  this->model_color = color;

  buildBoundingBox();
}

Model::Model(const char *model_path, DirectX::XMFLOAT4 color) {
  loadObj(model_path);
  this->model_color = color;

  buildBoundingBox();
}

void Model::render() {
  // Get the current active shader
  Shader* shader = Shader::getCurrentActiveShader();

  render(shader);
}

void Model::render(Shader *shader) {
  // Activate the shader
  shader->activate();

  // Update the shader with the model matrix
  shader->setModelMatrix(getTransformationMatrix());
  shader->setNormalRotationMatrix(getRotationMatrix());
  shader->setModelColor(model_color);
  shader->updatePerModelConstantBuffer();

  for (Mesh &mesh : meshes) {
    mesh.render();
  }

  // Next, render the bounding box
  bounding_box_mesh.render();
}

DirectX::XMMATRIX Model::getTransformationMatrix() {
  return DirectX::XMMatrixTranspose(DirectX::XMMatrixAffineTransformation(
         scaling,
         DirectX::g_XMZero,
         rotation,
         translation));

}

DirectX::XMMATRIX Model::getRotationMatrix() {
  return DirectX::XMMatrixRotationQuaternion(this->rotation);
}

void Model::rotate(float roll, float pitch, float yaw) {
  DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
  rotate(rotation);
}

void Model::rotate(DirectX::XMVECTOR rotation) {
  this->rotation = DirectX::XMQuaternionMultiply(this->rotation, rotation);
}

void Model::translate(float x, float y, float z) {
  DirectX::XMVECTOR translation = DirectX::XMVECTORF32({x, y, z});
  translate(translation);
}

void Model::translate(DirectX::XMVECTOR translation) {
  this->translation = DirectX::XMVectorAdd(this->translation, translation);
}

void Model::scale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  scale(scaling);
}

void Model::scale(DirectX::XMVECTOR scaling) {
this->scaling = DirectX::XMVectorMultiply(this->scaling, scaling);
}

void Model::setRotation(DirectX::XMVECTOR rotation) {
  this->rotation = rotation;
}

void Model::setScale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  setPosition(scaling);
}

void Model::setScale(DirectX::XMVECTOR scaling) {
  this->scaling = scaling;
}

void Model::setPosition(float x, float y, float z) {
  DirectX::XMVECTOR position = DirectX::XMVECTORF32({x, y, z});
  setPosition(position);
}

void Model::setPosition(DirectX::XMVECTOR position) {
  this->translation = position;
}

void Model::setColor(DirectX::XMFLOAT4 color) {
  model_color = color;
}

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
    std::vector<vertex_t> mesh_vertices;
    std::vector<unsigned int> mesh_indices;

    size_t index_offset = 0;

    // Loop over the faces of the mesh, where each face is a number containing
    // the number of vertices in that face
    for (unsigned int face_vertices_count : shape.mesh.num_face_vertices) {
      // Loop over the vertices of the face
      for (unsigned int vertex_index = 0; vertex_index < face_vertices_count; vertex_index++) {
        tinyobj::index_t index = shape.mesh.indices[index_offset + vertex_index];
        vertex_t current_vertex;

        // Load the X, Y, Z coordinates of the current vertex
        tinyobj::real_t vertex_x = attrib.vertices[3 * index.vertex_index + 0];
        tinyobj::real_t vertex_y = attrib.vertices[3 * index.vertex_index + 1];
        tinyobj::real_t vertex_z = attrib.vertices[3 * index.vertex_index + 2];
        current_vertex.coordinates = DirectX::XMFLOAT3(vertex_x, vertex_y, vertex_z);

        // Check whether we can load the normals, if not we set unit normals (please note
        // that lighting will not work correctly in that case).
        if (index.normal_index >= 0) {
          tinyobj::real_t normal_x = attrib.normals[3 * index.normal_index + 0];
          tinyobj::real_t normal_y = attrib.normals[3 * index.normal_index + 1];
          tinyobj::real_t normal_z = attrib.normals[3 * index.normal_index + 2];
          current_vertex.normal = DirectX::XMFLOAT3(normal_x, normal_y, normal_z);
        }
        else {
          current_vertex.normal = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
        }

        // Check whether we can load the texture coordinates, if not we set 0,0 (please note
        // that textures will not work correctly in that case).
        if (index.texcoord_index >= 0) {
          tinyobj::real_t texture_x = attrib.texcoords[2 * index.texcoord_index + 0];
          tinyobj::real_t texture_y = attrib.texcoords[2 * index.texcoord_index + 1];
          current_vertex.texture_coordinates = DirectX::XMFLOAT2(0.0f, 0.0f);
        }
        else {
          current_vertex.texture_coordinates = DirectX::XMFLOAT2(0.0f, 0.0f);
        }

        // Add the newly created vertex to the vector of vertices
        mesh_vertices.push_back(current_vertex);

        // It's nessecary to "reverse" the order of the indices, as otherwise
        // the models will be rendered inside out
        mesh_indices.push_back(index_offset + (face_vertices_count - 1) - vertex_index);
      }

      index_offset += face_vertices_count;
    }

    meshes.push_back(Mesh(mesh_vertices, mesh_indices));
  };
}

void Model::registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  Model::device = device;
  Model::device_context = device_context;
}

void Model::buildBoundingBox() {
  size_t points_count = meshes.size() * 8;
  DirectX::XMFLOAT3 all_corners[points_count];
  size_t idx = 0;

  // Get all bounding boxes of all meshes, and put the
  // corners into the previously defined array
  for(Mesh mesh : meshes) {
    DirectX::XMFLOAT3 corners[8];
    mesh.getBoundingBox().GetCorners(corners);

    for (size_t j = 0; j < 8; j++) {
      all_corners[idx++] = corners[j];
    }
  }

  // Build the bounding box
  DirectX::BoundingOrientedBox::CreateFromPoints(bounding_box, points_count, all_corners, sizeof(DirectX::XMFLOAT3));

  // Create the bounding box mesh, such that we can render it
  DirectX::XMFLOAT3 corners[bounding_box.CORNER_COUNT];
  bounding_box.GetCorners(corners);
  std::vector<vertex_t> bounding_box_vertices;

  // Create vertices from the corners of the bounding box
  for (int i = 0; i < bounding_box.CORNER_COUNT; i++) {
    bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
  }

  bounding_box_mesh = BoundingBoxMesh(bounding_box_vertices);
}

bool Model::intersects(DirectX::BoundingOrientedBox other) {
  // TODO: if intersects, check all "child bounding boxes"
  // for increased accuracy
  return getTransformedBoundingBox().Intersects(other);
}

DirectX::BoundingOrientedBox Model::getTransformedBoundingBox() {
  DirectX::BoundingOrientedBox transformed;

  // This method does not work with scaling in different x, y and z scales, and
  // therefore we need to build a transformation matrix with just scaling and
  // apply that one. Also, the method that uses a transformation matrix ignores
  // translations. We therefore build a matrix only for scaling, apply it, and
  // then apply rotation and translation.
  DirectX::XMFLOAT3 scalingVect;
  DirectX::XMStoreFloat3(&scalingVect, scaling);
  DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scalingVect.x, scalingVect.y, scalingVect.z);

  // Apply the scaling
  bounding_box.Transform(transformed, scaling);

  // Apply rotation and translation
  transformed.Transform(transformed, 1.0f, rotation, translation);

  // Done
  return transformed;
}

void Model::makeInteractable() {
  Model::interactable_instances.push_back(this);
}
