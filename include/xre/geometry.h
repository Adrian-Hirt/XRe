#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// DirectX includes
#include <d3d11.h>
#include <directxmath.h>

namespace Geometry {
  inline DirectX::XMMATRIX computeViewProjectionMatrix(XrCompositionLayerProjectionView &view) {
    // First, we need to build the projection matrix

    // Set values for the near and far clipping plane. Usually, the near plane is
    // clipped at 1, but for XR applications, this seems to be too close, and as
    // such a lower value is used.
    const float far_clipping = 100.0f;
    const float near_clipping = 0.05f;

    // Compute the four coordinates of the view at the near clipping plane.
    // We get the FOV from OpenXR, in the form of four angles, which we need
    // to compute the tan from then and then multiply with the near clipping
    // value to get the positions.
    float view_left = near_clipping * tanf(view.fov.angleLeft);
    float view_right = near_clipping * tanf(view.fov.angleRight);
    float view_top = near_clipping * tanf(view.fov.angleUp);
    float view_bottom = near_clipping * tanf(view.fov.angleDown);

    // Then we can compute the perspective matrix from the values
    // we just computed.
    DirectX::XMMATRIX perspective_matrix = DirectX::XMMatrixPerspectiveOffCenterRH(view_left,
                                                                                  view_right,
                                                                                  view_bottom,
                                                                                  view_top,
                                                                                  near_clipping,
                                                                                  far_clipping);

    // Build the view matrix

    // Load the rotation quaternion and the position vector into a vector
    DirectX::XMVECTOR view_rotation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&view.pose.orientation);
    DirectX::XMVECTOR view_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&view.pose.position);

    // Build an affine transformation matrix from the rotation & position. The First
    // Param is a scaling factor, which we set to all ones, as we currently don't want
    // any scaling to be applied. The second param is the rotation origin, which we set
    // to a vector of all zeroes, such that we rotate about the origin.
    DirectX::XMMATRIX view_matrix = DirectX::XMMatrixInverse(NULL, DirectX::XMMatrixAffineTransformation(DirectX::g_XMOne, DirectX::g_XMZero, view_rotation, view_position));

    // Return the transposed product of the view and the perspective matrix
    return DirectX::XMMatrixTranspose(view_matrix * perspective_matrix);
  }
}
