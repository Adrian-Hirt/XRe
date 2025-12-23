#include <xre/object_oriented_bounding_box.h>

// ------------------------------------
// Power iteration eigenvector solver
// ------------------------------------
glm::vec3 OOBB::powerIteration(const glm::mat3 &matrix, int max_iterations = 50) {
  // Use a vector with all ones as the "starting" vector
  glm::vec3 b = glm::one<glm::vec3>();

  for (int i = 0; i < max_iterations; i++) {
    glm::vec3 b2 = matrix * b;
    float norm = glm::length(b2);
    if (norm < 1e-6f) {
      break;
    }
    b = b2 / norm;
  }
  return glm::normalize(b);
}

OOBB::OOBB() {}

OOBB::OOBB(const std::vector<glm::vec3> &points) {
  if (points.empty()) {
    m_center = glm::zero<glm::vec3>();
    m_extents = glm::zero<glm::vec3>();
    m_axes = glm::identity<glm::mat3>();
    return;
  }

  // -------------------------------
  // 1. Compute centroid
  // -------------------------------
  // Sum up all points and then divide by the number of points
  // to find the center of mass of the points.
  glm::vec3 centroid = glm::zero<glm::vec3>();
  for (auto &p : points) {
    centroid += p;
  }
  centroid /= (float)points.size();

  // -------------------------------
  // 2. Compute covariance matrix
  // -------------------------------
  // Build the covariance matrix of the points
  glm::mat3 covariance = glm::zero<glm::mat3>();

  for (auto &p : points) {
    glm::vec3 d = p - centroid;
    covariance[0][0] += d.x * d.x;
    covariance[0][1] += d.x * d.y;
    covariance[0][2] += d.x * d.z;
    covariance[1][0] += d.y * d.x;
    covariance[1][1] += d.y * d.y;
    covariance[1][2] += d.y * d.z;
    covariance[2][0] += d.z * d.x;
    covariance[2][1] += d.z * d.y;
    covariance[2][2] += d.z * d.z;
  }

  covariance /= (float)points.size();

  // -------------------------------
  // 3. Compute eigenvectors (principal axes)
  // -------------------------------
  // First eigenvector, directly computed from the power iteration
  // method.
  glm::vec3 e1 = powerIteration(covariance);

  // Approx eigenvalue for the first eigenvector we computed beforehand.
  float lambda1 = glm::dot(e1, covariance * e1);

  // Deflate matrix (remove component of first eigenvector), such that
  // computing the largest eigenvector of the matrix C2 gives us the
  // second largest eigenvector of the original C.
  glm::mat3 deflated_covariance = covariance - lambda1 * glm::outerProduct(e1, e1);

  // Compute eigenvector and approximate the second eigenvalue
  glm::vec3 e2 = powerIteration(deflated_covariance);

  // Check if e1 and e2 are too parallel, which might happen e.g. with cubes,
  // which means computing the principal components will fail.
  if (glm::length2(glm::cross(e1, e2)) < 1e-6f) {
    // Pick any vector orthogonal to e1
    if (fabs(e1.x) < 0.9f) {
      e2 = glm::normalize(glm::cross(e1, glm::vec3(1, 0, 0)));
    } else {
      e2 = glm::normalize(glm::cross(e1, glm::vec3(0, 1, 0)));
    }
  }
  float lambda2 = glm::dot(e2, covariance * e2);

  // Third eigenvector is orthogonal to the first two
  glm::vec3 e3 = glm::normalize(glm::cross(e1, e2));
  float lambda3 = glm::dot(e3, covariance * e3);

  // Orthonormalize e2 to remove small numerical errors
  e2 = glm::normalize(glm::cross(e3, e1));

  // Store the eigenvectors in a matrix for later usage
  glm::mat3 axes;

  // 4. Check for degenerate case (cube or sphere), where otherwise
  // the bounding boxes would be rotated by 45 degrees.
  float eps = 1e-5f;
  if (fabs(lambda1 - lambda2) < eps && fabs(lambda2 - lambda3) < eps) {
    // fallback to world axes
    axes = glm::identity<glm::mat3>();
  } else {
    axes[0] = e1;
    axes[1] = e2;
    axes[2] = e3;
  }

  // -------------------------------
  // 4. Transform all points to local space
  // -------------------------------
  // local = R^T (p - centroid). The part (p - centroid) moves the pointcloud
  // such that the centroid is at the origin, and R^T rotates the pointcloud
  // such that the principal components now align with the XYZ axies of the
  // coordinate system. At that point we only need to compute the points that are the furthest
  // away from the origin (== centroid) to compute the extends. Once we have
  // this, we effectively have an AABB, which we then can undo the transformation
  // to get an OOBB.
  glm::mat3 rotation = axes;
  glm::mat3 rotation_transposed = glm::transpose(rotation);

  glm::vec3 minv(std::numeric_limits<float>::max());
  glm::vec3 maxv(-std::numeric_limits<float>::max());

  for (auto &p : points) {
    glm::vec3 d = p - centroid;
    glm::vec3 q = rotation_transposed * d;

    minv = glm::min(minv, q);
    maxv = glm::max(maxv, q);
  }

  // -------------------------------
  // 5. Compute extents and center
  // -------------------------------
  glm::vec3 extents = 0.5f * (maxv - minv);
  glm::vec3 local_center = 0.5f * (maxv + minv);
  glm::vec3 world_center = rotation * local_center + centroid;

  m_center = world_center;
  m_extents = extents;
  m_axes = axes;
}

