#include <iostream>
#include <fstream>
#include <algorithm>
#include "GameController.h"

bool GameController::play(BlockFall& game, const string& commands_file){
    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.

    game.high_score = game.leaderboard.head_leaderboard_entry == nullptr ? 0 : game.leaderboard.head_leaderboard_entry->score;
    std::ifstream file(commands_file);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << commands_file << std::endl;
        return false;
    }

    placing(game);

    std::string line;
    while (std::getline(file, line)) {
        if (line == "PRINT_GRID") {
            std::cout << "Score: " << game.current_score << std::endl;
            std::cout << "High Score: " << game.high_score << std::endl;
            print_grid(game);
        } else if (line == "MOVE_RIGHT") {
            move_right(game);
        } else if (line == "MOVE_LEFT") {
            move_left(game);
        } else if (line == "ROTATE_RIGHT") {
            right_rotation(game);
        } else if (line == "ROTATE_LEFT") {
            left_rotation(game);
        } else if (line == "DROP") {
            if (game.gravity_mode_on) {
                drop_with_gravity(game);
            } else {
                drop_no_gravity(game);
            }

            //power_up
            if(exist_powerup(game)) {
                power_up(game);
            } else {
                clean_filled_row(game, true);
            }

            block_row = 0;
            block_column = 0;

            if (game.active_rotation->next_block == nullptr) {
                std::cout << "YOU WIN!\nNo more blocks." << std::endl;
                finish_game(game);
                return true;
            }
            game.active_rotation = game.active_rotation->next_block;

            if (game_over(game)) {
                std::cout << "GAME OVER!\nNext block that couldn't fit:" << std::endl;

                for (int i = 0; i < game.active_rotation->row; ++i) {
                    for (int j = 0; j < game.active_rotation->column; ++j) {
                        std::cout << (game.active_rotation->shape[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
                finish_game(game);
                return false;
            }
        } else if (line == "GRAVITY_SWITCH") {
            gravity_switch(game);
            if(exist_powerup(game)) {
                power_up(game);
            }
            clean_filled_row(game, false);
        } else {
            std::cout << "Unknown command: " << line << std::endl;
        }
        game.high_score = game.current_score >= game.high_score ? game.current_score : game.high_score;
        placing(game);
    }
    file.close();

    clean(game);
    std::cout << "GAME FINISHED!\nNo more commands." << std::endl;
    finish_game(game);

    return true;
}

void GameController::placing(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->row; i++) {
        for (int j = 0; j < game.active_rotation->column; j++) {
            if (game.grid[i + block_row][j + block_column] == 0) {
                game.grid[i + block_row][j + block_column] = game.active_rotation->shape[i][j];
            }
        }
    }
}

void GameController::clean(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->row; i++) {
        for (int j = 0; j < game.active_rotation->column; j++) {
            if (game.active_rotation->shape[i][j] == 1) {
                game.grid[i + block_row][j + block_column] = 0;
            }
        }
    }
}

void GameController::move_right(BlockFall& game) {
    if (block_column + game.active_rotation->column < game.cols  &&  can_move_right(game)) {
        placing(game);
    }
}

void GameController::move_left(BlockFall& game) {
    if (block_column > 0 && can_move_left(game)) {
        placing(game);
    }
}

bool GameController::can_move_right(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->row; i++) {
        for (int j = game.active_rotation->column; j > block_column ; j--) {
            if (game.active_rotation->shape[i][j - 1] == 1) {
                if (game.grid[block_row + i][block_column + j] == 1) {
                    return false;
                } else if (game.grid[block_row + i][block_column + j] == 0) {
                    break;
                }
            }
        }
    }
    clean(game);
    block_column += 1;
    return true;
}

bool GameController::can_move_left(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->row; i++) {
        for (int j = block_column - 1; j < block_column + game.active_rotation->column; j++) {
            if (game.grid[i][j + 1] == 1) {
                if (game.grid[i][j] == 1) {
                    return false;
                } else if (game.grid[i][j] == 0) {
                    break;
                }
            }
        }
    }
    clean(game);
    block_column -= 1;
    return true;
}

void GameController::right_rotation(BlockFall& game) {
    clean(game);
    if (can_right_rotate(game)) {
        game.active_rotation = game.active_rotation->right_rotation;
    }
    placing(game);
}

void GameController::left_rotation(BlockFall& game) {
    clean(game);
    if (can_left_rotate(game)) {
        game.active_rotation = game.active_rotation->left_rotation;
    }
    placing(game);
}

bool GameController::can_right_rotate(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->column; i++) {
        for (int j = 0; j < game.active_rotation->row; j++) {
            if (i + block_row >= game.rows || j + block_column >= game.cols || (game.active_rotation->right_rotation->shape[i][j] == 1 && game.grid[block_row + i][block_column + j] == 1))
                return false;
        }
    }
    return true;
}

bool GameController::can_left_rotate(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->column; i++) {
        for (int j = 0; j < game.active_rotation->row; j++) {
            if (i >= game.rows || j >= game.cols || (game.active_rotation->left_rotation->shape[i][j] == 1 && game.grid[block_row + i][block_column + j] == 1))
                return false;
        }
    }
    return true;
}

void GameController::drop_no_gravity(BlockFall& game) {
    int distance = -1;

    for (int i = block_column; i < block_column + game.active_rotation->column; i++) {
        int empty_space = 0;
        int index_bottom_cell = game.active_rotation->row;

        for (int j = game.active_rotation->row; j > block_row ; j--) {
            if (game.grid[j - 1][i] == 1) {
                break;
            } else {
                index_bottom_cell--;
            }
        }

        for (int j = index_bottom_cell; j < game.rows; j++) {
            if (game.grid[j][i] == 1) {
                break;
            }
            empty_space++;
        }

        if (distance == -1 || empty_space < distance) {
            distance = empty_space;
        }
    }

    int filled_num = 0;
    for (const auto& boolVector : game.active_rotation->shape) {
        filled_num += std::count(boolVector.begin(), boolVector.end(), true);
    }
    game.current_score += distance * filled_num;

    clean(game);
    block_row += distance;
    placing(game);
}

