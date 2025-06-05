#pragma once

#include <memory>
#include <rclcpp/rclcpp.hpp>

namespace svo {

// forward declarations
class BackendInterface;

namespace backend_factory {

std::shared_ptr<BackendInterface> makeBackend(std::shared_ptr<rclcpp::Node> pnh);

}  // namespace backend_factory
}  // namespace svo
