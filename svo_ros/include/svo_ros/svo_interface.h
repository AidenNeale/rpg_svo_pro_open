#pragma once

#include <svo/common/camera_fwd.h>
#include <svo/common/transformation.h>
#include <svo/common/types.h>

#include <image_transport/image_transport.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <std_msgs/msg/string.hpp>  // user-input
#include <thread>

namespace svo {

// forward declarations
class FrameHandlerBase;
class Visualizer;
class ImuHandler;
class BackendInterface;
class CeresBackendInterface;
class CeresBackendPublisher;

enum class PipelineType { kMono, kStereo, kArray };

/// SVO Interface
class SvoInterface {
 public:
  // ROS subscription and publishing.
  std::shared_ptr<rclcpp::Node> nh_;
  PipelineType pipeline_type_;
  std::shared_ptr<rclcpp::Subscription<std_msgs::msg::String>> sub_remote_key_;
  std::shared_ptr<rclcpp::Subscription<sensor_msgs::msg::Imu>> sub_imu_;
  std::string remote_input_;
  std::unique_ptr<std::thread> imu_thread_;
  std::unique_ptr<std::thread> image_thread_;
  std::shared_ptr<image_transport::Subscriber> it_sub_;

  // SVO modules.
  std::shared_ptr<FrameHandlerBase> svo_;
  std::shared_ptr<Visualizer> visualizer_;
  std::shared_ptr<ImuHandler> imu_handler_;
  std::shared_ptr<BackendInterface> backend_interface_;
  std::shared_ptr<CeresBackendInterface> ceres_backend_interface_;
  std::shared_ptr<CeresBackendPublisher> ceres_backend_publisher_;

  CameraBundlePtr ncam_;

  std::queue<std::pair<cv::Mat, int64_t>> image_queue_;
  std::mutex image_queue_mutex_;
  std::condition_variable image_queue_cv_;
  std::unique_ptr<std::thread> processing_thread_;

  // Parameters
  bool set_initial_attitude_from_gravity_ = true;

  // System state.
  std::atomic<bool> quit_ = false;
  bool idle_ = false;
  bool automatic_reinitialization_ = false;

  SvoInterface(const PipelineType& pipeline_type, std::shared_ptr<rclcpp::Node> nh);

  virtual ~SvoInterface();

  // Processing
  void processImageBundle(const std::vector<cv::Mat>& images, int64_t timestamp_nanoseconds);

  bool setImuPrior(const int64_t timestamp_nanoseconds);

  void publishResults(const std::vector<cv::Mat>& images, const int64_t timestamp_nanoseconds);

  // Subscription and callbacks
  void monoCallback(const sensor_msgs::msg::Image::ConstSharedPtr& msg);
  void stereoCallback(const sensor_msgs::msg::Image::ConstSharedPtr& msg0,
                      const sensor_msgs::msg::Image::ConstSharedPtr& msg1);
  void imuCallback(const sensor_msgs::msg::Imu::ConstSharedPtr& imu_msg);
  void inputKeyCallback(const std_msgs::msg::String::ConstSharedPtr& key_input);

  // These functions are called before and after monoCallback or stereoCallback.
  // a derived class can implement some additional logic here.
  virtual void imageCallbackPreprocessing(int64_t timestamp_nanoseconds) {}
  virtual void imageCallbackPostprocessing() {}

  void subscribeImu();
  void subscribeImage();
  void subscribeRemoteKey();

  void monoLoop();
  void stereoLoop();
};

}  // namespace svo
