#include "coord.hpp"
namespace ur{
std::ostream& operator<<(std::ostream &os, const coord &c)
{
    os << '(' << c.x << ", " << c.y << ')';
    return os;
}
}