#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <iomanip>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry *new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).

    int entry_num = 0;
    LeaderboardEntry *entry = head_leaderboard_entry;

    while (entry != nullptr) {
        entry_num++;
        entry = entry->next_leaderboard_entry;
    }

    if (entry_num < MAX_LEADERBOARD_SIZE) {
        LeaderboardEntry *prev_entry = nullptr;
        LeaderboardEntry *current_entry = head_leaderboard_entry;

        while (current_entry != nullptr && new_entry->score <= current_entry->score) {
            prev_entry = current_entry;
            current_entry = current_entry->next_leaderboard_entry;
        }

        if (prev_entry != nullptr) {
            prev_entry->next_leaderboard_entry = new_entry;
        } else {
            head_leaderboard_entry = new_entry;
        }

        new_entry->next_leaderboard_entry = current_entry;
    } else {
        LeaderboardEntry *prev_entry = nullptr;

        for (LeaderboardEntry *current_entry = head_leaderboard_entry; current_entry != nullptr; current_entry = current_entry->next_leaderboard_entry) {
            if (new_entry->score > current_entry->score) {
                //remove last
                LeaderboardEntry *last_entry = current_entry;
                LeaderboardEntry *penultimate_entry = prev_entry;

                while (last_entry->next_leaderboard_entry != nullptr) {
                    penultimate_entry = last_entry;
                    last_entry = last_entry->next_leaderboard_entry;
                }
                delete penultimate_entry->next_leaderboard_entry;
                penultimate_entry->next_leaderboard_entry = nullptr;

                //add new
                if (prev_entry != nullptr) {
                    prev_entry->next_leaderboard_entry = new_entry;
                    new_entry->next_leaderboard_entry = current_entry;
                } else {
                    head_leaderboard_entry = new_entry;
                    head_leaderboard_entry->next_leaderboard_entry = current_entry;
                }
                break;
            }
            prev_entry = current_entry;
        }
    }
}

void Leaderboard::write_to_file(const string &filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions

    std::ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    LeaderboardEntry *leaderboard_entry = head_leaderboard_entry;

    while (leaderboard_entry != nullptr) {
        outputFile << leaderboard_entry->score << " ";
        outputFile << leaderboard_entry->last_played << " ";
        outputFile << leaderboard_entry->player_name << std::endl;

        leaderboard_entry = leaderboard_entry->next_leaderboard_entry;
    }
}

void Leaderboard::read_from_file(const string &filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "The file is not exist: " << filename << std::endl;
        return;
    }

    int entry_num = 0;

    std::string line;
    while (std::getline(file, line) && entry_num < MAX_LEADERBOARD_SIZE) {
        if (line.size() == 0) {
            continue;
        }

        std::istringstream iss(line);
        std::vector<std::string> infos{
                std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>{}
        };


        LeaderboardEntry *new_entry = new LeaderboardEntry(std::stoul(infos[0]), std::stol(infos[1]), infos[2]);

        insert_new_entry(new_entry);
        entry_num++;
    }
}

void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions

    std::cout << "Leaderboard\n-----------" << std::endl;

    int order = 1;
    LeaderboardEntry *leaderboard_entry = head_leaderboard_entry;

    while (leaderboard_entry != nullptr) {
        std::cout << order << ". ";
        std::cout << leaderboard_entry->player_name << " ";
        std::cout << leaderboard_entry->score << " ";
        std::tm *time_info = std::localtime(&leaderboard_entry->last_played);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", time_info);

        std::cout << buffer << std::endl;

        leaderboard_entry = leaderboard_entry->next_leaderboard_entry;
        order++;
    }
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    LeaderboardEntry *current_entry = head_leaderboard_entry;
    LeaderboardEntry *next_entry;

    while (current_entry != nullptr) {
        next_entry = current_entry->next_leaderboard_entry;
        delete current_entry;
        current_entry = next_entry;
    }
}
