/// settings.hpp

// TODO:
//
// - store settings in a class member JSON object
// - have template<T> get(std::string name, T default = {}) which gets setting
//    - returns default if none exists
//    - check if none exists by catching JSON exception
//    - return REFERENCE to setting so it can be modified
// - find a way to store settings on disk

#pragma once

class settings: public singleton<settings>, loggable
{
private:

    nlohmann::json data { };

public:

    settings();

    template<typename T>
    T load(std::string const & name, T _default = {});
};

#include "settings.ipp"
