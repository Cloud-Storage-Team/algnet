#include <cstdint>
#include <functional>


class Event
{    
    public:
        Event(uint64_t time, const std::function<void()>& handler);

        Event() : execution_time_(0), handler_([](){}) {} 

        bool operator<(const Event& other) const;

        uint64_t  GetTime();
        
        void Execute() const;
        

    private:
        uint64_t  execution_time_;
        std::function<void()> handler_;

};

