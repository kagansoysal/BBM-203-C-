#include "Network.h"

#include <utility>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    this->message_limit = message_limit;
    this->sender_port = sender_port;
    this->receiver_port = receiver_port;

    for (const string &command: commands) {
        cout << std::string(command.length() + 9, '-') << endl;
        cout << "Command: " << command << endl;
        cout << std::string(command.length() + 9, '-') << endl;

        string line = command;
        string message;
        if (command[0] == 'M') {
            size_t start = command.find('#');
            size_t end = command.rfind('#');

            line = command.substr(0, start);
            message = command.substr(start + 1, end - start - 1);
        }

        std::istringstream iss(line);
        std::vector<std::string> infos{
                std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>{}
        };

        if (infos[0] == "MESSAGE") {
            create_message(clients, infos[1], infos[2], message);
        } else if (infos[0] == "SEND") {
            sending(clients);
        } else if (infos[0] == "RECEIVE") {
            receive(clients);
        } else if (infos[0] == "SHOW_Q_INFO") {
            show_q(clients, infos[1], infos[2]);
        } else if (infos[0] == "SHOW_FRAME_INFO") {
            show_frame(clients, infos[1], infos[2], stoi(infos[3]));
        } else if (infos[0] == "PRINT_LOG") {
            print_log(clients, infos[1]);
        } else {
            cout << "Invalid command." << endl;
        }
    }
}

void Network::print_log(vector<Client> &clients, const string &client_id) {
    Client *client = find_client_2(clients, client_id);
    int log_entry = 1;
    string activity_type;

    for (const Log &log: client->log_entries) {
        if (log_entry == 1)
            std::cout << "Client " << client_id << " Logs:" << std::endl;

        if (log.activity_type == ActivityType::MESSAGE_FORWARDED) {
            activity_type = "Forwarded";
        } else if (log.activity_type == ActivityType::MESSAGE_SENT) {
            activity_type = "Sent";
        } else if (log.activity_type == ActivityType::MESSAGE_RECEIVED) {
            activity_type = "Received";
        } else if (log.activity_type == ActivityType::MESSAGE_DROPPED) {
            activity_type = "Dropped";
        }

        std::cout << "--------------" << std::endl;
        std::cout << "Log Entry #" << log_entry << ":" << std::endl;
        std::cout << "Activity: Message " << activity_type << std::endl;
        std::cout << "Timestamp: " << log.timestamp << std::endl;
        std::cout << "Number of frames: " << log.number_of_frames << std::endl;
        std::cout << "Number of hops: " << log.number_of_hops << std::endl;
        std::cout << "Sender ID: " << log.sender_id << std::endl;
        std::cout << "Receiver ID: " << log.receiver_id << std::endl;
        std::cout << "Success: " << (log.success_status ? "Yes" : "No") << std::endl;

        if (log.activity_type == ActivityType::MESSAGE_SENT || log.activity_type == ActivityType::MESSAGE_RECEIVED) {
            std::cout << "Message: \"" << log.message_content << "\"" << std::endl;
        }

        log_entry++;
    }
}

