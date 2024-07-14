#include "udp.hpp"

/*====================================/
                SERVER
/====================================*/

UDP::Server::Server() {};
UDP::Server::~Server() {
    enet_host_destroy(sv);
};

bool UDP::Server::create(int conn, int chan, int port) {
    sv_addr.port = port;
    sv_addr.host = ENET_HOST_ANY;
    return create_host(conn, chan);
};

bool UDP::Server::create(int conn, int chan, const char* ip, int port) {
    enet_address_set_host(&sv_addr, ip);
    sv_addr.port = port;
    return create_host(conn, chan);
};

bool UDP::Server::create_host(int n_connections, int n_channels) {
    sv = enet_host_create(&sv_addr, n_connections, n_channels, 0, 0);
    return !!sv;
};

void UDP::Server::listen() {
    if (listen_daemon_thread) stop_listen();
    listening = true;
    listen_daemon_thread = new std::thread(&listen_daemon, this);
};

void UDP::Server::stop_listen() {
    listening = false;
    if (!listen_daemon_thread) return;
    if (listen_daemon_thread->joinable())
        listen_daemon_thread->join();
    delete listen_daemon_thread;
};

void UDP::Server::listen_daemon() {
    while (listening)
        while (enet_host_service(sv, &ev, 1000) > 0) {
            std::string cli_id = get_host_id(ev.peer);
            PacketInfo pack;
            pack.host_id = cli_id;
            switch (ev.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                pack.type = pack_connect;
                clients.insert_or_assign(cli_id, ev.peer);
                break;
            };
            case ENET_EVENT_TYPE_DISCONNECT: {
                pack.type = pack_disconnect;
                clients.erase(cli_id);
                break;
            };
            case ENET_EVENT_TYPE_RECEIVE: {
                pack.type = pack_recieve;
                pack.data = uint8_to_string(ev.packet->data, ev.packet->dataLength);
                break;
            };
            };
            packet_buffer.push(pack);
            enet_packet_destroy(ev.packet);
        };
};

bool UDP::Server::get_packet(std::string& data) {
    PacketInfo alldata;
    if (!get_packet(alldata)) return false;
    data = alldata.data;
    return true;
};

bool UDP::Server::get_packet(PacketInfo& alldata) {
    if (packet_buffer.size() == 0) return false;
    return packet_buffer.pop(alldata);
};

void UDP::Server::broadcast_packet(std::string data) {
    broadcast_packet(0, data);
};

void UDP::Server::broadcast_packet(int chan, std::string data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(sv, chan, packet);
};

void UDP::Server::send_packet(std::string host_id, std::string data) {
    send_packet(host_id, 0, data);
};

void UDP::Server::send_packet(std::string host_id, int chan, std::string data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(clients.at(host_id), chan, packet);
};

/*====================================/
                F_SERVER
/====================================*/

UDP::F_Server::F_Server() {};
UDP::F_Server::~F_Server() {
    enet_host_destroy(sv);
};

bool UDP::F_Server::create(int conn, int chan, int port) {
    sv_addr.port = port;
    sv_addr.host = ENET_HOST_ANY;
    return create_host(conn, chan);
};

bool UDP::F_Server::create(int conn, int chan, const char* ip, int port) {
    enet_address_set_host(&sv_addr, ip);
    sv_addr.port = port;
    return create_host(conn, chan);
};

bool UDP::F_Server::create_host(int n_connections, int n_channels) {
    sv = enet_host_create(&sv_addr, n_connections, n_channels, 0, 0);
    return !!sv;
};

void UDP::F_Server::listen() {
    if (listen_daemon_thread) stop_listen();
    listening = true;
    listen_daemon_thread = new std::thread(&listen_daemon, this);
};

void UDP::F_Server::stop_listen() {
    listening = false;
    if (!listen_daemon_thread) return;
    if (listen_daemon_thread->joinable())
        listen_daemon_thread->join();
    delete listen_daemon_thread;
};

void UDP::F_Server::listen_daemon() {
    while (listening)
        while (enet_host_service(sv, &ev, 1000) > 0) {
            if (func) func(ev);
            enet_packet_destroy(ev.packet);
        };
};

void UDP::F_Server::broadcast_packet(std::string data) {
    broadcast_packet(0, data);
};

void UDP::F_Server::broadcast_packet(int chan, std::string data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(sv, chan, packet);
};

void UDP::F_Server::send_packet(ENetPeer* peer, std::string data) {
    send_packet(peer, 0, data);
};

void UDP::F_Server::send_packet(ENetPeer* peer, int chan, std::string data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, chan, packet);
};

void UDP::F_Server::setCallBack(void (*f)(ENetEvent&)) {
    func = f;
};