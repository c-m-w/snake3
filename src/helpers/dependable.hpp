/// dependable.hpp

template<typename T, typename ... Ts>
class dependable
{
public:

    dependable()
    {
        T::get();
        dependable<Ts...>();
    }
};