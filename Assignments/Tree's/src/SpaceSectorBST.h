#ifndef SPACESECTORBST_H
#define SPACESECTORBST_H

#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

#include "Sector.h"

class SpaceSectorBST {
private:
    std::vector<Sector*> sectors;

public:
    Sector *root;
    SpaceSectorBST();
    ~SpaceSectorBST();
    void readSectorsFromFile(const std::string& filename); 
    void insertSectorByCoordinates(int x, int y, int z);
    void deleteSector(const std::string& sector_code);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);

    static void assign_letter(Sector *sector, int coor, const std::string &neg, const std::string &pos);

    Sector *create_new_sector(int x, int y, int z);

    Sector* find_location(Sector *parent, int x, int y, int z);

    void find_deleted_sector(Sector *sector, int x, int y, int z);

    void delete_sector(Sector *sector);

    Sector *find_inorder_successor(Sector *sector);

    void pre_order(Sector *sector);

    void post_order(Sector *sector);

    void in_order(Sector *sector);

    void find_location(Sector *&current_sector, Sector *new_sector);

    void replace_sector(Sector *sector, Sector *replacement_sector);
};

#endif // SPACESECTORBST_H
