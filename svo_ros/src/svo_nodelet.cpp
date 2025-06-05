#include <svo_ros/svo_interface.h>
#include <svo_ros/svo_nodelet.h>
#include <vikit/params_helper.h>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(svo::SvoNodelet)
namespace svo {

SvoNodelet::SvoNodelet(const rclcpp::NodeOptions& options) : rclcpp::Node("svo_nodelet", options) {
  onInit();
}

SvoNodelet::~SvoNodelet() {
  RCLCPP_INFO_STREAM(this->get_logger(), "SVO quit");
  svo_interface_->quit_ = true;
}

void SvoNodelet::onInit() {
  RCLCPP_INFO_STREAM(this->get_logger(), "Initialized " << this->get_name() << " nodelet.");
  svo::PipelineType type = svo::PipelineType::kMono;
  if (vk::param<bool>(this->shared_from_this(), "pipeline_is_stereo", false))
    type = svo::PipelineType::kStereo;

  svo_interface_ = std::make_unique<SvoInterface>(type, this->shared_from_this());
  if (svo_interface_->imu_handler_) svo_interface_->subscribeImu();
  svo_interface_->subscribeImage();
  svo_interface_->subscribeRemoteKey();
}

}  // namespace svo
