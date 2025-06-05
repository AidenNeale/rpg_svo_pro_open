#ifndef ROS_PARAMS_HELPER_H_
#define ROS_PARAMS_HELPER_H_

#include <rclcpp/rclcpp.hpp>
#include <string>

namespace vk {

// Check if a parameter exists on the node
inline bool hasParam(std::shared_ptr<rclcpp::Node> node, const std::string& name) {
  return node->has_parameter(name);
}

// Get parameter with default value
template <typename T>
T getParam(std::shared_ptr<rclcpp::Node> node, const std::string& name, const T& defaultValue) {
  if (!hasParam(node, name)) {
    node->declare_parameter<T>(name, defaultValue);
  }
  T v = defaultValue;
  if (node->get_parameter(name, v)) {
    RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    return v;
  } else {
    RCLCPP_WARN_STREAM(node->get_logger(), "Cannot find value for parameter: "
                                               << name << ", assigning default: " << defaultValue);
    return defaultValue;
  }
}

// Get parameter, error if not found
template <typename T>
T getParam(std::shared_ptr<rclcpp::Node> node, const std::string& name) {
  if (!hasParam(node, name)) {
    node->declare_parameter<T>(name, T{});
  }
  T v;
  if (node->get_parameter(name, v)) {
    RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    return v;
  } else {
    RCLCPP_ERROR_STREAM(node->get_logger(), "Cannot find value for parameter: " << name);
    return T();
  }
}

// Optional: silent version
template <typename T>
T param(std::shared_ptr<rclcpp::Node> node, const std::string& name, const T& defaultValue,
        bool silent = false) {
  if (!hasParam(node, name)) {
    node->declare_parameter<T>(name, defaultValue);
  }
  T v = defaultValue;
  if (node->get_parameter(name, v)) {
    if (!silent) {
      RCLCPP_INFO_STREAM(node->get_logger(), "Found parameter: " << name << ", value: " << v);
    }
    return v;
  }
  if (!silent) {
    RCLCPP_WARN_STREAM(node->get_logger(), "Cannot find value for parameter: "
                                               << name << ", assigning default: " << defaultValue);
  }
  return defaultValue;
}

}  // namespace vk

#endif  // ROS_PARAMS_HELPER_H_