#include "vikit/cameras/camera_geometry_base.h"

#include <aslam/common/yaml-serialization.h>
#include <vikit/cameras/yaml/camera-yaml-serialization.h>
#include <vikit/path_utils.h>

#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <utility>

namespace vk {
namespace cameras {

CameraGeometryBase::CameraGeometryBase(const int width, const int height)
    : width_(width), height_(height) {}

CameraGeometryBase::Ptr CameraGeometryBase::loadFromYaml(const std::string& yaml_file) {
  try {
    YAML::Node doc = YAML::LoadFile(yaml_file.c_str());
    CameraGeometryBase::Ptr cam = doc.as<CameraGeometryBase::Ptr>();

    std::string basename = vk::path_utils::getBaseName(yaml_file);
    if (basename.empty()) {
      return cam;
    }
    const YAML::Node& mask = doc["mask"];
    if (mask) {
      cam->loadMask(basename + "/" + mask.as<std::string>());
    }
    return cam;
  } catch (const std::exception& ex) {
    std::cerr << "Failed to load Camera from file " << yaml_file << " with the error: \n"
              << ex.what();
  }
  // Return nullptr in the failure case.
  return CameraGeometryBase::Ptr();
}

void CameraGeometryBase::backProject3(const Eigen::Ref<const Eigen::Matrix2Xd>& keypoints,
                                      Eigen::Matrix3Xd* out_bearing_vectors,
                                      std::vector<bool>* success) const {
  const int num_keypoints = keypoints.cols();
  if (!out_bearing_vectors) {
    throw std::runtime_error("CameraGeometryBase::backProject3: out_bearing_vectors is null");
  }
  if (!success) {
    throw std::runtime_error("CameraGeometryBase::backProject3: success is null");
  }
  out_bearing_vectors->resize(Eigen::NoChange, num_keypoints);
  success->resize(num_keypoints);

  for (int i = 0; i < num_keypoints; ++i) {
    Eigen::Vector3d bearing_vector;
    (*success)[i] = backProject3(keypoints.col(i), &bearing_vector);
    out_bearing_vectors->col(i) = bearing_vector;
  }
}

void CameraGeometryBase::setMask(const cv::Mat& mask) {
  if (height_ != mask.rows || width_ != mask.cols) {
    throw std::runtime_error("CameraGeometryBase::setMask: Mask size does not match camera size.");
  }
  if (mask.type() != CV_8UC1) {
    throw std::runtime_error("CameraGeometryBase::setMask: Mask type must be CV_8UC1.");
  }
  mask_ = mask;
}

void CameraGeometryBase::loadMask(const std::string& mask_file) {
  cv::Mat mask(cv::imread(mask_file, 0));
  if (mask.data)
    setMask(mask);
  else
    std::cerr << "Unable to load mask file.";
}

bool CameraGeometryBase::isMasked(const Eigen::Ref<const Eigen::Vector2d>& keypoint) const {
  return keypoint[0] < 0.0 || keypoint[0] >= static_cast<double>(width_) || keypoint[1] < 0.0 ||
         keypoint[1] >= static_cast<double>(height_) ||
         (!mask_.empty() &&
          mask_.at<uint8_t>(static_cast<int>(keypoint[1]), static_cast<int>(keypoint[0])) == 0);
}

Eigen::Vector2d CameraGeometryBase::createRandomKeypoint() const {
  Eigen::Vector2d out;
  do {
    out.setRandom();
    out(0) = std::abs(out(0)) * imageWidth();
    out(1) = std::abs(out(1)) * imageHeight();
  } while (isMasked(out));

  return out;
}

}  // namespace cameras
}  // namespace vk
