/*
* Created by boil on 25-2-26.
*/

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

namespace core
{

    class World;

    class System {
    public:
        explicit System(World& world) : world_(world) {}
        virtual void update(double deltaTime) = 0;
        virtual ~System() = default;

    protected:
        World& world_;
    };


} // namespace rendu

#endif //SYSTEM_HPP
