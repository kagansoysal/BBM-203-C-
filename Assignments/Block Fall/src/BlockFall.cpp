#include <fstream>
#include <sstream>
#include <iostream>
#include "BlockFall.h"

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    linking_blocks(input_file);
    determine_power_up();
    remove_last_block();
    set_rotations();



    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
}

void BlockFall::initialize_grid(const string &input_file) {
    std::ifstream file(input_file);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<int> row;

            int pixel;
            while (iss >> pixel) {
                row.push_back(pixel);
            }
            grid.push_back(row);
        }
        file.close();
    }

    rows = grid.size();
    cols = grid[0].size();

    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main
}

void BlockFall::determine_power_up() {
    Block* lastBlock = initial_block;
    while (lastBlock->next_block != nullptr) {
        lastBlock = lastBlock->next_block;
    }
    power_up = lastBlock->shape;
}

void BlockFall::remove_last_block() {
    Block* penultimateBlock = initial_block;
    while (penultimateBlock->next_block->next_block != nullptr) {
        penultimateBlock = penultimateBlock->next_block;
    }
    delete penultimateBlock->next_block;
    penultimateBlock->next_block = nullptr;
}

void BlockFall::linking_blocks(const string &input_file) {
    std::ifstream file(input_file);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << input_file << std::endl;
        return;
    }

    bool initIsDefined = false;
    initial_block = new Block();
    Block* currentBlock = initial_block;

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<bool> row;

        for (char character : line) {
            if (std::isdigit(character)) {
                row.push_back(character == '1');
            }
        }

        if (line.find('[') != std::string::npos) {
            if (initIsDefined) {
                currentBlock->next_block = new Block();
                currentBlock = currentBlock->next_block;
            }
        } else if (line.find(']') != std::string::npos) {
            initIsDefined = true;
        }

        if (!row.empty()) {
            currentBlock->shape.push_back(row);
            currentBlock->row = currentBlock->shape.size();
            currentBlock->column = currentBlock->shape[0].size();
        }
    }

    file.close();
    active_rotation = initial_block;
}

void BlockFall::set_rotations() {
    Block* block = initial_block;

    while (block != nullptr) {
        Block* current_rotation = block;

        while (!(current_rotation->shape == block->shape && block->right_rotation != nullptr)) {
            vector<vector<bool>> r =rotate_right(current_rotation->shape);

            if (r == block->shape) {
                current_rotation->right_rotation = block;
                block->left_rotation = current_rotation;

            } else {
                auto* right_rotation = new Block();
                right_rotation->shape = r;
                right_rotation->row = r.size();
                right_rotation->column = r[0].size();
                right_rotation->next_block = block->next_block;
                current_rotation->right_rotation = right_rotation;
                right_rotation->left_rotation = current_rotation;
            }
            current_rotation = current_rotation->right_rotation;
        }

        block = block->next_block;
    }
}

vector<vector<bool>> BlockFall::rotate_right(vector<vector<bool>> current_rotation) {
    int row = current_rotation.size();
    int col = current_rotation[0].size();

    std::vector<std::vector<bool>> rotatedMatrix(col, std::vector<bool>(row, 0));

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            rotatedMatrix[j][row - 1 - i] = current_rotation[i][j];
        }
    }

    return rotatedMatrix;
}

BlockFall::~BlockFall() {
    Block* current_block=initial_block;
    while (current_block!= nullptr){
        Block* block1=current_block->next_block;
        while (current_block->right_rotation!=current_block){
            Block* block2=current_block->right_rotation;
            block2->right_rotation->left_rotation=current_block;
            current_block->right_rotation=block2->right_rotation;
            delete block2;
        }
        delete current_block;
        current_block=block1;
    }


    // TODO: Free dynamically allocated memory used for storing game blocks
}