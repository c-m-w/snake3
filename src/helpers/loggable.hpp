/// loggable.hpp

#pragma once

#define LOGGABLE_IMPL \
    where = __func__;

class loggable
{
protected:
    
    std::string where;

    loggable(std::string where):
        where(where)
    { }

    inline void runtime_error(std::string const & what) const
    {
        std::cerr << "\033[31;5;1m[" << where << "::error" 
                  << "]\033[0;37;1m " << what << "\033[0m" << std::endl;
        exit(-1);
    }
};
