#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename);

private:
    int message_limit;
    string sender_port;
    string receiver_port;

    void create_message(vector<Client> &clients,const string &sender_id, const string &receiver_id, const string &message);

    string find_receiver_mac(vector<Client> &clients,const string& sender_mac, const string& receiver_id);

    Client * find_client(vector<Client> &clients,const string &client_mac);

    void sending(vector<Client> &clients);

    int count_hop_num(vector<Client> &clients,string sender_mac, const string &current_mac, const string &receiver_id);

    string find_client_mac(vector<Client> &clients,const string &client_id);

    void receive(vector<Client> &clients);

    static bool end_of_sentence(const string &message);

    static void report_frame(PhysicalLayerPacket *physical_layer, NetworkLayerPacket *network_layer,
                      TransportLayerPacket *transport_layer, ApplicationLayerPacket *app_layer, int number_of_hops);

    static void
    log_entry(const string &message, int frame_num, int number_of_hops, ApplicationLayerPacket *app_layer, bool success,
              ActivityType activity_type, Client *client);

    Client *find_client_2(vector<Client> &clients,const string &client_id);

    void show_q(vector<Client> &clients,const string &client_id, const string &queue_type);

    void show_frame(vector<Client> &clients,const string &client_id, const string &queue_type, int frame_num);

    void print_log(vector<Client> &clients,const string &client_id);
};

#endif  // NETWORK_H
