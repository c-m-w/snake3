/// dependable.hpp

template<typename ... Ts>
class dependable
{
public:

    dependable()
    {
        static_assert((std::is_base_of<singleton<Ts>, Ts>::value, ...), "Ts... need to inherit from singleton<Ts> to use dependable.");
        (Ts::get(), ...);
    }
};
