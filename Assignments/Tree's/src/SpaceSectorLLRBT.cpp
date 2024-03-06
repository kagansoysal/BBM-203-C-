#include "SpaceSectorLLRBT.h"
#include <cmath>
#include <algorithm>

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string& filename) {
    // TODO: read the sectors from the input file and insert them into the LLRBT sector map
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

// Remember to handle memory deallocation properly in the destructor.
SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    // TODO: Free any dynamically allocated memory in this class.

    for (Sector* sector : sectors) {
        delete sector;
    }
}

void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    // TODO: Instantiate and insert a new sector into the space sector LLRBT map 
    // according to the coordinates-based comparison criteria.

    Sector* new_sector = create_new_sector(x, y, z);
    find_location(root, new_sector);
    if (root != new_sector) balance(new_sector);
}

void SpaceSectorLLRBT::find_location(Sector*& current_sector, Sector* new_sector) {
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

void SpaceSectorLLRBT::balance(Sector* sector) {
    if (need_flip(sector)) flip(sector);
    else if (need_left_rotation(sector)) rotate_left(sector);
    else if (need_right_rotation(sector)) rotate_right(sector);
}

void SpaceSectorLLRBT::flip(Sector* sector) {
    Sector* sibling = (sector == sector->parent->left) ? sector->parent->right : sector->parent->left;

    sector->parent->color = RED;
    sector->color = BLACK;
    sibling->color = BLACK;

    if (sector->parent->parent == nullptr)
        sector->parent->color = BLACK;
    else
        balance(sector->parent);
}

void SpaceSectorLLRBT::rotate_left(Sector* sector) {
    Sector* parent_of_sector = sector->parent;
    Sector* granpa_of_sector = sector->parent->parent; //can be null
    Sector* left_of_sector = sector->left; //can be null

    sector->color = parent_of_sector->color;
    parent_of_sector->color = RED;

    sector->parent = granpa_of_sector;
    sector->left = parent_of_sector;
    parent_of_sector->parent = sector;
    parent_of_sector->right = left_of_sector;

    if (left_of_sector != nullptr) left_of_sector->parent = parent_of_sector;

    (granpa_of_sector != nullptr ?  granpa_of_sector->right == parent_of_sector ? granpa_of_sector->right : granpa_of_sector->left
                                 : root) = sector;

    balance(sector->left);
}

void SpaceSectorLLRBT::rotate_right(Sector* sector) {
    Sector* parent_of_sector = sector->parent;
    Sector* granpa_of_sector = sector->parent->parent;
    Sector* big_granpa_of_sector = sector->parent->parent->parent; //can be null
    Sector* sibling_of_sector = sector->parent->right; //can be null

    parent_of_sector->color = BLACK;
    granpa_of_sector->color = RED;

    (big_granpa_of_sector != nullptr ?  big_granpa_of_sector->right == granpa_of_sector ? big_granpa_of_sector->right : big_granpa_of_sector->left
                                 : root) = parent_of_sector;

    granpa_of_sector->parent = parent_of_sector;
    granpa_of_sector->left = sibling_of_sector;
    parent_of_sector->parent = big_granpa_of_sector;
    parent_of_sector->right = granpa_of_sector;
    if (sibling_of_sector != nullptr) sibling_of_sector->parent = granpa_of_sector;

    balance(sector);
}

bool SpaceSectorLLRBT::need_flip(Sector* sector) {
    Sector* sibling = (sector == sector->parent->left) ? sector->parent->right : sector->parent->left;
    return sibling != nullptr && sibling->color;
}

bool SpaceSectorLLRBT::need_left_rotation(Sector* sector) {
    return sector->parent->right == sector;
}

bool SpaceSectorLLRBT::need_right_rotation(Sector* sector) {
    return sector->color && sector->parent->color;
}

Sector* SpaceSectorLLRBT::create_new_sector(int x, int y, int z) {
    auto* new_sector = new Sector(x,y,z);

    if (root == nullptr) new_sector->color = BLACK;
    new_sector->distance_from_earth = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    new_sector->sector_code += to_string(static_cast<int>(std::floor(new_sector->distance_from_earth)));

    assign_letter(new_sector, x, "L", "R");
    assign_letter(new_sector, y, "D", "U");
    assign_letter(new_sector, z, "B", "F");

    sectors.push_back(new_sector);
    return new_sector;
}

void SpaceSectorLLRBT::assign_letter(Sector* sector, int coordinate, const string& neg, const string& pos) {
    sector->sector_code += coordinate == 0 ? "S" : coordinate > 0 ? pos : neg;
}

void SpaceSectorLLRBT::displaySectorsInOrder() {
    // TODO: Traverse the space sector LLRBT map in-order and print the sectors 
    // to STDOUT in the given format.

    cout << "Space sectors inorder traversal:" << endl;
    in_order(root);
    cout << endl;
}

void SpaceSectorLLRBT::in_order(Sector* sector) {
    if (sector != nullptr) {
        in_order(sector->left);
        cout << (sector->color ? "RED" : "BLACK") << " sector: " << sector->sector_code << endl;
        in_order(sector->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    // TODO: Traverse the space sector LLRBT map in pre-order traversal and print 
    // the sectors to STDOUT in the given format.

    cout << "Space sectors preorder traversal:" << endl;
    pre_order(root);
    cout << endl;
}

void SpaceSectorLLRBT::pre_order(Sector* sector) {
    if (sector != nullptr) {
        cout << (sector->color ? "RED" : "BLACK") << " sector: " << sector->sector_code << endl;
        pre_order(sector->left);
        pre_order(sector->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    // TODO: Traverse the space sector LLRBT map in post-order traversal and print 
    // the sectors to STDOUT in the given format.

    cout << "Space sectors postorder traversal:" << endl;
    post_order(root);
    cout << endl;
}

void SpaceSectorLLRBT::post_order(Sector* sector) {
    if (sector != nullptr) {
        post_order(sector->left);
        post_order(sector->right);
        cout << (sector->color ? "RED" : "BLACK") << " sector: " << sector->sector_code << endl;
    }
}

std::vector<Sector*> SpaceSectorLLRBT::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;
    // TODO: Find the path from the Earth to the destination sector given by its
    // sector_code, and return a vector of pointers to the Sector nodes that are on
    // the path. Make sure that there are no duplicate Sector nodes in the path!

    Sector* target_sector {};
    Sector* current_sector {};

    for (Sector *sector: sectors)
        if (sector->sector_code == sector_code)
            target_sector = sector;
        else if (sector->sector_code == "0SSS")
            current_sector = sector;

    if (target_sector != nullptr && target_sector->sector_code == "0SSS") {
        path.push_back(target_sector);
        return path;
    }

    if (target_sector != nullptr) {
        while (current_sector != target_sector) {
            path.push_back(current_sector);

            bool same_subtree {};
            Sector* temp = target_sector;

            while (temp != nullptr) {
                if (temp == current_sector)
                    same_subtree = true;
                temp = temp->parent;
            }

            int target_coordinate = (target_sector->x != current_sector->x) ? target_sector->x
                                                                             : (target_sector->y != current_sector->y) ? target_sector->y
                                                                                                                       : target_sector->z;

            int current_coordinate = (target_sector->x != current_sector->x) ? current_sector->x
                                                                              : (target_sector->y != current_sector->y) ? current_sector->y
                                                                                                                        : current_sector->z;

            current_sector = same_subtree ? (target_coordinate > current_coordinate ? current_sector->right : current_sector->left) : current_sector->parent;
        }
        path.push_back(target_sector);
    }
    return path;
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector*>& path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function 
    // to STDOUT in the given format.

    if (!path.empty()) {
        cout << "The stellar path to Dr. Elara: ";

        for (Sector *sector: path) {
            cout << sector->sector_code;
            if (sector == path.back()) cout << endl;
            else cout << "->";
        }
        cout << endl;
    } else cout << "A path to Dr. Elara could not be found." << endl;
}