void Network::receive(vector<Client> &clients) {
    for (Client &receiver_client: clients) {
        string message;
        int frame_num = 1;

        while (!receiver_client.incoming_queue.empty()) {
            stack<Packet *> frame = receiver_client.incoming_queue.front();

            //frame'i aç
            std::vector<Packet *> packets;

            while (!frame.empty()) {
                packets.push_back(frame.top());
                frame.pop();
            }


            auto *physical_layer = dynamic_cast<PhysicalLayerPacket *>(packets[0]);
            auto *network_layer = dynamic_cast<NetworkLayerPacket *>(packets[1]);
            auto *transport_layer = dynamic_cast<TransportLayerPacket *>(packets[2]);
            auto *app_layer = dynamic_cast<ApplicationLayerPacket *>(packets[3]);

            int number_of_hops = count_hop_num(clients, find_client_mac(clients, app_layer->sender_ID),
                                               receiver_client.client_mac, app_layer->receiver_ID);

            //check if message sent to itself
            if (physical_layer->receiver_MAC_address == find_client_mac(clients, app_layer->receiver_ID)) {

                std::cout << "Client " << receiver_client.client_id << " receiving frame #" << frame_num
                          << " from client "
                          << find_client(clients, physical_layer->sender_MAC_address)->client_id
                          << ", originating from client " << app_layer->sender_ID << std::endl;
                report_frame(physical_layer, network_layer, transport_layer, app_layer, number_of_hops);
                message += app_layer->message_data;

                if (end_of_sentence(message)) {
                    std::cout << "Client " << app_layer->receiver_ID << " received the message \"" << message
                              << "\" from client " << app_layer->sender_ID << ".\n--------" << endl;

                    log_entry(message, frame_num, number_of_hops, app_layer, true, ActivityType::MESSAGE_RECEIVED,
                              &receiver_client);
                    message = "";
                    frame_num = 0;
                }

                delete app_layer;
                delete transport_layer;
                delete network_layer;
                delete physical_layer;

            } else {
                string neighbor_id = find_client(clients, physical_layer->receiver_MAC_address)->routing_table.at(
                        app_layer->receiver_ID);
                string neighbor_mac = find_client_mac(clients, neighbor_id);

                if (neighbor_mac.empty()) { //dropped
                    std::cout << "Client " << receiver_client.client_id << " receiving frame #" << frame_num
                              << " from client "
                              << find_client(clients, physical_layer->sender_MAC_address)->client_id
                              << ", but intended for client " << app_layer->receiver_ID
                              << ". Forwarding..." << std::endl;

                    std::cout << "Error: Unreachable destination. Packets are dropped after " << number_of_hops
                              << " hops!" << std::endl;

                    message += app_layer->message_data;

                    if (end_of_sentence(message)) {
                        std::cout << "--------" << endl;

                        log_entry(message, frame_num, number_of_hops, app_layer, false, ActivityType::MESSAGE_DROPPED,
                                  &receiver_client);

                        message = "";
                        frame_num = 0;
                    }

                    delete app_layer;
                    delete transport_layer;
                    delete network_layer;
                    delete physical_layer;

                } else { //forward
                    if (frame_num == 1) {
                        std::cout << "Client " << receiver_client.client_id << " receiving a message from client "
                                  << find_client(clients, physical_layer->sender_MAC_address)->client_id
                                  << ", but intended for client " << app_layer->receiver_ID << ". Forwarding..."
                                  << std::endl;
                    }
                    std::cout << "Frame #" << frame_num << " MAC address change: New sender MAC "
                              << receiver_client.client_mac << ", new receiver MAC " << neighbor_mac << std::endl;

                    physical_layer->sender_MAC_address = receiver_client.client_mac;
                    physical_layer->receiver_MAC_address = neighbor_mac;

                    std::reverse(packets.begin(), packets.end());
                    for (Packet *packet: packets)
                        frame.push(packet);

                    receiver_client.outgoing_queue.push(frame);
                    message += app_layer->message_data;

                    if (end_of_sentence(message)) {
                        std::cout << "--------" << endl;

                        log_entry(message, frame_num, number_of_hops, app_layer, true, ActivityType::MESSAGE_FORWARDED,
                                  &receiver_client);

                        message = "";
                        frame_num = 0;
                    }
                }

            }
            frame_num++;
            receiver_client.incoming_queue.pop();
        }
    }
}

void Network::sending(vector<Client> &clients) {
    for (Client &sender_client: clients) {
        string message;
        int frame_num = 1;

        while (!sender_client.outgoing_queue.empty()) {
            std::vector<Packet *> packets;

            while (!sender_client.outgoing_queue.front().empty()) {
                packets.push_back(sender_client.outgoing_queue.front().top());
                sender_client.outgoing_queue.front().pop();
            }

            auto *physical_layer = dynamic_cast<PhysicalLayerPacket *>(packets[0]);
            auto *network_layer = dynamic_cast<NetworkLayerPacket *>(packets[1]);
            auto *transport_layer = dynamic_cast<TransportLayerPacket *>(packets[2]);
            auto *app_layer = dynamic_cast<ApplicationLayerPacket *>(packets[3]);

            string receiver_mac = physical_layer->receiver_MAC_address;
            Client *receiver_client = find_client(clients, receiver_mac);

            int number_of_hops = count_hop_num(clients, find_client_mac(clients, app_layer->sender_ID),
                                               physical_layer->receiver_MAC_address, app_layer->receiver_ID);
            std::cout << "Client " << sender_client.client_id << " sending frame #" << frame_num << " to client "
                      << receiver_client->client_id << std::endl;
            report_frame(physical_layer, network_layer, transport_layer, app_layer, number_of_hops);

            std::reverse(packets.begin(), packets.end());
            for (Packet *packet: packets)
                sender_client.outgoing_queue.front().push(packet);

            receiver_client->incoming_queue.push(sender_client.outgoing_queue.front());
            sender_client.outgoing_queue.pop();

            message += app_layer->message_data;

            if (end_of_sentence(message)) {
                message = "";
                frame_num = 0;
            }
            frame_num++;
        }
    }
}

