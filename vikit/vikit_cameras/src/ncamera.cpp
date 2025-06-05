#include <aslam/common/yaml-serialization.h>
#include <vikit/cameras/camera_geometry_base.h>
#include <vikit/cameras/ncamera.h>
#include <vikit/cameras/yaml/ncamera-yaml-serialization.h>
#include <vikit/path_utils.h>

#include <string>
#include <utility>

namespace vk {
namespace cameras {

NCamera::NCamera(const TransformationVector& T_C_B, const std::vector<Camera::Ptr>& cameras,
                 const std::string& label)
    : T_C_B_(T_C_B), cameras_(cameras), label_(label) {
  initInternal();
}

NCamera::Ptr NCamera::loadFromYaml(const std::string& yaml_file) {
  try {
    YAML::Node doc = YAML::LoadFile(yaml_file.c_str());
    NCamera::Ptr ncam = doc.as<NCamera::Ptr>();

    std::string basename = vk::path_utils::getBaseName(yaml_file);
    if (basename.empty()) {
      return ncam;
    }

    const YAML::Node& cameras_node = doc["cameras"];
    if (cameras_node.size() != ncam->numCameras()) {
      throw std::runtime_error(
          "NCamera::loadFromYaml: Number of cameras in YAML file does not match the number of "
          "cameras in NCamera.");
    }
    for (size_t i = 0; i < cameras_node.size(); i++) {
      const YAML::Node& cam_node = (cameras_node[i])["camera"];
      const YAML::Node& mask = cam_node["mask"];
      Camera::Ptr cam = ncam->getCameraShared(i);
      if (mask) {
        cam->loadMask(basename + "/" + mask.as<std::string>());
      }
    }

    return ncam;
  } catch (const std::exception& ex) {
    std::cerr << "Failed to load NCamera from file " << yaml_file << " with the error: \n"
              << ex.what();
  }
  // Return nullptr in the failure case.
  return NCamera::Ptr();
}

void NCamera::initInternal() {
  if (cameras_.size() != T_C_B_.size()) {
    throw std::runtime_error(
        "NCamera::initInternal: Number of cameras does not match the number of transformations.");
  }
  for (size_t i = 0; i < cameras_.size(); ++i) {
    if (!cameras_[i]) {
      throw std::runtime_error("NCamera::initInternal: Camera pointer is null.");
    }
  }
}

const Transformation& NCamera::get_T_C_B(size_t camera_index) const {
  if (camera_index >= cameras_.size()) {
    throw std::out_of_range("NCamera::get_T_C_B: camera_index out of range.");
  }
  return T_C_B_[camera_index];
}

const TransformationVector& NCamera::getTransformationVector() const { return T_C_B_; }

const Camera& NCamera::getCamera(size_t camera_index) const {
  if (camera_index >= cameras_.size()) {
    throw std::out_of_range("NCamera::get_T_C_B: camera_index out of range.");
  }
  if (!cameras_[camera_index]) {
    throw std::runtime_error("NCamera::getCamera: Camera pointer is null.");
  }
  return *cameras_[camera_index];
}

Camera::Ptr NCamera::getCameraShared(size_t camera_index) {
  if (camera_index >= cameras_.size()) {
    throw std::out_of_range("NCamera::get_T_C_B: camera_index out of range.");
  }
  return cameras_[camera_index];
}

Camera::ConstPtr NCamera::getCameraShared(size_t camera_index) const {
  if (camera_index >= cameras_.size()) {
    throw std::out_of_range("NCamera::get_T_C_B: camera_index out of range.");
  }
  return cameras_[camera_index];
}

void NCamera::printParameters(std::ostream& out, const std::string& s) const {
  out << s << std::endl;
  for (size_t i = 0; i < cameras_.size(); ++i) {
    out << "Camera #" << i << std::endl;
    cameras_[i]->printParameters(out, "");
    out << "  T_C_B = " << T_C_B_.at(i) << std::endl;
  }
}

const std::vector<Camera::Ptr>& NCamera::getCameraVector() const { return cameras_; }

}  // namespace cameras
}  // namespace vk
