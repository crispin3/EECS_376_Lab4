// example_action_server: a simple action server
// Wyatt Newman

#include<ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include<my_action_server/demo2Action.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Twist.h>

int g_count = 0;
bool g_count_failure = false;
double sample_dt = 0.01;
geometry_msgs::Twist twist_cmd;

class ExampleActionServer {
private:
    ros::NodeHandle nh_;
    ros::Publisher twist_commander = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    actionlib::SimpleActionServer<my_action_server::demo2Action> as_;
    my_action_server::demo2Goal goal_; 
    my_action_server::demo2Result result_; 
    my_action_server::demo2Feedback feedback_; 

public:
    ExampleActionServer(); 

    ~ExampleActionServer(void) {
    }
    void executeCB(const actionlib::SimpleActionServer<my_action_server::demo2Action>::GoalConstPtr& goal);
};

ExampleActionServer::ExampleActionServer() :
   as_(nh_, "my_action_server", boost::bind(&ExampleActionServer::executeCB, this, _1),false) 
{
    ROS_INFO("in constructor of exampleActionServer...");

    as_.start(); 
}
void ExampleActionServer::executeCB(const actionlib::SimpleActionServer<my_action_server::demo2Action>::GoalConstPtr& goal) {
    ROS_INFO("in executeCB");
    ROS_INFO("goal input is: %d", goal->input);

    ros::Rate loop_timer(1000);
    double timer=0.0;
      ROS_INFO("goal input is: %d ", goal->input);
	 ROS_INFO("goal halt =: %d goal finshed is =: %d ", goal->halt, goal->finished);
    if (goal->finished == 1 || goal->halt == 1) {
	twist_cmd.linear.x=0.0;
    	twist_cmd.linear.y=0.0;    
    	twist_cmd.linear.z=0.0;
  	twist_cmd.angular.x=0.0;
   	twist_cmd.angular.y=0.0;
  	twist_cmd.angular.z=0.0;  
	twist_commander.publish(twist_cmd);


    }
    else if (goal->turn == 1){
	while(timer<goal->howLong){
		twist_cmd.angular.y=.5;
		twist_cmd.linear.x= 0;
		twist_commander.publish(twist_cmd);
	        timer+=sample_dt;
	        loop_timer.sleep();
        }
    }	
    else{
	while(timer<goal->howLong){ 
		twist_cmd.angular.y=0;
		twist_cmd.linear.x= .5;
		twist_commander.publish(twist_cmd);
	        timer+=sample_dt;
	        loop_timer.sleep();
        }
    }

    g_count++; 
    result_.output = g_count; 
    result_.goal_stamp = goal->input;

    if (g_count != goal->input) {
        ROS_WARN("hey--mismatch!");
        ROS_INFO("g_count = %d; goal_stamp = %d", g_count, result_.goal_stamp);
        g_count_failure = true; 
        ROS_WARN("informing client of aborted goal");
        as_.setAborted(result_);  
    }
    else {
         as_.setSucceeded(result_); 
    }
}

int main(int argc, char** argv) {


    ros::init(argc, argv, "my_action_server"); 
    ros::NodeHandle nh_;  
    ros::Publisher twist_commander = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1);   
    ROS_INFO("instantiating the demo action server: ");

    ExampleActionServer as_object; 
    
    ROS_INFO("going into spin");

    // you will see 5 new topics under example_action: cancel, feedback, goal, result, status
    //while (!g_count_failure) {
        ros::spin(); //normally, can simply do: ros::spin();  
        // for debug, induce a halt if we ever get our client/server communications out of sync
    //}

    return 0;
}