void Network::log_entry(const string &message, int frame_num, int number_of_hops, ApplicationLayerPacket *app_layer,
                        bool success, ActivityType activity_type, Client *client) {
    client->log_entries.emplace_back("2023-11-22 20:30:03", message, frame_num, number_of_hops, app_layer->sender_ID,
                                                                        app_layer->receiver_ID, success, activity_type);
}

void Network::report_frame(PhysicalLayerPacket *physical_layer, NetworkLayerPacket *network_layer,
                           TransportLayerPacket *transport_layer, ApplicationLayerPacket *app_layer,
                           int number_of_hops) {
    physical_layer->print();
    network_layer->print();
    transport_layer->print();
    app_layer->print();
    std::cout << "Message chunk carried: " << "\"" << app_layer->message_data << "\"" << std::endl;
    std::cout << "Number of hops so far: " << number_of_hops << std::endl;
    std::cout << "--------" << std::endl;
}

void Network::create_message(vector<Client> &clients, const string &sender_id, const string &receiver_id,
                             const string &message) {
    cout << "Message to be sent: \"" << message << "\"\n" << endl;

    int frame_num = 1;
    int remain = message.length();

    while (remain > 0) {
        int chunk_size = std::min(message_limit, remain);
        string chunk = message.substr(message.length() - remain, chunk_size);

        stack<Packet *> frame;

        string sender_ip;
        string sender_mac;
        string receiver_ip;
        string receiver_mac;

        for (const Client &client: clients)
            if (client.client_id == sender_id) {
                sender_ip = client.client_ip;
                sender_mac = client.client_mac;
            } else if (client.client_id == receiver_id) {
                receiver_ip = client.client_ip;
            }
        receiver_mac = find_receiver_mac(clients, sender_mac, receiver_id);

        auto *app_layer = new ApplicationLayerPacket(APPLICATION_LAYER_ID, sender_id, receiver_id, chunk);
        auto *transport_layer = new TransportLayerPacket(TRANSPORT_LAYER_ID, sender_port, receiver_port);
        auto *network_layer = new NetworkLayerPacket(NETWORK_LAYER_ID, sender_ip, receiver_ip);
        auto *physical_layer = new PhysicalLayerPacket(PHYSICAL_LAYER_ID, sender_mac, receiver_mac);

        frame.push(app_layer);
        frame.push(transport_layer);
        frame.push(network_layer);
        frame.push(physical_layer);

        find_client_2(clients, sender_id)->outgoing_queue.push(frame);

        int number_of_hops = count_hop_num(clients, sender_mac, sender_mac, receiver_id);
        cout << "Frame #" << frame_num << endl;
        report_frame(physical_layer, network_layer, transport_layer, app_layer, number_of_hops);

        if (end_of_sentence(chunk)) {
            log_entry(message, frame_num, number_of_hops, app_layer, true, ActivityType::MESSAGE_SENT,
                      find_client(clients, sender_mac));
            return;
        }
        remain -= chunk_size;
        frame_num++;
    }
}

void Network::show_q(vector<Client> &clients, const string &client_id, const string &queue_type) {
    Client *client = find_client_2(clients, client_id);
    queue<stack<Packet *>> queue = queue_type == "in" ? client->incoming_queue : client->outgoing_queue;
    string queue_name = queue_type == "in" ? "Incoming Queue" : "Outgoing Queue";

    std::cout << "Client " << client_id << " " << queue_name << " Status" << std::endl;
    std::cout << "Current total number of frames: " << queue.size() << endl;
}

