#pragma once

namespace vk {
namespace cameras {

template <typename DerivedKeyPoint>
bool CameraGeometryBase::isKeypointVisible(
    const Eigen::MatrixBase<DerivedKeyPoint>& keypoint) const {
  EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(DerivedKeyPoint, 2, 1);
  typedef typename DerivedKeyPoint::Scalar Scalar;
  return keypoint[0] >= static_cast<Scalar>(0.0) && keypoint[1] >= static_cast<Scalar>(0.0) &&
         keypoint[0] < static_cast<Scalar>(imageWidth()) &&
         keypoint[1] < static_cast<Scalar>(imageHeight());
}

template <typename DerivedKeyPoint>
bool CameraGeometryBase::isKeypointVisibleWithMargin(
    const Eigen::MatrixBase<DerivedKeyPoint>& keypoint,
    typename DerivedKeyPoint::Scalar margin) const {
  typedef typename DerivedKeyPoint::Scalar Scalar;
  EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(DerivedKeyPoint, 2, 1);
  if ((2 * margin) >= static_cast<Scalar>(imageWidth())) {
    throw std::runtime_error(
        "CameraGeometryBase::isKeypointVisibleWithMargin: margin is too large for image width");
  }
  if ((2 * margin) >= static_cast<Scalar>(imageHeight())) {
    throw std::runtime_error(
        "CameraGeometryBase::isKeypointVisibleWithMargin: margin is too large for image height");
  }

  return keypoint[0] >= margin && keypoint[1] >= margin &&
         keypoint[0] < (static_cast<Scalar>(imageWidth()) - margin) &&
         keypoint[1] < (static_cast<Scalar>(imageHeight()) - margin);
}

}  // namespace cameras
}  // namespace vk