glm::vec3 OOBB::getCenter() { return m_center; };

glm::vec3 OOBB::getExtents() { return m_extents; };

glm::mat3 OOBB::getAxes() { return m_axes; };

void OOBB::print() {
  std::cout << "Center: " << m_center.x << ", " << m_center.y << ", " << m_center.z << "\n";

  std::cout << "Extents: " << m_extents.x << ", " << m_extents.y << ", " << m_extents.z << "\n";

  std::cout << "Axes: (columns)\n";

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << m_axes[i][j] << ", ";
    }
    std::cout << std::endl;
  }

  std::cout << "--------------" << std::endl;
}

OOBB OOBB::transformed(const glm::mat4 &model) const {
  OOBB transformed_bounding_box;

  // 1. Transform center (full transform)
  transformed_bounding_box.m_center = glm::vec3(model * glm::vec4(m_center, 1.0f));

  // 2. Extract rotation + scale (upper-left 3x3)
  glm::mat3 rotation_scale_matrix = glm::mat3(model);

  // 3. Transform axes (rotation only, normalize!)
  for (int i = 0; i < 3; ++i) {
    transformed_bounding_box.m_axes[i] = glm::normalize(rotation_scale_matrix * m_axes[i]);
  }

  // 4. Extract scale factors (length of basis vectors)
  glm::vec3 scale(glm::length(rotation_scale_matrix[0]), glm::length(rotation_scale_matrix[1]), glm::length(rotation_scale_matrix[2]));

  // 5. Scale extents
  transformed_bounding_box.m_extents = m_extents * scale;

  return transformed_bounding_box;
}

std::vector<glm::vec3> OOBB::getCorners() const {
  std::vector<glm::vec3> vertices;
  vertices.reserve(8);

  glm::vec3 ex = m_axes[0] * m_extents.x;
  glm::vec3 ey = m_axes[1] * m_extents.y;
  glm::vec3 ez = m_axes[2] * m_extents.z;

  vertices.push_back(m_center + ex + ey + ez); // 0
  vertices.push_back(m_center + ex + ey - ez); // 1
  vertices.push_back(m_center + ex - ey + ez); // 2
  vertices.push_back(m_center + ex - ey - ez); // 3
  vertices.push_back(m_center - ex + ey + ez); // 4
  vertices.push_back(m_center - ex + ey - ez); // 5
  vertices.push_back(m_center - ex - ey + ez); // 6
  vertices.push_back(m_center - ex - ey - ez); // 7

  return vertices;
}

std::vector<uint16_t> OOBB::getLineIndices() const {
  return {// +X face
          0, 2, 3, 3, 1, 0,
          // -X face
          4, 5, 7, 7, 6, 4,
          // +Y face
          0, 1, 5, 5, 4, 0,
          // -Y face
          2, 6, 7, 7, 3, 2,
          // +Z face
          0, 4, 6, 6, 2, 0,
          // -Z face
          1, 3, 7, 7, 5, 1};
}

