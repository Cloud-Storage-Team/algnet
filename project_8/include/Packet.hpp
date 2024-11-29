#include <cstdint>
namespace NetworkSimulator
{
    class Packet
        {
        public:
            // Constructor (stub)
            Packet(NodeAddress source_id, NodeAddress destination_id): source_id(source_id), destination_id(destination_id) {}

            // Getters (stub)
            NodeAddress get_source() const { return source_id; }
            NodeAddress get_destination() const { return destination_id; }
            

            void forward() const {}

            void display_info() const {}

        private:
            NodeAddress source_id;        
            NodeAddress destination_id;    

        };
}