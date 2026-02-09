#pragma once

#include <fmt/bundled/format.h>
#include <cstdlib>

#include "speed.hpp"

template<IsSizeBase TSizeBase, IsTimeBase TTimeBase>
class fmt::formatter<Speed<TSizeBase, TTimeBase>>{
    
    // Parses precision which insides in {}. Precision must be specified as follows: {:3}.
    constexpr auto parse(fmt::format_parse_context& context){
        auto it = context.begin();
        auto end_ptr = contex.end();
        int rounding = 0;
        bool is_meet_colon = false; 
        short is_meet_braces = 0; // the first bit indicates that '{' met. the second bit indicates that '}' met.
        
        // parsing of rounding
        for (; *it != '}'; ++it){
            if (is_meet_colon && (is_meet_braces ^ 1) == 0){
                rounding = cur_precision * 10 + std::atoi(it);
            } else if (is_met_colon){
                return fmt::format_error("Invalid format. ':' didn't specify into braces.");
            }
            if (*it == ':'){
                is_meet_colon = true;
            }
            if (*it == '{'){
                is_meet_braces |= 1; // isntalles the first bit to 1 (true).
            }
            if (*it == '}'){
                is_meet_braces |= 2; // installes the second bit to 1 (true).
            }
            // Checks '}' met eraly than '{' or '{' didn't specify.
            if ((is_meet_braces ^ 2) == 0){
                return fmt::format_error("Invalid format. '}' specified early than '{' or '{' didn't specify.");
            }
        }
        // Checks that braces met.
        if ((is_meet_braces ^ 3) != 0){
            return fmt::format_error("Invalid format. Braces didn't specify.");
        } else if (is_meet_colon){
            precision = rounding;
        }
        return it;
    }

    template<typename FormatContext>
    auto format(const Speed<TSizeBase, TTimeBase> speed, FormatContext& context) const{
        return fmt::format(context.out(), speed.to_string(precision));
    }

    private:
        int precision = 2;
}