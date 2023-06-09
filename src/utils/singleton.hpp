/// singleton.hpp

#pragma once

template<typename T>
class singleton
{
public:

    static T* get()
    {
        static auto inst = T();

        return &inst;
    }
};
