#pragma once

#include <iostream>
namespace ur {
struct coord {
    // we might have many of these at once...
    int x:4, y:4;

    bool operator==(coord const&) const = default;
    
    friend std::ostream& operator<<(std::ostream &os, const coord &c);

    struct HashFunction
    {
      std::size_t operator()(const ur::coord& k) const
      {
        return k.x + (k.y << 5);
      }
    };
};
}