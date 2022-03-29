#ifndef LIMAP_TRIANGULATION_FUNCTIONS_H_
#define LIMAP_TRIANGULATION_FUNCTIONS_H_

#include "util/types.h"
#include "base/linebase.h"
#include "base/camera.h"
#include <tuple>

namespace limap {

namespace triangulation {

bool test_line_inside_ranges(const Line3d& line, const std::pair<V3D, V3D>& ranges);

V3D getNormalDirection(const Line2d& l, const CameraView& view);

V3D getDirectionFromVP(const V3D& vp, const CameraView& view);

// weak epipolar constraints
// intersect epipolar lines with the matched line on image 2
double compute_epipolar_IoU(const Line2d& l1, const CameraView& view1,
                            const Line2d& l2, const CameraView& view2);

// point triangulation
V3D point_triangulation(const V2D& p1, const CameraView& view1,
                        const V2D& p2, const CameraView& view2);

// Triangulating endpoints for triangulation
Line3d triangulate_endpoints(const Line2d& l1, const CameraView& view1,
                                  const Line2d& l2, const CameraView& view2);

// Asymmetric perspective to (view1, l1)
// Triangulation by plane intersection
Line3d triangulate(const Line2d& l1, const CameraView& view1,
                   const Line2d& l2, const CameraView& view2);

// Asymmetric perspective to (view1, l1)
// Triangulation with known direction
Line3d triangulate_with_direction(const Line2d& l1, const CameraView& view1,
                                  const Line2d& l2, const CameraView& view2,
                                  const V3D& direction);

} // namespace triangulation

} // namespace limap

#endif
