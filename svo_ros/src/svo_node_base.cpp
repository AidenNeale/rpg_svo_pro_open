#include "svo_ros/svo_node_base.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <svo/common/logging.h>
#include <vikit/params_helper.h>

#include <rclcpp/rclcpp.hpp>

namespace svo_ros {

void SvoNodeBase::initThirdParty(int argc, char **argv) { rclcpp::init(argc, argv); }

SvoNodeBase::SvoNodeBase()
    : nh_(std::make_shared<rclcpp::Node>("svo_node")),
      type_(vk::param<bool>(nh_, "pipeline_is_stereo", false) ? svo::PipelineType::kStereo
                                                              : svo::PipelineType::kMono),
      svo_interface_(std::make_unique<svo::SvoInterface>(type_, nh_)) {
  std::cout << "Creating node: svo_node" << std::endl;
  if (svo_interface_->imu_handler_) {
    svo_interface_->subscribeImu();
  }
  svo_interface_->subscribeImage();
  svo_interface_->subscribeRemoteKey();
}

void SvoNodeBase::run() {
  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(nh_);
  exec.spin();
  SVO_INFO_STREAM("SVO quit");
  svo_interface_->quit_ = true;
  SVO_INFO_STREAM("SVO terminated.\n");
}

}  // namespace svo_ros
