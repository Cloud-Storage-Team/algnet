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

    // Creates TObject by given args and registrate it
    // Returns nullptr if object with such Id already exists,
    // smart pointer to created object otherwise
    template <typename TObject, typename... Args>
    std::shared_ptr<TObject> create_object(Args&&... args) {
        static_assert(std::is_constructible_v<TObject, Args&&...>,
                      "TObject must be constructable from Args");
        static_assert(std::is_base_of_v<Identifiable, TObject>,
                      "TObject must implement Identifiable interface");
        std::shared_ptr<TObject> ptr =
            std::make_shared<TObject>(std::forward<Args>(args)...);
        if (!add_object(ptr)) {
            return nullptr;
        }
        return ptr;
    }

    void clear();

private:
    IdentifierFactory() {}
    IdentifierFactory(const IdentifierFactory&) = delete;
    IdentifierFactory& operator=(const IdentifierFactory&) = delete;

    std::map<Id, std::shared_ptr<Identifiable> > m_id_table;
};

}  // namespace sim