bool OOBB::intersects(OOBB &other) {
  constexpr float EPS = 1e-8f;

  // Center-to-center vector
  glm::vec3 center_to_center_vector = other.getCenter() - m_center;

  // Convenience access
  const glm::mat3 &other_axes = other.getAxes();

  // Lambda used to test whether a given axis is a separating axis
  // between two oriented bounding boxes (SAT test).
  //
  // If the projections of the two OOBBs onto this axis do NOT overlap,
  // then this axis separates the boxes and they do not intersect.
  auto isSeparatingAxis = [](glm::vec3 &axis, OOBB &self, OOBB &other, glm::vec3 center_to_center) {
    // Helper lambda that computes the projection "radius" of an OOBB
    // onto the given axis.
    //
    // Conceptually:
    //  - Each OOBB is projected onto the axis
    //  - The projection is a 1D interval centered at the box center
    //  - This function computes half the length of that interval
    //
    // The radius is the sum of each local axis contribution,
    // weighted by the box's half-extents.
    auto projectedRadius = [](OOBB &box, glm::vec3 &axis) {
      return box.getExtents().x * fabs(glm::dot(axis, box.getAxes()[0])) + box.getExtents().y * fabs(glm::dot(axis, box.getAxes()[1])) +
             box.getExtents().z * fabs(glm::dot(axis, box.getAxes()[2]));
    };

    // Project both boxes onto the axis and compute their radii
    float radius = projectedRadius(self, axis);
    float other_radius = projectedRadius(other, axis);

    // Project the center-to-center vector onto the axis.
    // This gives the distance between the two projection centers
    // along this axis.
    float center_distance = fabs(glm::dot(center_to_center, axis));

    // If the distance between centers is greater than the sum of
    // the projection radii, the intervals do not overlap.
    //
    // This means the axis separates the two OOBBs.
    return center_distance > radius + other_radius;
  };

  // Test the current bounding box axes
  for (int i = 0; i < 3; ++i) {
    glm::vec3 current_axis = m_axes[i];
    if (isSeparatingAxis(current_axis, *this, other, center_to_center_vector)) {
      return false;
    }
  }

  // Test the axes of the other bounding box
  for (int i = 0; i < 3; ++i) {
    glm::vec3 other_current_axis = other_axes[i];
    if (isSeparatingAxis(other_current_axis, *this, other, center_to_center_vector)) {
      return false;
    }
  }

  // Test cross-product axes
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      // Compute cross-product
      glm::vec3 cross_product = glm::cross(m_axes[i], other_axes[j]);
      if (glm::length2(cross_product) < EPS) {
        continue; // parallel axes
      }

      // Normalize
      cross_product = glm::normalize(cross_product);

      if (isSeparatingAxis(cross_product, *this, other, center_to_center_vector)) {
        return false;
      }
    }
  }

  // No separating axis found
  return true;
}

bool OOBB::intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance) {
  constexpr float EPS = 1e-8f;

  // Transform ray into OOBB local space
  // OOBB local space:
  //  - box center at (0,0,0)
  //  - axes aligned with XYZ
  //  - extents = m_extents
  glm::mat3 rotation_transposed = glm::transpose(m_axes);
  glm::vec3 local_origin = rotation_transposed * (line_start - m_center);
  glm::vec3 local_direction = rotation_transposed * line_direction;

  // Ray vs AABB (slab test)
  //
  // A ray can be written as P(t) = local_origin + t * local_direction
  // For each axis i ∈ (X, Y, Z), the AABB defines a slab -m_extents[i] ≤ P_i(t) ≤ +m_extents[i]
  float intersection_min = 0.0f;
  float intersection_max = std::numeric_limits<float>::max();

  for (int i = 0; i < 3; ++i) {
    if (fabs(local_direction[i]) < EPS) {
      // Ray parallel to slab. If the origin is outside the slab => no intersection, can
      // return false directly.
      if (local_origin[i] < -m_extents[i] || local_origin[i] > m_extents[i]) {
        return false;
      }
    } else {

      float inverse_direction = 1.0f / local_direction[i];

      // t_near is the intersection with the near plane, t_far is the intersection with the far plane.
      float t_near = (-m_extents[i] - local_origin[i]) * inverse_direction;
      float t_far = (m_extents[i] - local_origin[i]) * inverse_direction;

      // Might need to swap depending on the ray direction
      if (t_near > t_far) {
        std::swap(t_near, t_far);
      }

      // Merge with global computed interval
      intersection_min = std::max(intersection_min, t_near);
      intersection_max = std::min(intersection_max, t_far);

      // If the min value is larger than the max value, the intervals do not
      // overlap and we can directly return false
      if (intersection_min > intersection_max) {
        return false;
      }
    }
  }

  // Output intersection distance
  *out_distance = intersection_min;
  return true;
}
