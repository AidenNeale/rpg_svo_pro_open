#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/synchronizer.h>
#include <svo/common/camera.h>
#include <svo/common/conversions.h>
#include <svo/common/frame.h>
#include <svo/direct/depth_filter.h>
#include <svo/frame_handler_array.h>
#include <svo/frame_handler_mono.h>
#include <svo/frame_handler_stereo.h>
#include <svo/imu_handler.h>
#include <svo/initialization.h>
#include <svo/map.h>
#include <svo_ros/ceres_backend_factory.h>
#include <svo_ros/svo_factory.h>
#include <svo_ros/svo_interface.h>
#include <svo_ros/visualizer.h>
#include <vikit/params_helper.h>
#include <vikit/timer.h>

#include <cv_bridge/cv_bridge.hpp>
#include <functional>
#include <image_transport/subscriber_filter.hpp>
#include <sensor_msgs/image_encodings.hpp>

#ifdef SVO_USE_GTSAM_BACKEND
#include <svo/backend/backend_interface.h>
#include <svo/backend/backend_optimizer.h>
#include <svo_ros/backend_factory.h>
#endif

#ifdef SVO_LOOP_CLOSING
#include <svo/online_loopclosing/loop_closing.h>
#endif

#ifdef SVO_GLOBAL_MAP
#include <svo/global_map.h>
#endif