void Network::show_frame(vector<Client> &clients, const string &client_id, const string &queue_type, int frame_num) {
    Client *client = find_client_2(clients, client_id);
    queue<stack<Packet *>> queue = queue_type == "in" ? client->incoming_queue : client->outgoing_queue;
    string queue_name = queue_type == "in" ? "incoming queue" : "outgoing queue";

    if (frame_num > queue.size()) {
        std::cout << "No such frame." << std::endl;
        return;
    }

    int order = 1;
    stack<Packet *> frame;

    while (!queue.empty()) {
        if (order == frame_num) {
            frame = queue.front();
            break;
        }
        queue.pop();
        order++;
    }

    std::vector<Packet *> packets;

    while (!frame.empty()) {
        packets.push_back(frame.top());
        frame.pop();
    }

    auto *physical_layer = dynamic_cast<PhysicalLayerPacket *>(packets[0]);
    auto *network_layer = dynamic_cast<NetworkLayerPacket *>(packets[1]);
    auto *transport_layer = dynamic_cast<TransportLayerPacket *>(packets[2]);
    auto *app_layer = dynamic_cast<ApplicationLayerPacket *>(packets[3]);

    int number_of_hop = count_hop_num(clients, find_client_mac(clients, app_layer->sender_ID),
                                      find_client_mac(clients, client_id), app_layer->receiver_ID);

    std::cout << "Current Frame #" << frame_num << " on the " << queue_name << " of client " << client_id << std::endl;
    std::cout << "Carried Message: \"" << app_layer->message_data << "\"" << std::endl;
    std::cout << "Layer 0 info: ";
    app_layer->print();
    std::cout << "Layer 1 info: ";
    transport_layer->print();
    std::cout << "Layer 2 info: ";
    network_layer->print();
    std::cout << "Layer 3 info: ";
    physical_layer->print();
    std::cout << "Number of hops so far: " << number_of_hop << std::endl;
}

int Network::count_hop_num(vector<Client> &clients, string sender_mac, const string &current_mac,
                           const string &receiver_id) {
    int number_of_hop = 0;
    string neighbor_mac = std::move(sender_mac);

    while (neighbor_mac != current_mac) {
        number_of_hop++;
        neighbor_mac = find_receiver_mac(clients, neighbor_mac, receiver_id);
    }
    return number_of_hop;
}

string Network::find_client_mac(vector<Client> &clients, const string &client_id) {
    for (Client &client: clients) {
        if (client.client_id == client_id) {
            return client.client_mac;
        }
    }
    return "";
}

Client *Network::find_client(vector<Client> &clients, const string &client_mac) {
    for (Client &client: clients) {
        if (client.client_mac == client_mac) {
            return &client;
        }
    }
    return nullptr;
}

Client *Network::find_client_2(vector<Client> &clients, const string &client_id) {
    for (Client &client: clients) {
        if (client.client_id == client_id) {
            return &client;
        }
    }
    return nullptr;
}

string Network::find_receiver_mac(vector<Client> &clients, const string &sender_mac, const string &receiver_id) {
    for (const Client &client: clients) {
        if (client.client_mac == sender_mac) {
            string neighbor_id = client.routing_table.at(receiver_id);

            for (const Client &client2: clients) {
                if (client2.client_id == neighbor_id) {
                    return client2.client_mac;
                }
            }
        }
    }
    return "";
}

bool Network::end_of_sentence(const string &message) {
    return std::any_of(message.begin(), message.end(), [](char c) {
        return c == '.' || c == '!' || c == '?';
    });
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> infos{
                    std::istream_iterator<std::string>{iss},
                    std::istream_iterator<std::string>{}
            };

            if (infos.size() != 3) continue;
            clients.emplace_back(infos[0], infos[1], infos[2]);
        }
        file.close();
    }
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.

    std::ifstream file(filename);

    if (!file.is_open()) return;

    int client_num{};
    std::string line;
    while (std::getline(file, line)) {
        if (line == "-") {
            client_num++;
            continue;
        }

        std::istringstream iss(line);
        std::vector<std::string> infos{
                std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>{}
        };

        clients[client_num].routing_table.insert(std::make_pair(infos[0], infos[1]));
    }
    file.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        int line_count{};

        while (std::getline(file, line)) {
            if (line_count > 0)
                commands.push_back(line);
            line_count++;
        }
        file.close();

        return commands;
    }
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.

}
