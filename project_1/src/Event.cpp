#include "Event.hpp"

Event::Event(ServerBase& initiator, event_type type, std::uint32_t units_number)
    : server_initiator(initiator), type(type), units_number(units_number) { }

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << "Initiator ID: " << event.server_initiator.GetID();
    switch (event.type) {
        case event_type::SEND_DATA:
            out << ". Type: SEND_DATA"
                << ". Number of packets: " << event.units_number << ".\n";
            break;
        case event_type::ACKNOWLEDGEMENT:
            out << ". Type: ACKNOWLEDGEMENT.\n";
            break;
    }
    return out;
}