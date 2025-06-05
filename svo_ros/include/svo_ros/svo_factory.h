#pragma once

#include <svo/common/camera_fwd.h>

#include <memory>
#include <rclcpp/rclcpp.hpp>

namespace svo {

// forward declarations
class ImuHandler;
class LoopClosing;
class GlobalMap;
class FrameHandlerMono;
class FrameHandlerStereo;
class FrameHandlerArray;
class FrameHandlerDenseMono;

namespace factory {

/// Get IMU Handler.
std::shared_ptr<ImuHandler> getImuHandler(std::shared_ptr<rclcpp::Node> pnh);

#ifdef SVO_LOOP_CLOSING
/// Create loop closing module
std::shared_ptr<LoopClosing> getLoopClosingModule(std::shared_ptr<rclcpp::Node> pnh,
                                                  const CameraBundlePtr& cam = nullptr);
#endif

#ifdef SVO_GLOBAL_MAP
std::shared_ptr<GlobalMap> getGlobalMap(std::shared_ptr<rclcpp::Node> pnh,
                                        const CameraBundlePtr& ncams = nullptr);
#endif

/// Factory for Mono-SVO.
std::shared_ptr<FrameHandlerMono> makeMono(std::shared_ptr<rclcpp::Node> pnh,
                                           const CameraBundlePtr& cam = nullptr);

/// Factory for Stereo-SVO.
std::shared_ptr<FrameHandlerStereo> makeStereo(std::shared_ptr<rclcpp::Node> pnh,
                                               const CameraBundlePtr& cam = nullptr);

/// Factory for Camera-Array-SVO.
std::shared_ptr<FrameHandlerArray> makeArray(std::shared_ptr<rclcpp::Node> pnh,
                                             const CameraBundlePtr& cam = nullptr);

/// Factory for Camera-Array-SVO
std::shared_ptr<FrameHandlerDenseMono> makeDenseMono(std::shared_ptr<rclcpp::Node> pnh);

}  // namespace factory
}  // namespace svo
