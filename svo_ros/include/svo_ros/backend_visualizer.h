// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).

#pragma once

#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>
#include <svo/backend/smart_factors_fwd.h>
#include <svo/common/transformation.h>

#include <Eigen/Core>
#include <fstream>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <string>

namespace swe {
class SweGtsamBookeeper;
}
namespace svo {

/// Publish visualisation messages to ROS.
class CeresBackendPublisher {
 public:
  typedef std::shared_ptr<CeresBackendPublisher> Ptr;
  typedef pcl::PointXYZ PointType;
  typedef pcl::PointCloud<PointType> PointCloud;

  static const std::string kWorldFrame;

  std::shared_ptr<rclcpp::Node> pnh_;
  size_t trace_id_;
  std::string trace_dir_;
  std::shared_ptr<rclcpp::Publisher<visualization_msgs::msg::Marker>> pub_markers_;
  std::shared_ptr<rclcpp::Publisher<sensor_msgs::msg::PointCloud2>> pub_pc_;
  double vis_scale_;
  std::ofstream ofs_states_;
  std::ofstream ofs_covariance_;

  CeresBackendPublisher(const std::string& trace_dir, std::shared_ptr<rclcpp::Node> pnh);

  ~CeresBackendPublisher() = default;

  void visualizeFrames(const gtsam::Values& values);

  void visualizePoints(const gtsam::Values& values);

  void publishPointcloud(const gtsam::Values& values);

  void visualizeSmartFactors(const SmartFactorMap& smart_factors);

  void visualizeVelocity(const gtsam::Values& values);

  void visualizePoseCovariance(const Transformation& T_W_B,
                               const Eigen::Matrix<double, 6, 6>& covariance);

  void tracePose(std::map<int, int64_t, std::less<int>> frameid_timestamp_map,
                 const gtsam::Values& values);

  void traceStates(const gtsam::Values& values);

  void traceCovariance(const Eigen::Matrix<double, 6, 6>& C, const int64_t timestamp);
};

}  // end namespace svo
