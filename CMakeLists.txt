cmake_minimum_required(VERSION 3.16)
project(umrt-drivetrain-ros)

set(CMAKE_CXX_STANDARD 14)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD_REQUIRED On)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra -fPIC)
endif()

# find dependencies
set(THIS_PACKAGE_INCLUDE_DEPENDS
  hardware_interface
  pluginlib
  rclcpp
  rclcpp_lifecycle
  ros2_socketcan
)

# find dependencies
find_package(ament_cmake REQUIRED)
foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${Dependency} REQUIRED)
endforeach()

# Setup ros2_control_demo_description
install(
        DIRECTORY hardware/include/ros2_control_demo_description/diffbot/urdf hardware/include/ros2_control_demo_description/diffbot/rviz
        DESTINATION share/${PROJECT_NAME}/ros2_control_demo_description/diffbot
)
# EXAMPLE - we name them per year?
# install(
#         DIRECTORY hardware/include/ros2_control_demo_description/prairie_pioneer/urdf hardware/include/ros2_control_demo_description/prairie_pioneer/rviz
#         DESTINATION share/${PROJECT_NAME}/ros2_control_demo_description/prairie_pioneer
# )

## COMPILE
add_library(
  ${PROJECT_NAME} 
  SHARED
  hardware/diffbot_system.cpp
  # hardware/I2CPeripheral.cpp
  # hardware/PCA9685.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/hardware/include>
$<INSTALL_INTERFACE:include/umrt-drivetrain-ros>
)
 
# target_link_libraries(${PROJECT_NAME} 
#         PUBLIC
#         i2c
# )

ament_target_dependencies(
  ${PROJECT_NAME} PUBLIC
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

# GOT FROM umrt-arm-ros-firmware, might need it in the future if we are seperating the drivetrain libraries for CAN stuff
# ament_target_dependencies doesn't use namespaces when linking, so we need to link umrt-arm-firmware-lib ourself
# target_link_libraries(umrt-arm-ros-firmware PRIVATE umrt-arm-firmware-lib::umrt-arm-firmware-lib)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "umrt-drivetrain-ros_BUILDING_DLL")

# Export hardware plugins
pluginlib_export_plugin_description_file(hardware_interface umrt-drivetrain-ros.xml)

# Parse version number from the file
file(READ "version" version_input)
string(REGEX MATCH "([0-9]+)\.([0-9]+)\.([0-9]+)" _ ${version_input})

# Fill version number into package.xml
configure_file(
        package.in.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
        @ONLY
)

# INSTALL
install(
  DIRECTORY hardware/include/
  DESTINATION include/${PROJECT_NAME} 
)
install(
  DIRECTORY description/launch description/ros2_control description/urdf
  DESTINATION share/${PROJECT_NAME} 
)
install(
  DIRECTORY bringup/launch bringup/config
  DESTINATION share/${PROJECT_NAME} 
)
install(TARGETS ${PROJECT_NAME}
  EXPORT export_umrt-drivetrain-ros
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

## EXPORTS
ament_export_targets(export_umrt-drivetrain-ros HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})
ament_package()
