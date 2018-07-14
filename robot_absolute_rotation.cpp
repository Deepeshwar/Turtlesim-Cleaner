#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <sstream>

ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;
turtlesim::Pose turtlesim_pose;

const double x_min = 0.0;
const double y_min = 0.0;
const double x_max = 11.0;
const double y_max = 11.0;

const double PI = 3.14159265359;

using namespace std;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double angle, bool clockwise);
double degrees2radians(double angle_in_degress);
double setDesiredOrientation(double desired_angle_radians);
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);

int main(int argc, char **argv)
{
	ros::init(argc, argv, "robot_cleaner");
	ros::NodeHandle n;

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",10);
	pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);
	ros::Rate loop_rate(10);
	
	double speed;
	double distance;
	bool isForward;
	
	double angular_speed;
	double angle;
	bool clockwise;
	
	ROS_INFO("\n\n\n ********START TESTING*********\n");
	/*cout<<"enter speed: ";
	cin>>speed;
	cout<<"enter distance: ";
	cin>>distance;
	cout<<"forward? : ";
	cin>>isForward;
	move(speed, distance, isForward);

	cout<<"enter angular velocity (degrees/sec): ";
	cin>>angular_speed;
	cout<<"enter desired angle (degrees): ";
	cin>>angle;
	cout<<"clockwise ?: ";
	cin>>clockwise;
	rotate(degrees2radians(angular_speed), degrees2radians(angle), clockwise);*/

	setDesiredOrientation(degrees2radians(120));
	ros::Rate loop_rate(0.5);
	loop_rate.sleep();
	setDesiredOrientation(degrees2radians(-60));
	loop_rate.sleep();
	setDesiredOrientation(degrees2radians(0));

		
		
	ros::spin();
	
	return 0;
}

void move(double speed, double distance, bool isForward)
{
	geometry_msgs::Twist vel_msg;
	//distance = speed*time
	
	//set a random linear velocity in the x-axis
	if(isForward)
		vel_msg.linear.x =abs(speed);
	else
		vel_msg.linear.x =-abs(speed);
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;

//set a random angular velocity in the y-axis

	vel_msg.angular.x =0;
	vel_msg.angular.y =0;
	vel_msg.angular.z =0;

	//t0: initial time
	double t0 = ros::Time::now().toSec();
	double current_distance=0;
	ros::Rate loop_rate(10);
	do{
		velocity_publisher.publish(vel_msg);
		double t1 = ros::Time::now().toSec();
		current_distance = speed*(t1-t0);
		ros::spinOnce();
		loop_rate.sleep();
	  }while(current_distance < distance);
	
	vel_msg.linear.x = 0;
	velocity_publisher.publish(vel_msg); 
	//loop
	//publish the velocity
	//estimate the current_distance=speed*(t1-t0)
	//current_distance_moved_by_robot<=distance
}

void rotate (double angular_speed, double relative_angle, bool clockwise){

	geometry_msgs::Twist vel_msg;

	vel_msg.linear.x =0;
	vel_msg.linear.y =0;
	vel_msg.linear.z =0;

	vel_msg.angular.x =0;
	vel_msg.angular.y =0;

	if(clockwise)
		vel_msg.angular.z= -abs(angular_speed);
	else 
		vel_msg.angular.z= abs(angular_speed);

	double current_angle =0.0;
	double t0 = ros::Time::now().toSec();
 	ros::Rate loop_rate(10);
	do{
		velocity_publisher.publish(vel_msg);
		double t1 = ros::Time::now().toSec();
		current_angle = angular_speed * (t1-t0);
		ros::spinOnce();
		loop_rate.sleep();
	  }while(current_angle<relative_angle);
	vel_msg.angular.z =0;
	velocity_publisher.publish(vel_msg);

}

double degrees2radians(double angle_in_degrees){
	return angle_in_degrees *PI / 180.0;
}

double setDesiredOrientation(double desired_angle_radians){
	double relative_angle_radians = desired_angle_radians - turtlesim_pose.theta;
	bool clockwise = ((relative_angle_radians<0)?true:false);
	rotate (abs(relative_angle_radians), abs(relative_angle_radians), clockwise);
	
}

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message){
	turtlesim_pose.x = pose_message->x;
	turtlesim_pose.y = pose_message->y;
	turtlesim_pose.theta = pose_message->theta;
}
