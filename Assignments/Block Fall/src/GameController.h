#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay

private:
    int block_row = 0;
    int block_column = 0;

    void print_grid(BlockFall &game);

    bool can_move_right(BlockFall &game);

    void placing(BlockFall &game);

    void clean(BlockFall &game);

    bool can_move_left(BlockFall &game);

    void right_rotation(BlockFall &game);

    void left_rotation(BlockFall &game);

    void drop_no_gravity(BlockFall &game);

    bool can_left_rotate(BlockFall &game);

    void gravity_switch(BlockFall &game);

    bool can_right_rotate(BlockFall &game);

    void drop_with_gravity(BlockFall &game);

    void move_right(BlockFall &game);

    void move_left(BlockFall &game);

    bool exist_powerup(BlockFall &game);

    void clean_filled_row(BlockFall &game, bool drop);

    void finish_game(BlockFall &game);

    bool game_over(BlockFall &game);

    void power_up(BlockFall &game);

    void print_block(BlockFall &game);
};


#endif //PA2_GAMECONTROLLER_H
