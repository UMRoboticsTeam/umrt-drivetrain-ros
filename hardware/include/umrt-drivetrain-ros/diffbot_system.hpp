// Copyright 2021 ros2_control Development Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UMRT_DRIVETRAIN_ROS_DIFFBOT_SYSTEM_HPP_
#define UMRT_DRIVETRAIN_ROS_DIFFBOT_SYSTEM_HPP_

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

#include "umrt-drivetrain-ros/visibility_control.h"

#include <ros2_socketcan/socket_can_sender.hpp>

#include "umrt-drivetrain-ros/wheel_info.hpp"

// This is a hacky workaround to get velocity commands working on the robot. we
// are provided per-wheel angular velocitites, which we convert to a +/- 1 m/s
// velocity internally, and then scale that to a pulse width between 1 and 2 ms.
// See wiki.ros.org/diff_drive_controller Section 1.4. -njreichert
// #define WHEEL_RADIUS 0.1143 // m

namespace umrt_drivetrain_ros
{
class DiffBotSystemHardware : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(DiffBotSystemHardware);

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::return_type write(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  UMRT_DRIVETRAIN_ROS_PUBLIC
  hardware_interface::CallbackReturn on_error(
    const rclcpp_lifecycle::State &previous_state) override;

private:
  hardware_interface::return_type set_can_wheel_speed(int channel, double angular_speed);
  hardware_interface::return_type try_to_reset_wheels();


  // Parameters for the DiffBot simulation
  double hw_start_sec_;
  double hw_stop_sec_;

  // This is a hacky workaround to get velocity commands working on the robot. we
  // are provided per-wheel angular velocitites, which we convert to a +/- 1 m/s
  // velocity internally, and then scale that to a pulse width between 1 and 2 ms.
  // See wiki.ros.org/diff_drive_controller Section 1.4. -njreichert
  double wheel_radius_;

  // Store the command for the simulated robot
  std::vector<WheelInfo> wheels;

  std::unique_ptr<drivers::socketcan::SocketCanSender> can_sender;

};

}  // namespace umrt_drivetrain_ros

#endif  // UMRT_DRIVETRAIN_ROS_DIFFBOT_SYSTEM_HPP_
