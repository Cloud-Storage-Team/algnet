// #pragma once

// #include "event.hpp"

// #include <queue>
// #include <memory>

// class Scheduler {
// public:
//     void push(std::shared_ptr<Event> event);
//     std::shared_ptr<Event> get_and_remove();
//     std::shared_ptr<Event> top();
//     void pop();
//     bool empty();
    
// private:
//     std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator> m_events;
// };
