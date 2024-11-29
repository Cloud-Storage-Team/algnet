#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <Packet.hpp>
namespace NetworkSimulator
{
    // Add inheritance from enable_shared_from_this
    class NetworkNode : public std::enable_shared_from_this<NetworkNode>
    {
    public:
        virtual void send(NetworkSimulator::Packet packet);

        bool add_connection(const  std::shared_ptr<NetworkNode>& other_node);

        bool remove_connection(const std::shared_ptr<NetworkNode>& other_node) ;

        uint32_t get_id()   const { return address.id; }
        uint32_t get_rack() const { return address.rack;}
        uint32_t get_zone() const { return address.zone;}
        NodeType get_type() const { return address.type;}

    private:
        NodeAddress address;
        std::vector<std::weak_ptr<NetworkNode>> connections;  // Use weak_ptr to avoid cyclic dependencies and memory leaks
    };

    enum class NodeType : uint8_t 
    {
        Server = 0,
        Switch = 1
    };

    class NodeAddress 
    {
        friend class NetworkNode;


        explicit NodeAddress(uint32_t id, uint32_t rack, uint32_t zone, NodeType type) : id{id}, rack{rack}, zone{zone}, type{type} {}
        
        bool operator==(const NodeAddress& other) const 
        {
            return id == other.id && rack == other.rack && zone == other.zone && type == other.type;
        }

        bool operator<(const NodeAddress& other) const 
        {
            if (rack != other.rack) 
            {
                return rack < other.rack; 
            }
            if (zone != other.zone) 
            {
                return zone < other.zone; 
            }
            return id < other.id; 
        }

        uint32_t get_id()   const { return id; }
        uint32_t get_rack() const { return rack;}
        uint32_t get_zone() const { return zone;}
        NodeType get_type() const { return type;}

        private:
        uint32_t id;       
        uint32_t rack;     // Rack number
        uint32_t zone;     // Zone number
        NodeType type;     // Type Node ( Server or Switch)
    };
}
