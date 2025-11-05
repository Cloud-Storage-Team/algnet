#pragma once

#include <memory>
#include <vector>

#include "types.hpp"

namespace sim {

class Identifiable {
public:
    virtual ~Identifiable() = default;
    virtual Id get_id() const = 0;
};

class IdentifierFactory {
public:
    static IdentifierFactory& get_instance() {
        static IdentifierFactory instance;
        return instance;
    }

    bool add_object(std::shared_ptr<Identifiable> object);

    template <typename TObject>
    std::shared_ptr<TObject> get_object(Id id) {
        static_assert(std::is_base_of_v<Identifiable, TObject>,
                      "TObject must implement Identifiable interface");
        auto it = m_id_table.find(id);
        if (it == m_id_table.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<TObject>(it->second);
    }

    template <typename TObject>
    std::vector<std::shared_ptr<TObject> > get_objects() {
        static_assert(std::is_base_of_v<Identifiable, TObject>,
                      "TObject must implement Identifiable interface");
        std::vector<std::shared_ptr<TObject> > result;
        for (auto& [id, base_ptr] : m_id_table) {
            if (auto obj = std::dynamic_pointer_cast<TObject>(base_ptr); obj) {
                result.push_back(obj);
            }
        }
        return result;
    }

    void clear();

private:
    IdentifierFactory() {}
    IdentifierFactory(const IdentifierFactory&) = delete;
    IdentifierFactory& operator=(const IdentifierFactory&) = delete;

    std::map<Id, std::shared_ptr<Identifiable> > m_id_table;
};

}  // namespace sim
