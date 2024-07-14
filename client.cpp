#include "udp.hpp"

bool UDP::init_udp() {
    if (enet_initialize()) return false;
    atexit(enet_deinitialize);
    return true;
};

UDP::std::string UDP::uint8_to_string(enet_uint8* data, size_t data_length) {
    std::string msg;
    for (size_t i = 0; i < data_length; i++)
        msg.push_back(data[i]);
    return msg;
};

UDP::std::string UDP::get_host_id(ENetPeer* host) {
    return std::to_string(host->address.host) + ":" + std::to_string(host->address.port);
};

/*====================================/
                CLIENT
/====================================*/

UDP::Client::Client() {};
UDP::Client::~Client() {
    enet_host_destroy(cli);
};

bool UDP::Client::create(int conn, int chan) {
    cli = enet_host_create(nullptr, conn, chan, 0, 0);
    return !!cli;
};

void UDP::Client::set_server_addr(const char* ip, int port, int channels) {
    enet_address_set_host(&sv_addr, ip);
    max_channels = channels;
    sv_addr.port = port;
};

void UDP::Client::start_recv_daemon() {
    if (recv_daemon_thread) stop_recv_daemon();
    listening = true;
    recv_daemon_thread = new std::thread(&recv_daemon, this);
};

void UDP::Client::stop_recv_daemon() {
    listening = false;
    if (!recv_daemon_thread) return;
    if (recv_daemon_thread->joinable())
        recv_daemon_thread->join();
    delete recv_daemon_thread;
};

bool UDP::Client::connect() {
    sv = enet_host_connect(cli, &sv_addr, max_channels, 0);
    if (!sv) return false;
    if (!(enet_host_service(cli, &ev, 5000) > 0 && ev.type == ENET_EVENT_TYPE_CONNECT)) {
        enet_peer_reset(sv);
        return false;
    };
    start_recv_daemon();
    return true;
};

bool UDP::Client::connect(const char* ip, int port, int channels) {
    set_server_addr(ip, port, channels);
    connect();
};

void UDP::Client::disconnect() {
    enet_peer_disconnect(sv, 0);
    stop_recv_daemon();
};

void UDP::Client::recv_daemon() {
    while (listening)
        while (enet_host_service(cli, &ev, 1000) > 0) {
            switch (ev.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                PacketInfo pack;
                pack.data = uint8_to_string(ev.packet->data, ev.packet->dataLength);
                pack.host_id = get_host_id(ev.peer);
                buffer.push(pack);
                break;
            };
            case ENET_EVENT_TYPE_DISCONNECT: {
                disconnect();
                break;
            };
            };
            enet_packet_destroy(ev.packet);
        };
};

bool UDP::Client::get_packet(std::string& data) {
    PacketInfo alldata;
    if (!get_packet(alldata)) return false;
    data = alldata.data;
    return true;
};

bool UDP::Client::get_packet(PacketInfo& alldata) {
    if (buffer.size() == 0) return false;
    return buffer.pop(alldata);
};

void UDP::Client::send_packet(std::string data) {
    send_packet(data, 0);
};

void UDP::Client::send_packet(std::string data, int channel) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(sv, channel, packet);
};

bool UDP::Client::is_connected() {
    return listening;
};

/*====================================/
               F_CLIENT
/====================================*/

UDP::F_Client::F_Client() {};
UDP::F_Client::~F_Client() {
    enet_host_destroy(cli);
};

bool UDP::F_Client::create(int conn, int chan) {
    cli = enet_host_create(nullptr, conn, chan, 0, 0);
    return !!cli;
};

void UDP::F_Client::set_server_addr(const char* ip, int port, int channels) {
    enet_address_set_host(&sv_addr, ip);
    max_channels = channels;
    sv_addr.port = port;
};

void UDP::F_Client::start_recv_daemon() {
    if (recv_daemon_thread) stop_recv_daemon();
    listening = true;
    recv_daemon_thread = new std::thread(&recv_daemon, this);
};

void UDP::F_Client::stop_recv_daemon() {
    listening = false;
    if (!recv_daemon_thread) return;
    if (recv_daemon_thread->joinable())
        recv_daemon_thread->join();
    delete recv_daemon_thread;
};

bool UDP::F_Client::connect() {
    sv = enet_host_connect(cli, &sv_addr, max_channels, 0);
    if (!sv) return false;
    if (!(enet_host_service(cli, &ev, 5000) > 0 && ev.type == ENET_EVENT_TYPE_CONNECT)) {
        enet_peer_reset(sv);
        return false;
    };
    start_recv_daemon();
    return true;
};

bool UDP::F_Client::connect(const char* ip, int port, int channels) {
    set_server_addr(ip, port, channels);
    connect();
};

void UDP::F_Client::disconnect() {
    enet_peer_disconnect(sv, 0);
    stop_recv_daemon();
};

void UDP::F_Client::recv_daemon() {
    while (listening)
        while (enet_host_service(cli, &ev, 1000) > 0) {
            switch (ev.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                if (func) func(ev);
                break;
            };
            case ENET_EVENT_TYPE_DISCONNECT: {
                disconnect();
                break;
            };
            };
            enet_packet_destroy(ev.packet);
        };
};

void UDP::F_Client::send_packet(std::string data) {
    send_packet(data, 0);
};

void UDP::F_Client::send_packet(std::string data, int channel) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(sv, channel, packet);
};

bool UDP::F_Client::is_connected() {
    return listening;
};

void UDP::F_Client::setCallBack(void (*f)(ENetEvent&)) {
    func = f;
};