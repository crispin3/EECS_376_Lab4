// example_action_client: 
// wsn, October, 2014

#include<ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <iostream>
#include <string>
#include <nav_msgs/Path.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>

#include <my_action_server/demo2Action.h>

bool g_lidar_alarm= false; 
bool g_goal_is_done= true;

geometry_msgs::PoseStamped pose_stamped;
geometry_msgs::Pose pose;

void doneCb(const actionlib::SimpleClientGoalState& state,
    const my_action_server::demo2ResultConstPtr& result) {
    ROS_INFO(" doneCb: server responded with state [%s]", state.toString().c_str());
    int diff = result->output - result->goal_stamp;
    ROS_INFO("got result output = %d; goal_stamp = %d; diff = %d", result->output, result->goal_stamp, diff);
    g_goal_is_done=true;
}

void alarmCallback(const std_msgs::Bool& alarm_msg) 
{ 
  g_lidar_alarm = alarm_msg.data; 
  if (g_lidar_alarm) {
     ROS_INFO("LIDAR alarm received!"); 
  }
} 


int main(int argc, char** argv) {
    ros::init(argc, argv, "my_action_client_node"); 
    int g_count = 0;
    ros::NodeHandle n; 
    ros::Subscriber alarm_subscriber = n.subscribe("lidar_alarm",1,alarmCallback); 
 
    my_action_server::demo2Goal goal;

    actionlib::SimpleActionClient<my_action_server::demo2Action> action_client("my_action_server", true);

   
    ROS_INFO("waiting for server: ");
    bool server_exists = action_client.waitForServer(ros::Duration(5.0));
    //bool server_exists = action_client.waitForServer(ros::Duration(5.0)); 

    if (!server_exists) {
        ROS_WARN("could not connect to server; halting");
        return 0; 
    }

    ROS_INFO("connected to action server"); 

    goal.finished = false;
    while (true) {
	ros::Duration(0.1).sleep();
	g_count++;
        goal.input = g_count; 
	int turn = 0;
	ros::spinOnce(); 
		if (turn == 0){
			goal.turn = 0;
			goal.howLong = 6;
			int turn = 1;
			g_goal_is_done = false;
			
		}
		else if(turn == 1) {
		goal.turn = 1;
		goal.howLong = 3; 
		g_goal_is_done = false;
		turn = 0;
}	
		
        if (!g_goal_is_done) {
            ros::spinOnce();
            ros::Duration(0.1).sleep();
            if (g_lidar_alarm) {
	       goal.halt = true;
	}
		else
	       goal.halt = false;
}
	ROS_INFO("goal halt =: %d goal finshed is =: %d ", goal.halt, goal.finished);
        g_goal_is_done=false;
        action_client.sendGoal(goal, &doneCb); 

  
        bool finished_before_timeout = action_client.waitForResult(ros::Duration(5.0));
        if (!finished_before_timeout) {
            ROS_WARN("giving up waiting on result for goal number %d", g_count);
            return 0;
        } else {
        }

    }

    return 0;
}

