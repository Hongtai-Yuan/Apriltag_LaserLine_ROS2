#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/string.hpp"
using std::placeholders::_1;
using namespace std::chrono_literals;


class LaserStopNode : public rclcpp::Node {
  bool laser_flag = false;

public:
  LaserStopNode() : Node("lader_subscriber") {
    laser_subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10, std::bind(&LaserStopNode::topic_callback, this, _1));
    stopnode_publisher_ = this->create_publisher<std_msgs::msg::String>("stop_node", 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&LaserStopNode::timer_callback, this));
  }
  

private:
  void topic_callback(const sensor_msgs::msg::LaserScan &msg) {
    laser_flag = false;
    for (size_t i = 0; i < msg.ranges.size(); ++i) {
      float angle = (msg.angle_min + i * msg.angle_increment) * 180 / M_PI;
      if (abs(angle) > 90 && msg.ranges[i] < 0.3) {
        laser_flag = true;
        break;
      }
    }
  }

  void timer_callback() {
    auto message = std_msgs::msg::String();
    if (laser_flag) {
      message.data = "laser_stop";
      stopnode_publisher_->publish(message);
    }else{
      message.data = "no_people";
      stopnode_publisher_->publish(message);
    }
    RCLCPP_INFO(this->get_logger(), "message: %s", message.data.c_str());
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_subscription_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr stopnode_publisher_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LaserStopNode>());
  rclcpp::shutdown();
  return 0;
}