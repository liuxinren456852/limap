#ifndef LIMAP_BASE_CAMERA_H
#define LIMAP_BASE_CAMERA_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <cmath>
#include <fstream>

namespace py = pybind11;

#include "util/types.h"
#include "_limap/helpers.h"

#include <colmap/base/camera.h>
#include <colmap/base/camera_models.h>
#include <colmap/base/pose.h>

namespace limap {

// colmap camera models:
// (0, SIMPLE_PINHOLE)
// (1, PINHOLE)
// (2, SIMPLE_RADIAL)
// (3, RADIAL)
// (4, OPENCV)
// (5, OPENCV_FISHEYE)
// (6, FULL_OPENCV)
// (7, FOV)
// (8, SIMPLE_RADIAL_FISHEYE)
// (9, RADIAL_FISHEYE)
// (10, THIN_PRISM_FISHEYE)

class Camera: public colmap::Camera {
public:
    Camera() {}
    Camera(const colmap::Camera& cam);
    Camera(int model_id, const std::vector<double>& params, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1));
    Camera(const std::string& model_name, const std::vector<double>& params, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1));
    // initialize with intrinsics
    Camera(M3D K, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1));
    Camera(int model_id, M3D K, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1));
    Camera(const std::string& model_name, M3D K, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1));
    Camera(py::dict dict);

    py::dict as_dict() const;
    void resize(const size_t width, const size_t height) { Rescale(width, height); }
    void set_max_image_dim(const int& val);
    M3D K() const { return CalibrationMatrix(); }
    M3D K_inv() const { return K().inverse(); }
    int h() const { return Height(); }
    int w() const { return Width(); }
    std::vector<double> params() const { return Params(); }
    
    double uncertainty(double depth, double var2d = 5.0) const;
};

class CameraPose {
public:
    CameraPose() {}
    CameraPose(V4D qqvec, V3D ttvec): qvec(qqvec), tvec(ttvec) {}
    CameraPose(M3D R, V3D T): tvec(T) { qvec = colmap::RotationMatrixToQuaternion(R); }
    CameraPose(py::dict dict);

    V4D qvec;
    V3D tvec;

    py::dict as_dict() const;
    M3D R() const { return colmap::QuaternionToRotationMatrix(qvec); }
    V3D T() const { return tvec; }

    V3D center() const { return -R().transpose() * T(); }
    double projdepth(const V3D& p3d) const;
};

class CameraView {
public:
    CameraView() {}
    CameraView(const Camera& input_cam, const CameraPose& input_pose): cam(input_cam), pose(input_pose) {}
    CameraView(py::dict dict);

    Camera cam;
    CameraPose pose;
    
    py::dict as_dict() const;
    M3D K() const { return cam.K(); }
    M3D K_inv() const { return cam.K_inv(); }
    int h() const { return cam.h(); }
    int w() const { return cam.w(); }
    M3D R() const { return pose.R(); }
    V3D T() const { return pose.T(); }

    V2D projection(const V3D& p3d) const;
    V3D ray_direction(const V2D& p2d) const;
};

// used for optimization
class MinimalPinholeCamera {
public:
    MinimalPinholeCamera() {}
    MinimalPinholeCamera(const CameraView& view);
    CameraView GetCameraView() const;

    V4D kvec; // [f1, f2, c1, c2]
    V4D qvec;
    V3D tvec;
    int height, width;
};

// interchanging between CameraView and MinimalPinholeCamera
MinimalPinholeCamera cam2minimalcam(const CameraView& view);

CameraView minimalcam2cam(const MinimalPinholeCamera& camera);

} // namespace limap

#endif
