#include <xre/mesh.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Mesh::Mesh() {}

//------------------------------------------------------------------------------------------------------
// Initialize the mesh.
// Arguments:
//  1) Vector with the vertices of the mesh
//  2) Vector with the incides of the mesh
//------------------------------------------------------------------------------------------------------
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
  // Call general initialize method
  initialize(vertices, indices);
}

// //------------------------------------------------------------------------------------------------------
// // Initialize the mesh.
// // Arguments:
// //  1) Vector with the vertices of the mesh
// //  2) Vector with the incides of the mesh
// //  3) Path to the file holding the texture to be applied
// //------------------------------------------------------------------------------------------------------
// Mesh::Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path) {
//   // Call general initialize method
//   initialize(vertices, indices);

//   // Load the texture
//   std::wstring filepath = Utils::stringToWString(texture_path);
//   ID3D11Resource *texture; // Throwaway variable
//   // Use the `Ex` variant, which allows us to set the loader to assume that
//   // the passed in texture is already sRGB.
//   HRESULT result = DirectX::CreateWICTextureFromFileEx(
//     s_device,
//     s_device_context,
//     filepath.c_str(),
//     0,
//     D3D11_USAGE_DEFAULT,
//     D3D11_BIND_SHADER_RESOURCE,
//     0,
//     0,
//     DirectX::WIC_LOADER_SRGB_DEFAULT,
//     &texture,
//     &m_texture_view
//   );

//   std::string error_string = "Failed to load the texture ";
//   error_string += texture_path;
//   Utils::checkHresult(result, error_string.c_str());
// }
