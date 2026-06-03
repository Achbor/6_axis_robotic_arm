 /*
 * This program creates a ROS 2 node that moves a robot arm between target and home positions,
 * coordinating with gripper actions (open/close) at each position. The movement happens
 * in a continuous loop.
 *
 * Action Clients:
 *     /arm_controller/follow_joint_trajectory (control_msgs/FollowJointTrajectory):
 *         Commands for controlling arm joint positions
 *     /gripper_action_controller/gripper_cmd (control_msgs/GripperCommand):
 *         Commands for opening and closing the gripper
 */

 #include <chrono>
 #include <memory>
 #include <thread>
 #include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "control_msgs/action/follow_joint_trajectory.hpp"
#include "control_msgs/action/gripper_command.hpp"
#include "trajectory_msgs/msg/joint_trajectory_point.hpp"
#include "builtin_interfaces/msg/duration.hpp"

using namespace std::chrono_literals;

/*
* This class creates a ROS2 Node that:
* -> Moves arm to the target
* -> Closes the Gripper
* -> Moves the arm to the origin
* -> opens the gripper
* -> Repeats the steps all over
*/

class ArmGripperLoopController : public rclcpp::Node
{
    public:
        using FollowJointTrajectory = control_msgs::action::FollowJointTrajectory;
        using GripperCommand = control_msgs::action::GripperCommand;
    
        /*
        * Contructor to initialize the ArmGripperLoopController class.
        * It sets up ttthe two actions (For teh arm movements and for the Gripper movements)
        */

        ArmGripperLoopController() : Node("arm_gripper_loop_controller")
        {
            // SEtting up the two action clients
            arm_client_ = rclcpp_action::create_client<FollowJointTrajectory>(
                this, 
                "/arm_controller/follow_joint_trajectory" // action type
            );

            gripper_client_ = rclcpp_action::create_client<GripperCommand>(
                this,
                "/gripper_action_controller/gripper_cmd" //action type
            );

            // confirm if action servers are available (wait here if not)
            RCLCPP_INFO(this->get_logger(), "Waiting for action Serevrs...");
            if (!arm_client_->wait_for_action_server(20s)){
                RCLCPP_ERROR(this->get_logger(), "Arm Action servers not Found!");
                return;
            } 
             if (!gripper_client_->wait_for_action_server(20s)){
                RCLCPP_ERROR(this->get_logger(), "Gripper Action servers not Found!");
                return;
            } 
            RCLCPP_INFO(this->get_logger(), "Action Servers Found!!");

            // INitialize joint names
            joint_names_ = {
                "link1_to_link2", "link2_to_link3", "link3_to_link4",
                "link4_to_link5", "link5_to_link6", "link6_to_link6_flange"
            };

            //define joint positions in Radians
            target_pos_ = {1.345, -1.23, 0.246, -0.296, 0.389, -1.5}; 
            home_pos_ = {0, 0, 0, 0, 0, 0};

            //create timer for control loop
            timer_ = this->create_wall_timer(
                100ms,
                std::bind(&ArmGripperLoopController::controlLoopCallback, this)
            );
        }

        private:
            /* 
            * This is where the commands are sent for the movements of teh arm
            */

            void sendArmCommand(const std::vector<double>&positions){
                auto goal_msg = FollowJointTrajectory::Goal();
                goal_msg.trajectory.joint_names = joint_names_;

                trajectory_msgs::msg::JointTrajectoryPoint point;
                point.positions = positions;
                point.time_from_start.sec = 2; //setting the movement to take 2 secs in total
                goal_msg.trajectory.points.push_back(point);

                auto send_goal_options = rclcpp_action::Client<FollowJointTrajectory>::SendGoalOptions();
                arm_client_->async_send_goal(goal_msg, send_goal_options);
            }

            /* 
            * This is where the commands are sent for the movements of teh gripper a
            */
            void sendGripperCommand(double position){
                auto goal_msg = GripperCommand::Goal();
                goal_msg.command.position = position;
                goal_msg.command.max_effort = 5.0;

                auto send_goal_options = rclcpp_action::Client<GripperCommand>::SendGoalOptions();
                gripper_client_->async_send_goal(goal_msg, send_goal_options);
            }

            /*
            * Her is the execution flow of how commands will be sent as per design
            * First arm moves to target
            * Second then the gripper closes
            * then the arm moves to secondary point
            *Finally the gripper opens up and teh process repeats
            */

            void controlLoopCallback(){

                //Moving to target location or point
                RCLCPP_INFO(this->get_logger(), "Moving to target position");
                sendArmCommand(target_pos_); 
                std::this_thread::sleep_for(2500ms);

                // pause at target
                RCLCPP_INFO(this->get_logger(), "Reached the target position");                
                std::this_thread::sleep_for(1000ms);

                // Close the gripper
                RCLCPP_INFO(this->get_logger(), "Closing up the gripper");   
                sendGripperCommand(-0.7); //hardcoded closing value
                std::this_thread::sleep_for(500ms);
                
                //go to secondary position
                RCLCPP_INFO(this->get_logger(), "Moving to Secondary position");
                sendArmCommand(home_pos_);
                std::this_thread::sleep_for(2500ms);
                
                //Pause 
                RCLCPP_INFO(this->get_logger(), "Reached the secondary position");                
                std::this_thread::sleep_for(1000ms);

                // Open the gripper
                RCLCPP_INFO(this->get_logger(), "Closing uo the gripper");   
                sendGripperCommand(0.0); //hardcoded closing value
                std::this_thread::sleep_for(500ms);

                // final pause
                std::this_thread::sleep_for(1000ms);

            }

            // Action clients
            rclcpp_action::Client<FollowJointTrajectory>::SharedPtr arm_client_;
            rclcpp_action::Client<GripperCommand>::SharedPtr gripper_client_;

            // Timer
            rclcpp::TimerBase::SharedPtr timer_;

            // Joint names and positions
            std::vector<std::string> joint_names_;
            std::vector<double> target_pos_;
            std::vector<double>  home_pos_;
};

/*
Brief main function to implement the ROS@ formalities and run the arm_Gripper_loop_controll node
*/

int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ArmGripperLoopController>();

    try {
        rclcpp::spin(node);
    }catch (const std::exception &e){
        RCLCPP_ERROR(node->get_logger(), "Exception caught: %s", e.what());
    }

    rclcpp::shutdown();
    return 0;
}