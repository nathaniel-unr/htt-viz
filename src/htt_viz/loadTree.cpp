/**
 * Main file to load a distributive task tree. Only needs a description of a task tree in the rosparams server
 *	to be able to load up and run a task tree.
 *
 *
 *	Inputs:
 *		- The gameplan is to input a task tree in standard form in the rosparams.
 *		- Could also input the name of a node.
 *		- Mostly a copy of previously used test file for the tree.
 * Created By: Tyler Becker
 * Created On: 11/11/21
 * Last Updated: 11/11/21
 */

#include "task_tree/node.h"
#include "task_tree/behavior.h"
#include <boost/thread/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <signal.h>
#include <vector>
#include <string>
#include <map>

typedef std::vector<std::string> NodeParam;

void EndingFunc(int signal) {
	printf("Closing Program...\n");
	ros::shutdown();
}

int main(int argc, char** argv)
{

	ros::init(argc, argv, "behavior_network", ros::init_options::NoSigintHandler);
	signal(SIGINT, EndingFunc);
	ros::NodeHandle nh_("~");
	ros::NodeHandle nh;
	task_net::Node** network;

	task_net::NodeId_t name_param;
	std::vector<std::string> peers_param_str;
	task_net::NodeList peers_param;
	std::vector<std::string> children_param_str;
	task_net::NodeList children_param;
	task_net::NodeId_t parent_param;
	NodeParam nodes;
	std::string obj_name;


	if (nh_.getParam("NodeList", nodes)) {
		printf("Tree Size: %lu\n", nodes.size());
	}

	network = new task_net::Node * [nodes.size()];

	// Grab Node Attributes
	std::string param_prefix = "Nodes/";
	std::string param_ext_children = "children";
	std::string param_ext_parent = "parent";
	std::string param_ext_peers = "peers";
	for (int i = 0; i < nodes.size(); ++i)
	{
		// Get name
		name_param.topic = nodes[i];
		// printf("name: %s\n", name_param.topic.c_str());

		// only init the nodes for the correct robot!!!
		int robot;
		if (nh_.getParam((param_prefix + nodes[i] + "/mask/robot").c_str(), robot))
		{
			if (true)
			{

				printf("Creating Task Node for:\n");
				printf("\tname: %s\n", name_param.topic.c_str());

				// get parent 
				if (nh_.getParam((param_prefix + nodes[i] + "/" + param_ext_parent).c_str(), parent_param.topic))
				{
					printf("Node: %s Parent: %s\n", nodes[i].c_str(), parent_param.topic.c_str());
				}

				// get children
				children_param.clear();
				if (nh_.getParam((param_prefix + nodes[i] + "/" + param_ext_children).c_str(), children_param_str))
				{
					for (int j = 0; j < children_param_str.size(); ++j)
					{
						task_net::NodeId_t temp;
						temp.topic = children_param_str[j];
						temp.pub = NULL;
						children_param.push_back(temp);
						printf("Node: %s Child: %s\n", nodes[i].c_str(), temp.topic.c_str());
					}
				}

				// get peers
				peers_param.clear();
				if (nh_.getParam((param_prefix + nodes[i] + "/" + param_ext_peers).c_str(), peers_param_str))
				{
					for (int j = 0; j < peers_param_str.size(); ++j)
					{
						task_net::NodeId_t temp;
						temp.topic = peers_param_str[j];
						temp.pub = NULL;
						peers_param.push_back(temp);
						printf("Node: %s Peer: %s\n", nodes[i].c_str(), temp.topic.c_str());
					}
				}

				// Create Node
				task_net::State_t state;
				task_net::Node* test;

				int type;
				if (nh_.getParam((param_prefix + nodes[i] + "/mask/type").c_str(), type))
				{
					// printf("Node: %s NodeType: %d\n", nodes[i].c_str(), type);
				}

				switch (type)
				{
				case task_net::THEN:
					network[i] = new task_net::ThenBehavior(name_param,
						peers_param,
						children_param,
						parent_param,
						state,
						"N/A",
						false);
					// printf("\ttask_net::THEN %d\n",task_net::THEN);
					break;
				case task_net::OR:
					network[i] = new task_net::OrBehavior(name_param,
						peers_param,
						children_param,
						parent_param,
						state,
						"N/A",
						false);
					// printf("\ttask_net::OR %d\n",task_net::OR);
					break;
				case task_net::AND:
					network[i] = new task_net::AndBehavior(name_param,
						peers_param,
						children_param,
						parent_param,
						state,
						"N/A",
						false);
					// printf("\ttask_net::AND %d\n",task_net::AND);
					break;
				/*case task_net::BEHAVIOR:
				{
					// ROS_INFO("Children Size: %lu", children_param.size());
					// object = name_param.topic.c_str();
					// get the name of the object of corresponding node:
					nh_.getParam((param_prefix + nodes[i] + "/object").c_str(), obj_name);
					// set up network for corresponding node:
					// ros::param::get(("/ObjectPositions/"+obj_name).c_str(), object_pos);
					// network[i] = new task_net::TableObject(name_param,
					//                           peers_param,
					//                           children_param,
					//                           parent_param,
					//                           state,
					//                           "/right_arm_mutex",
					//                           obj_name.c_str(),
					//                           neutral_object_pos,
					//                           object_pos,
					//                           false);

					std::string place;
					int behavior_type;

					nh_.getParam((param_prefix + nodes[i] + "/obj_dest"), place);
					nh_.getParam((param_prefix + nodes[i] + "/b_type"), behavior_type);
					if (behavior_type == 0)
					{
						int s_int = 0;

						//temp filler to check for compilation errors
						geometry_msgs::PoseStamped p;
						std::string dest_name;

						nh_.getParam((param_prefix + nodes[i] + "/obj_dest"), dest_name);

						float temp = 0;
						std::string sanity = "/task_tree_node/" + dest_name + "/position" + "/x";

						nh_.getParam(("/task_tree_node/" + dest_name + "/position" + "/x"), p.pose.position.x);
						nh_.getParam(("/task_tree_node/" + dest_name + "/position" + "/y"), p.pose.position.y);
						nh_.getParam(("/task_tree_node/" + dest_name + "/position" + "/z"), p.pose.position.z);
						nh_.getParam(("/task_tree_node/" + dest_name + "/orientation" + "/x"), p.pose.orientation.x);
						nh_.getParam(("/task_tree_node/" + dest_name + "/orientation" + "/y"), p.pose.orientation.y);
						nh_.getParam(("/task_tree_node/" + dest_name + "/orientation" + "/z"), p.pose.orientation.z);
						nh_.getParam(("/task_tree_node/" + dest_name + "/orientation" + "/w"), p.pose.orientation.w);

						p.header.frame_id = "node_tree";

						nh_.getParam((param_prefix + nodes[i] + "/s_intent"), s_int);

						network[i] = new task_net::MoveBehavior(p,
							s_int,
							nh,
							"base_odometry/odom",
							name_param,
							peers_param,
							children_param,
							parent_param,
							state,
							"blank");
					}
					else if (behavior_type == 1)//pick
					{
						std::string obj_name;
						nh_.getParam((param_prefix + nodes[i] + "/obj_name"), obj_name);
						network[i] = new task_net::PickBehavior(obj_name,
							place[0],
							place[1],
							nh,
							"robot_location",
							name_param,
							peers_param,
							children_param,
							parent_param,
							state,
							"blank");
					}
					else if (behavior_type == 2)//place
					{
						std::string obj_name;
						nh_.getParam((param_prefix + nodes[i] + "/obj_name"), obj_name);
						network[i] = new task_net::PlaceBehavior(obj_name,
							place[0],
							place[1],
							nh,
							"robot_location",
							name_param,
							peers_param,
							children_param,
							parent_param,
							state,
							"blank");
					}
				}
				break;
				case task_net::ROOTBEHAVIOR:
					network[i] = new task_net::RootBehavior(name_param,
						peers_param,
						children_param,
						parent_param,
						state,
						"N/A",
						false,
						boost::posix_time::millisec(1000),
						"pr2.yaml");
					break;*/
				case task_net::ROOT:
					break;
				default:
					network[i] = NULL;
					// printf("\ttask_net::ROOT %d\n",task_net::ROOT);
					break;
				}
			}
			// printf("MADE 5\n");
		}
	}

	ROS_INFO("Running spin");

	ros::MultiThreadedSpinner spinner(4);
	spinner.spin();

	return 0;
}