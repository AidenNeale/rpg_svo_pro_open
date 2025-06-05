/*
 * output_helper.h
 *
 *  Created on: Jan 20, 2013
 *      Author: cforster
 */

#pragma once

#include <kindr/minimal/quat-transformation.h>
#include <tf2_ros/transform_broadcaster.h>

#include <Eigen/Core>
#include <rclcpp/rclcpp.hpp>
#include <string>
#include <visualization_msgs/msg/marker.hpp>

namespace vk {

using Transformation = kindr::minimal::QuatTransformation;

namespace output_helper {

using namespace std;
using namespace Eigen;

void publishTfTransform(const Transformation& T, const rclcpp::Time& stamp, const string& frame_id,
                        const string& child_frame_id, tf2_ros::TransformBroadcaster& br);

void publishPointMarker(std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
                        const Vector3d& pos, const string& ns, const rclcpp::Time& timestamp,
                        int id, int action, double marker_scale, const Vector3d& color,
                        rclcpp::Duration lifetime = rclcpp::Duration::from_seconds(0.0));

void publishLineMarker(std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
                       const Vector3d& start, const Vector3d& end, const string& ns,
                       const rclcpp::Time& timestamp, int id, int action, double marker_scale,
                       const Vector3d& color,
                       rclcpp::Duration lifetime = rclcpp::Duration::from_seconds(0.0));

void publishArrowMarker(std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
                        const Vector3d& pos, const Vector3d& dir, double scale, const string& ns,
                        const rclcpp::Time& timestamp, int id, int action, double marker_scale,
                        const Vector3d& color);

void publishHexacopterMarker(
    std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub, const string& frame_id,
    const string& ns, const rclcpp::Time& timestamp, int id, int action, double marker_scale,
    const Vector3d& color);

void publishQuadrocopterMarkers(
    std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub, const string& frame_id,
    const string& ns, const rclcpp::Time& timestamp, int id, int action, double marker_scale,
    const Vector3d& color);

void publishCameraMarker(std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
                         const string& frame_id, const string& ns, const rclcpp::Time& timestamp,
                         int id, int action, double marker_scale, const Vector3d& color);

void publishFrameMarker(std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
                        const Matrix3d& rot, const Vector3d& pos, const string& ns,
                        const rclcpp::Time& timestamp, int id, int action, double marker_scale,
                        rclcpp::Duration lifetime = rclcpp::Duration::from_seconds(0.0));

void publishGtsamPoseCovariance(
    std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub,
    const Eigen::Vector3d& mean,
    const Eigen::Matrix3d& R_W_B,  // Body in World-Frame
    const Eigen::Matrix<double, 6, 6>& covariance, const string& ns, int id, int action,
    double sigma_scale, const Vector4d& color);

}  // namespace output_helper
}  // namespace vk
