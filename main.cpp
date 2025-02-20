#include <iostream>
#include "queue"
#include "vector"
#include "source/event.hpp"

using namespace sim;

struct Compare {
    bool operator()(const Event* e1, const Event* e2) {
        return e1->time > e2->time;
    }
};

int main() {
    std::priority_queue<Event*, std::vector<Event*>, Compare> q;
    Stop e1(2), e2(3);
    q.push(&e1);
    q.push(&e2);
    while (!q.empty())
    {
        Event* el = q.top();
        q.pop();
        std::cout << el->time << std::endl;
    }
    
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
