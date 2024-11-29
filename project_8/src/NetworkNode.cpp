#include "NetworkNode.hpp"

bool NetworkSimulator::NetworkNode::add_connection(const  std::shared_ptr<NetworkNode>& other_node) 
        {
            // Check if the connection already exists
            auto it = std::find_if(connections.begin(), connections.end(),
                [&other_node](const std::weak_ptr<NetworkNode>& node) 
                {
                    return node.lock() == other_node;
                });
            
            if (it == connections.end()) 
            {
                connections.emplace_back(other_node);
                other_node->add_connection(shared_from_this());  // Ensure mutual connection
                return true;  // Connection was successfully added
            }

            return false; // Connection already exists
        }

bool NetworkSimulator::NetworkNode::remove_connection(const  std::shared_ptr<NetworkNode>& other_node) 
        {
            auto it = std::find_if(connections.begin(), connections.end(),
                [&other_node](const std::weak_ptr<NetworkNode>& node) 
                {
                    return node.lock() == other_node;
                });

            if (it != connections.end()) 
            {
                connections.erase(it);
                other_node->remove_connection(shared_from_this());  // Remove mutual connection
                return true;  // Successfully removed
            }

            return false; // Connection not found
        }