namespace svo {

SvoInterface::SvoInterface(const PipelineType& pipeline_type, std::shared_ptr<rclcpp::Node> nh)
    : nh_(nh),
      pipeline_type_(pipeline_type),
      set_initial_attitude_from_gravity_(
          vk::param<bool>(nh_, "set_initial_attitude_from_gravity", true)),
      automatic_reinitialization_(vk::param<bool>(nh_, "automatic_reinitialization", false)) {
  nh_->declare_parameter<std::string>("cam0_topic", "/default_cam0_topic");
  std::string cam0_topic = nh_->get_parameter("cam0_topic").as_string();
  RCLCPP_INFO(nh_->get_logger(), "cam0_topic parameter: %s", cam0_topic.c_str());
  switch (pipeline_type) {
    case PipelineType::kMono:
      svo_ = factory::makeMono(nh_);
      break;
    case PipelineType::kStereo:
      svo_ = factory::makeStereo(nh_);
      break;
    case PipelineType::kArray:
      svo_ = factory::makeArray(nh_);
      break;
    default:
      LOG(FATAL) << "Unknown pipeline";
      break;
  }
  ncam_ = svo_->getNCamera();

  visualizer_.reset(new Visualizer(svo_->options_.trace_dir, nh_, ncam_->getNumCameras()));

  if (vk::param<bool>(nh_, "use_imu", false)) {
    imu_handler_ = factory::getImuHandler(nh_);
    svo_->imu_handler_ = imu_handler_;
  }

  if (vk::param<bool>(nh_, "use_ceres_backend", false)) {
    ceres_backend_interface_ = ceres_backend_factory::makeBackend(nh_, ncam_);
    if (imu_handler_) {
      svo_->setBundleAdjuster(ceres_backend_interface_);
      ceres_backend_interface_->setImu(imu_handler_);
      ceres_backend_interface_->makePublisher(nh_, ceres_backend_publisher_);
    } else {
      SVO_ERROR_STREAM("Cannot use ceres backend without using imu");
    }
  }
#ifdef SVO_USE_GTSAM_BACKEND
  if (vk::param<bool>(nh_, "use_backend", false)) {
    backend_interface_ = svo::backend_factory::makeBackend(nh_);
    ceres_backend_publisher_.reset(new CeresBackendPublisher(svo_->options_.trace_dir, nh_));
    svo_->setBundleAdjuster(backend_interface_);
    backend_interface_->imu_handler_ = imu_handler_;
  }
#endif
  if (vk::param<bool>(nh_, "runlc", false)) {
#ifdef SVO_LOOP_CLOSING
    LoopClosingPtr loop_closing_ptr = factory::getLoopClosingModule(nh_, svo_->getNCamera());
    svo_->lc_ = std::move(loop_closing_ptr);
    CHECK(svo_->depth_filter_->options_.extra_map_points)
        << "The depth filter seems to be initialized without extra map points.";
#else
    LOG(FATAL) << "You have to enable loop closing in svo_cmake.";
#endif
  }

  if (vk::param<bool>(nh_, "use_global_map", false)) {
#ifdef SVO_GLOBAL_MAP
    svo_->global_map_ = factory::getGlobalMap(nh_, svo_->getNCamera());
    if (imu_handler_) {
      svo_->global_map_->initializeIMUParams(imu_handler_->imu_calib_, imu_handler_->imu_init_);
    }
#else
    LOG(FATAL) << "You have to enable global map in cmake";
#endif
  }

  svo_->start();
  processing_thread_ = std::make_unique<std::thread>(&SvoInterface::monoLoop, this);
}

SvoInterface::~SvoInterface() { VLOG(1) << "Destructed SVO."; }

void SvoInterface::processImageBundle(const std::vector<cv::Mat>& images,
                                      const int64_t timestamp_nanoseconds) {
  if (!svo_->isBackendValid()) {
    if (vk::param<bool>(nh_, "use_ceres_backend", false, true)) {
      ceres_backend_interface_ = ceres_backend_factory::makeBackend(nh_, ncam_);
      if (imu_handler_) {
        svo_->setBundleAdjuster(ceres_backend_interface_);
        ceres_backend_interface_->setImu(imu_handler_);
        ceres_backend_interface_->makePublisher(nh_, ceres_backend_publisher_);
      } else {
        SVO_ERROR_STREAM("Cannot use ceres backend without using imu");
      }
    }
  }
  svo_->addImageBundle(images, timestamp_nanoseconds);
}

void SvoInterface::publishResults(const std::vector<cv::Mat>& images,
                                  const int64_t timestamp_nanoseconds) {
  CHECK_NOTNULL(svo_.get());
  CHECK_NOTNULL(visualizer_.get());

  visualizer_->img_caption_.clear();
  if (svo_->isBackendValid()) {
    std::string static_str = ceres_backend_interface_->getStationaryStatusStr();
    visualizer_->img_caption_ = static_str;
  }

  visualizer_->publishSvoInfo(svo_.get(), timestamp_nanoseconds);
  switch (svo_->stage()) {
    case Stage::kTracking: {
      Eigen::Matrix<double, 6, 6> covariance;
      covariance.setZero();
      visualizer_->publishImuPose(svo_->getLastFrames()->get_T_W_B(), covariance,
                                  timestamp_nanoseconds);
      visualizer_->publishCameraPoses(svo_->getLastFrames(), timestamp_nanoseconds);
      visualizer_->visualizeMarkers(svo_->getLastFrames(), svo_->closeKeyframes(), svo_->map());
      visualizer_->exportToDense(svo_->getLastFrames());
      bool draw_boundary = false;
      if (svo_->isBackendValid()) {
        draw_boundary = svo_->getBundleAdjuster()->isFixedToGlobalMap();
      }
      visualizer_->publishImagesWithFeatures(svo_->getLastFrames(), timestamp_nanoseconds,
                                             draw_boundary);
#ifdef SVO_LOOP_CLOSING
      // detections
      if (svo_->lc_) {
        visualizer_->publishLoopClosureInfo(svo_->lc_->cur_loop_check_viz_info_,
                                            std::string("loop_query"),
                                            Eigen::Vector3f(0.0f, 0.0f, 1.0f), 0.5);
        visualizer_->publishLoopClosureInfo(svo_->lc_->loop_detect_viz_info_,
                                            std::string("loop_detection"),
                                            Eigen::Vector3f(1.0f, 0.0f, 0.0f), 1.0);
        if (svo_->isBackendValid()) {
          visualizer_->publishLoopClosureInfo(svo_->lc_->loop_correction_viz_info_,
                                              std::string("loop_correction"),
                                              Eigen::Vector3f(0.0f, 1.0f, 0.0f), 3.0);
        }
        if (svo_->getLastFrames()->at(0)->isKeyframe()) {
          bool pc_recalculated = visualizer_->publishPoseGraph(
              svo_->lc_->kf_list_, svo_->lc_->need_to_update_pose_graph_viz_,
              static_cast<size_t>(svo_->lc_->options_.ignored_past_frames));
          if (pc_recalculated) {
            svo_->lc_->need_to_update_pose_graph_viz_ = false;
          }
        }
      }
#endif
#ifdef SVO_GLOBAL_MAP
      if (svo_->global_map_) {
        visualizer_->visualizeGlobalMap(*(svo_->global_map_), std::string("global_vis"),
                                        Eigen::Vector3f(0.0f, 0.0f, 1.0f), 0.3);
        visualizer_->visualizeFixedLandmarks(svo_->getLastFrames()->at(0));
      }
#endif
      break;
    }
    case Stage::kInitializing: {
      visualizer_->publishBundleFeatureTracks(svo_->initializer_->frames_ref_,
                                              svo_->getLastFrames(), timestamp_nanoseconds);
      break;
    }
    case Stage::kPaused:
    case Stage::kRelocalization:
      visualizer_->publishImages(images, timestamp_nanoseconds);
      break;
    default:
      LOG(FATAL) << "Unknown stage";
      break;
  }

#ifdef SVO_USE_GTSAM_BACKEND
  if (svo_->stage() == Stage::kTracking && backend_interface_) {
    if (svo_->getLastFrames()->isKeyframe()) {
      std::lock_guard<std::mutex> estimate_lock(backend_interface_->optimizer_->estimate_mut_);
      const gtsam::Values& state = backend_interface_->optimizer_->estimate_;
      ceres_backend_publisher_->visualizeFrames(state);
      if (backend_interface_->options_.add_imu_factors)
        ceres_backend_publisher_->visualizeVelocity(state);
      ceres_backend_publisher_->visualizePoints(state);
    }
  }
#endif
}

bool SvoInterface::setImuPrior(const int64_t timestamp_nanoseconds) {
  if (svo_->getBundleAdjuster()) {
    // if we use backend, this will take care of setting priors
    if (!svo_->hasStarted()) {
      // when starting up, make sure we already have IMU measurements
      if (imu_handler_->getMeasurementsCopy().size() < 10u) {
        return false;
      }
    }
    return true;
  }

  if (imu_handler_ && !svo_->hasStarted() && set_initial_attitude_from_gravity_) {
    // set initial orientation
    Quaternion R_imu_world;
    if (imu_handler_->getInitialAttitude(
            timestamp_nanoseconds * common::conversions::kNanoSecondsToSeconds, R_imu_world)) {
      VLOG(3) << "Set initial orientation from accelerometer measurements.";
      svo_->setRotationPrior(R_imu_world);
    } else {
      return false;
    }
  } else if (imu_handler_ && svo_->getLastFrames()) {
    // set incremental rotation prior
    Quaternion R_lastimu_newimu;
    if (imu_handler_->getRelativeRotationPrior(
            svo_->getLastFrames()->getMinTimestampNanoseconds() *
                common::conversions::kNanoSecondsToSeconds,
            timestamp_nanoseconds * common::conversions::kNanoSecondsToSeconds, false,
            R_lastimu_newimu)) {
      VLOG(3) << "Set incremental rotation prior from IMU.";
      svo_->setRotationIncrementPrior(R_lastimu_newimu);
    }
  }
  return true;
}

void SvoInterface::monoCallback(const sensor_msgs::msg::Image::ConstSharedPtr& msg) {
  if (idle_) return;

  cv::Mat image;
  try {
    image = cv_bridge::toCvCopy(msg)->image;
  } catch (cv_bridge::Exception& e) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("svo_interface"), "cv_bridge exception: " << e.what());
    return;
  }

  auto timestamp_ns = rclcpp::Time(msg->header.stamp).nanoseconds();

  // Push to a thread-safe queue for processing
  {
    std::lock_guard<std::mutex> lock(image_queue_mutex_);
    image_queue_.emplace(image.clone(), timestamp_ns);
  }
  image_queue_cv_.notify_one();
}

