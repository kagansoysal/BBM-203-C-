#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

class SpaceSectorLLRBT {
private:
    std::vector<Sector*> sectors;

public:
    Sector* root;
    SpaceSectorLLRBT();
    ~SpaceSectorLLRBT();
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);


    Sector *create_new_sector(int x, int y, int z);

    static void assign_letter(Sector *sector, int coor, const std::string &neg, const std::string &pos);

    void find_location(Sector *&current_sector, Sector *new_sector);

    void balance(Sector *sector);

    static bool need_flip(Sector *sector);

    static bool need_left_rotation(Sector *sector);

    static bool need_right_rotation(Sector *sector);

    void flip(Sector *sector);

    void rotate_left(Sector *sector);

    void rotate_right(Sector *sector);

    void in_order(Sector *sector);

    void pre_order(Sector *sector);

    void post_order(Sector *sector);
};

#endif // SPACESECTORLLRBT_H
