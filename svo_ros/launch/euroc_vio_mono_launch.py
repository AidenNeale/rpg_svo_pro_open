import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    calib_file_arg = DeclareLaunchArgument(
        'calib_file',
        default_value=os.path.join(
            get_package_share_directory('svo_ros'),
            'param/calib/euroc_mono.yaml'
        ),
        description='Path to camera calibration file'
    )

    svo_node = Node(
        package='svo_ros',
        executable='svo_node',
        name='svo_node',
        output='screen',
        parameters=[
            os.path.join(get_package_share_directory('svo_ros'), 'param', 'vio_mono.yaml'),
            {'calib_file': LaunchConfiguration('calib_file')},
            {'cam0_topic': '/cam0/image_raw'},
            {'cam1_topic': '/cam1/image_raw'},
            {'imu_topic': '/imu0'},
            {'runlc': True}
        ],
        arguments=['--ros-args', '--log-level', 'info'],
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='vis',
        arguments=['-d', os.path.join(
            get_package_share_directory('svo_ros'),
            'rviz_config_vio.rviz'
        )],
    )

    return LaunchDescription([
        calib_file_arg,
        svo_node,
        rviz_node
    ])