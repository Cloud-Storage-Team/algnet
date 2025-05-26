#pragma once

#include <memory>

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

    template <typename TObject>
    bool add_object(std::unique_ptr<TObject> object) {
        static_assert(std::is_base_of_v<Identifiable, TObject>,
                      "TObject must implement Identifiable interface");
        Id id = object->get_id();
        auto it = m_id_table.lower_bound(id);
        if (it == m_id_table.end()) {
            return false;
        }
        m_id_table.insert(it,
                          {id, std::shared_ptr<TObject>(std::move(object))});
        return true;
    }

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

private:
    IdentifierFactory() {}
    IdentifierFactory(const IdentifierFactory&) = delete;
    IdentifierFactory& operator=(const IdentifierFactory&) = delete;

    std::map<Id, std::shared_ptr<Identifiable> > m_id_table;
};

}  // namespace sim
