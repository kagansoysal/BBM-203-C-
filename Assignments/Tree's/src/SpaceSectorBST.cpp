#include <iterator>
#include <cmath>
#include <algorithm>
#include "SpaceSectorBST.h"

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    // Free any dynamically allocated memory in this class.

    for (Sector *sector: sectors) {
        delete sector;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string &filename) {
    // TODO: read the sectors from the input file and insert them into the BST sector map
    // according to the given comparison critera based on the sector coordinates.

    ifstream file(filename);

    if (file.is_open()) {
        string line;
        int line_count{};

        while (getline(file, line)) {
            if (line_count > 0) {
                stringstream ss(line);
                vector<int> coordinates;

                string number;
                while (getline(ss, number, ',')) {
                    coordinates.push_back(stoi(number));
                }
                insertSectorByCoordinates(coordinates[0], coordinates[1], coordinates[2]);
            }
            line_count++;
        }
        file.close();
    }
}

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    // Instantiate and insert a new sector into the space sector BST map according to the
    // coordinates-based comparison criteria.

    Sector *new_sector = create_new_sector(x, y, z);
    find_location(root, new_sector);
}

void SpaceSectorBST::find_location(Sector *&current_sector, Sector *new_sector) {
    if (current_sector == nullptr) {
        current_sector = new_sector;
        return;
    }

    bool is_right = (new_sector->x != current_sector->x) ? (new_sector->x > current_sector->x) :
                    ((new_sector->y != current_sector->y) ? (new_sector->y > current_sector->y) :
                     (new_sector->z > current_sector->z));

    Sector *&nextSector = is_right ? current_sector->right : current_sector->left;
    find_location(nextSector, new_sector);

    if (current_sector->right != nullptr)
        current_sector->right->parent = current_sector;

    if (current_sector->left != nullptr)
        current_sector->left->parent = current_sector;
}

Sector *SpaceSectorBST::create_new_sector(int x, int y, int z) {
    auto *new_sector = new Sector(x, y, z);

    new_sector->distance_from_earth = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    new_sector->sector_code += to_string(static_cast<int>(floor(new_sector->distance_from_earth)));

    assign_letter(new_sector, x, "L", "R");
    assign_letter(new_sector, y, "D", "U");
    assign_letter(new_sector, z, "B", "F");

    sectors.push_back(new_sector);
    return new_sector;
}

void SpaceSectorBST::assign_letter(Sector *sector, int coordinate, const string &neg, const string &pos) {
    sector->sector_code += coordinate == 0 ? "S" : coordinate > 0 ? pos : neg;
}

void SpaceSectorBST::deleteSector(const std::string &sector_code) {
    // TODO: Delete the sector given by its sector_code from the BST.

    for (Sector *sector: sectors)
        if (sector->sector_code == sector_code)
            find_deleted_sector(root, sector->x, sector->y, sector->z);
}

void SpaceSectorBST::find_deleted_sector(Sector *sector, int x, int y, int z) {
    if (sector->x == x && sector->y == y && sector->z == z)
        delete_sector(sector);
    else {
        bool is_right = (x != sector->x) ? (x > sector->x) :
                        ((y != sector->y) ? (y > sector->y) :
                         (z > sector->z));

        Sector *next_sector = is_right ? sector->right : sector->left;
        find_deleted_sector(next_sector, x, y, z);
    }
}

void SpaceSectorBST::delete_sector(Sector *sector) {
    if (sector->right == nullptr && sector->left == nullptr) {
        replace_sector(sector, nullptr);

    } else if (sector->left == nullptr) {
        replace_sector(sector, sector->right);

    } else if (sector->right == nullptr) {
        replace_sector(sector, sector->left);

    } else {
        Sector *inorder_successor = find_inorder_successor(sector->right);
        Sector *left_of_sector = sector->left;
        Sector *right_of_sector = sector->right;
        Sector *right_of_inorder_successor = inorder_successor->right;
        Sector *parent_of_inorder_successor = inorder_successor->parent;

        replace_sector(sector, inorder_successor);

        inorder_successor->left = left_of_sector;
        left_of_sector->parent = inorder_successor;
        parent_of_inorder_successor->left = right_of_inorder_successor;


        if (inorder_successor != right_of_sector) {
            inorder_successor->right = right_of_sector;
            right_of_sector->parent = inorder_successor;
            if (right_of_inorder_successor != nullptr) right_of_inorder_successor->parent = parent_of_inorder_successor;
            if (right_of_sector->left == sector) right_of_sector->left = right_of_inorder_successor;
        }

        if (root == sector) root = inorder_successor;
    }
}

void SpaceSectorBST::replace_sector(Sector *sector, Sector *replacement_sector) {
    Sector *parent_sector = sector->parent;

    if (root == sector) {
        root = replacement_sector;
    } else if (parent_sector->right == sector) {
        parent_sector->right = replacement_sector;
    } else if (parent_sector->left == sector) {
        parent_sector->left = replacement_sector;
    }
    if (replacement_sector != nullptr) replacement_sector->parent = parent_sector;
}

Sector *SpaceSectorBST::find_inorder_successor(Sector *sector) {
    return sector->left == nullptr ? sector : find_inorder_successor(sector->left);
}

void SpaceSectorBST::displaySectorsInOrder() {
    // TODO: Traverse the space sector BST map in-order and print the sectors 
    // to STDOUT in the given format.

    cout << "Space sectors inorder traversal:" << endl;
    in_order(root);
    cout << endl;
}

void SpaceSectorBST::in_order(Sector *sector) {
    if (sector != nullptr) {
        in_order(sector->left);
        cout << sector->sector_code << endl;
        in_order(sector->right);
    }
}

void SpaceSectorBST::displaySectorsPreOrder() {
    // TODO: Traverse the space sector BST map in pre-order traversal and print 
    // the sectors to STDOUT in the given format.

    cout << "Space sectors preorder traversal:" << endl;
    pre_order(root);
    cout << endl;
}

void SpaceSectorBST::pre_order(Sector *sector) {
    if (sector != nullptr) {
        cout << sector->sector_code << endl;
        pre_order(sector->left);
        pre_order(sector->right);
    }
}

void SpaceSectorBST::displaySectorsPostOrder() {
    // TODO: Traverse the space sector BST map in post-order traversal and print 
    // the sectors to STDOUT in the given format.

    cout << "Space sectors postorder traversal:" << endl;
    post_order(root);
    cout << endl;
}

void SpaceSectorBST::post_order(Sector *sector) {
    if (sector != nullptr) {
        post_order(sector->left);
        post_order(sector->right);
        cout << sector->sector_code << endl;
    }
}

std::vector<Sector *> SpaceSectorBST::getStellarPath(const std::string &sector_code) {
    std::vector<Sector *> path;
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!

    Sector *target_sector = nullptr;

    for (Sector *sector: sectors) {
        if (sector->sector_code == sector_code) {
            target_sector = sector;
        }
    }

    while (target_sector != nullptr) {
        path.push_back(target_sector);
        target_sector = target_sector->parent;
    }

    std::reverse(path.begin(), path.end());

    return path;
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector *> &path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function 
    // to STDOUT in the given format.

    if (!path.empty()) {
        cout << "The stellar path to Dr. Elara: ";

        for (Sector *sector: path) {
            cout << sector->sector_code;
            if (sector == path.back()) cout << endl;
            else cout << "->";
        }
    } else cout << "A path to Dr. Elara could not be found." << endl;
}