void GameController::drop_with_gravity(BlockFall& game) {
    int cell_num = 0;
    int filled_num = 0;

    for (int i = 0; i < game.active_rotation->column; i++) {
        for (int j = game.active_rotation->row - 1; j >= 0; j--) {
            if (game.active_rotation->shape[j][i] == 1) {
                int distance = 0;

                for (int k = block_row + j + 1; k < game.rows; k++) {
                    if (game.grid[k][block_column + i] == 1) {
                        break;
                    }
                    distance++;
                }
                game.grid[block_row + j][block_column + i] = 0;
                game.grid[block_row + j + distance][block_column + i] = 1;
                filled_num += 1;
                cell_num = cell_num == 0 || distance < cell_num ? distance : cell_num;
            }
        }
    }
    game.current_score += filled_num * cell_num;
}

void GameController::gravity_switch(BlockFall& game) {
    game.gravity_mode_on = !game.gravity_mode_on;

    if (game.gravity_mode_on) {
        clean(game);
        for (int i = 0; i < game.cols; i++) {
            for (int j = game.rows - 1; j >= 0; j--) {
                if (game.grid[j][i] == 1) {
                    int distance = 0;

                    for (int k = j + 1; k < game.rows; k++) {
                        if (game.grid[k][i] == 1) {
                            break;
                        }
                        distance++;
                    }
                    game.grid[j][i] = 0;
                    game.grid[j + distance][i] = 1;
                }
            }
        }
        placing(game);
    }
}

void GameController::print_grid(BlockFall& game) {
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            std::cout << (game.grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}

bool GameController::exist_powerup(BlockFall& game) {
    for (int i = 0; i <= game.rows - game.power_up.size(); i++) {
        for (int j = 0; j <= game.cols - game.power_up[0].size(); j++) {
            vector<vector<bool>> sub_vector;

            for (size_t k = i; k < i + game.power_up.size(); k++) {
                std::vector<bool> rowVec(game.grid[k].begin() + j, game.grid[k].begin() + j + game.power_up[0].size());
                sub_vector.push_back(rowVec);
            }

            if (sub_vector == game.power_up)
                return true;
        }
    }
    return false;
}

void GameController::clean_filled_row(BlockFall &game, bool drop) {
    bool is_cleaned = false;

    for (auto& row: game.grid) {
        bool all_filled = std::all_of(row.begin(), row.end(), [](bool val) { return val; });
        if (all_filled) {
            is_cleaned = all_filled;
            break;
        }
    }

    if (is_cleaned && drop) {
        std::cout << "Before clearing:\n";
        print_grid(game);
    }

    for (auto& row: game.grid) {
        bool all_filled = std::all_of(row.begin(), row.end(), [](bool val) { return val; });
        if (all_filled) {
            std::fill(row.begin(), row.end(), false);
            game.current_score += game.cols;
        }
    }

    int distance = 0;

    for (int i = game.rows - 1; i > 0; i--) {
        bool is_empty_row = std::none_of(game.grid[i].begin(), game.grid[i].end(), [](bool val) { return val; });

        if (is_empty_row) {
            distance++;
        }

        for (int j = 0; j < game.cols; ++j) {
            if (drop) {
                game.grid[i + distance - 1][j] = game.grid[i - 1][j];
            } else {
                bool out_of_bounds = (i - 1 < block_row || i - 1 >= block_row + game.active_rotation->row ||
                                      j < block_column || j >= block_column + game.active_rotation->column);

                if (out_of_bounds) {
                    game.grid[i + distance - 1][j] = game.grid[i - 1][j];
                }
            }

        }
    }

    for (int i = distance - 1; i >= 0; i--) {
        for (int j = 0; j < game.cols; ++j) {
            bool out_of_bounds = (i < block_row || i >= block_row + game.active_rotation->row ||
                                  j < block_column || j >= block_column + game.active_rotation->column);

            if (out_of_bounds || drop) {
                game.grid[i][j] = 0;
            }

        }
    }
}

void GameController::finish_game(BlockFall& game) {
    std::cout << "Final grid and score:\n" << std::endl;
    std::cout << "Score: " << game.current_score << std::endl;
    std::cout << "High Score: " << game.high_score << std::endl;
    print_grid(game);

    LeaderboardEntry* new_entry = new LeaderboardEntry(game.current_score, time(nullptr), game.player_name);
    game.leaderboard.insert_new_entry(new_entry);
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    game.leaderboard.print_leaderboard();
}

bool GameController::game_over(BlockFall& game) {
    for (int i = 0; i < game.active_rotation->row; i++) {
        for (int j = 0; j < game.active_rotation->column; j++) {
            if (game.grid[i][j] == 1 && game.active_rotation->shape[i][j] == 1) {
                return true;
            }
        }
    }
    return false;
}

void GameController::power_up(BlockFall& game) {
    std::cout << "Before clearing:\n";
    print_grid(game);
    game.current_score += 1000;

    int count_1s = 0;
    for (const auto& boolVector : game.grid) {
        count_1s += std::count(boolVector.begin(), boolVector.end(), 1);
    }
    game.current_score += count_1s;

    for (auto& row : game.grid) {
        std::fill(row.begin(), row.end(), 0);
    }
}