void SvoInterface::stereoCallback(const sensor_msgs::msg::Image::ConstSharedPtr& msg0,
                                  const sensor_msgs::msg::Image::ConstSharedPtr& msg1) {
  if (idle_) return;

  cv::Mat img0, img1;
  try {
    img0 = cv_bridge::toCvShare(msg0, "mono8")->image;
    img1 = cv_bridge::toCvShare(msg1, "mono8")->image;
  } catch (cv_bridge::Exception& e) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("svo_interface"), "cv_bridge exception: " << e.what());
  }
  auto timestamp_ns = rclcpp::Time(msg0->header.stamp).nanoseconds();

  if (!setImuPrior(timestamp_ns)) {
    VLOG(3) << "Could not align gravity! Attempting again in next iteration.";
    return;
  }

  imageCallbackPreprocessing(timestamp_ns);

  processImageBundle({img0, img1}, timestamp_ns);
  publishResults({img0, img1}, timestamp_ns);

  if (svo_->stage() == Stage::kPaused && automatic_reinitialization_) svo_->start();

  imageCallbackPostprocessing();
}

void SvoInterface::imuCallback(const sensor_msgs::msg::Imu::ConstSharedPtr& msg) {
  const Eigen::Vector3d omega_imu(msg->angular_velocity.x, msg->angular_velocity.y,
                                  msg->angular_velocity.z);
  const Eigen::Vector3d lin_acc_imu(msg->linear_acceleration.x, msg->linear_acceleration.y,
                                    msg->linear_acceleration.z);
  const ImuMeasurement m(rclcpp::Time(msg->header.stamp).seconds(), omega_imu, lin_acc_imu);
  if (imu_handler_) {
    imu_handler_->addImuMeasurement(m);
  } else {
    SVO_ERROR_STREAM("SvoNode has no ImuHandler");
  }
}

