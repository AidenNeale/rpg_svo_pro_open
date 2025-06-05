#pragma once

#include "svo_ros/svo_interface.h"

namespace svo_ros {

class SvoNodeBase {
 public:
  // Initializes glog, gflags and ROS.
  static void initThirdParty(int argc, char **argv);

  SvoNodeBase();

  void run();

 private:
  std::shared_ptr<rclcpp::Node> nh_;
  svo::PipelineType type_;

 public:
  std::unique_ptr<svo::SvoInterface> svo_interface_;
};

}  // namespace svo_ros
