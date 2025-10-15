#pragma once
#include <memory>

#include "connection/i_connection.hpp"
#include "types.hpp"
#include "utils/flag_manager.hpp"

namespace sim {
struct TcpCommon {
    TcpCommon(Id a_id, std::shared_ptr<IConnection> a_connection,
              bool a_ecn_capable);

    static void initialize_flag_manager();

    static constexpr std::string packet_type_label = "type";
    enum PacketType { ACK, DATA, ENUM_SIZE };

    static constexpr std::string ack_ttl_label = "ack_ttl";

    static bool is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> flag_manager;

    const static inline TTL MAX_TTL = 31;

    Id id;
    std::weak_ptr<IConnection> connection;
    bool ecn_capable;
};

using TcpCommonPtr = std::shared_ptr<const TcpCommon>;

}  // namespace sim