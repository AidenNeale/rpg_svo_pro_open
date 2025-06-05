#pragma once

#include <memory>

namespace svo {

// forward declarations
class SvoInterface;

class SvoNodelet : public rclcpp::Node {
 public:
  SvoNodelet(const rclcpp::NodeOptions& options);
  virtual ~SvoNodelet();

 private:
  virtual void onInit();

  std::unique_ptr<SvoInterface> svo_interface_;
};

}  // namespace svo