void SvoInterface::inputKeyCallback(const std_msgs::msg::String::ConstSharedPtr& key_input) {
  std::string remote_input = key_input->data;
  char input = remote_input.c_str()[0];
  switch (input) {
    case 'q':
      quit_ = true;
      SVO_INFO_STREAM("SVO user input: QUIT");
      break;
    case 'r':
      svo_->reset();
      idle_ = true;
      SVO_INFO_STREAM("SVO user input: RESET");
      break;
    case 's':
      svo_->start();
      idle_ = false;
      SVO_INFO_STREAM("SVO user input: START");
      break;
    case 'c':
      svo_->setCompensation(true);
      SVO_INFO_STREAM("Enabled affine compensation.");
      break;
    case 'C':
      svo_->setCompensation(false);
      SVO_INFO_STREAM("Disabled affine compensation.");
      break;
    default:;
  }
}

void SvoInterface::subscribeImu() {
  std::string imu_topic = vk::param<std::string>(nh_, "imu_topic", "imu");
  sub_imu_ = nh_->create_subscription<sensor_msgs::msg::Imu>(
      imu_topic, 100, std::bind(&svo::SvoInterface::imuCallback, this, std::placeholders::_1));
  sleep(3);
}

void SvoInterface::subscribeImage() {
  image_transport::ImageTransport it(nh_);
  if (pipeline_type_ == PipelineType::kMono) {
    std::string image_topic = vk::param<std::string>(nh_, "cam0_topic", "camera/image_raw");
    it_sub_ = std::make_shared<image_transport::Subscriber>(
        it.subscribe(image_topic, 5, &svo::SvoInterface::monoCallback, this));
  } else if (pipeline_type_ == PipelineType::kStereo) {
    image_thread_ = std::unique_ptr<std::thread>(new std::thread(&SvoInterface::stereoLoop, this));
  }
}

void SvoInterface::subscribeRemoteKey() {
  std::string remote_key_topic = vk::param<std::string>(nh_, "remote_key_topic", "svo/remote_key");
  sub_remote_key_ = nh_->create_subscription<std_msgs::msg::String>(
      remote_key_topic, 5,
      std::bind(&svo::SvoInterface::inputKeyCallback, this, std::placeholders::_1));
}

void SvoInterface::monoLoop() {
  SVO_INFO_STREAM("SvoNode: Started Image loop.");
  while (rclcpp::ok() && !quit_) {
    std::unique_lock<std::mutex> lock(image_queue_mutex_);
    image_queue_cv_.wait(lock, [this] { return !image_queue_.empty() || quit_; });
    if (quit_) {
      break;
    }

    auto [image, timestamp_ns] = image_queue_.front();
    image_queue_.pop();
    lock.unlock();

    if (!setImuPrior(timestamp_ns)) {
      VLOG(3) << "Could not align gravity! Attempting again in next iteration.";
      continue;
    }

    std::vector<cv::Mat> images{image};
    imageCallbackPreprocessing(timestamp_ns);
    processImageBundle(images, timestamp_ns);
    publishResults(images, timestamp_ns);

    if (svo_->stage() == Stage::kPaused && automatic_reinitialization_) {
      svo_->start();
    }

    imageCallbackPostprocessing();
  }
}

void SvoInterface::stereoLoop() {
  typedef message_filters::sync_policies::ExactTime<sensor_msgs::msg::Image,
                                                    sensor_msgs::msg::Image>
      ExactPolicy;
  typedef message_filters::Synchronizer<ExactPolicy> ExactSync;

  // subscribe to cam msgs
  std::string cam0_topic(vk::param<std::string>(nh_, "cam0_topic", "/cam0/image_raw"));
  std::string cam1_topic(vk::param<std::string>(nh_, "cam1_topic", "/cam1/image_raw"));
  image_transport::SubscriberFilter sub0(nh_.get(), cam0_topic, "raw");
  image_transport::SubscriberFilter sub1(nh_.get(), cam1_topic, "raw");
  ExactSync sync_sub(ExactPolicy(5), sub0, sub1);
  sync_sub.registerCallback(std::bind(&svo::SvoInterface::stereoCallback, this,
                                      std::placeholders::_1, std::placeholders::_2));

  while (rclcpp::ok() && !quit_) {
    rclcpp::spin_some(nh_);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

}  // namespace svo
