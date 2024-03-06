#include "Sector.h"

// Constructor implementation

Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
        // TODO: Calculate the distance to the Earth, and generate the sector code
}

Sector::~Sector() {
    // TODO: Free any dynamically allocated memory if necessary
}

Sector& Sector::operator=(const Sector& other) {
    // TODO: Overload the assignment operator

    if (this!=&other) {
        x = other.x;
        y = other.y;
        z = other.z;
        distance_from_earth = other.distance_from_earth;
        sector_code = other.sector_code;

        color = other.color;
        left = other.left;
        right = other.right;
        parent = other.parent;
    }
    return *this;
}

bool Sector::operator==(const Sector& other) const {
    return (sector_code == other.sector_code);
}

bool Sector::operator!=(const Sector& other) const {
    return !(*this == other);
}