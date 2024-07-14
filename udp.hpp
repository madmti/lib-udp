namespace UDP {
#include <enet/enet.h>
#include <thread>
#include "queue.hpp"
#include <string>
#include <map>

    const ushort pack_recieve = 0;
    const ushort pack_connect = 1;
    const ushort pack_disconnect = 2;

    bool init_udp();
    std::string uint8_to_string(enet_uint8* data, size_t data_length);
    std::string get_host_id(ENetPeer* host);

    struct PacketInfo {
        std::string host_id;
        std::string data;
        ushort type = 0;
    };

    class F_Client {
    public:
        F_Client();
        ~F_Client();

        bool create(int n_connections, int n_channels);
        void set_server_addr(const char* ip, int port, int channels);
        bool connect();
        bool connect(const char* ip, int port, int channels);
        void disconnect();
        void send_packet(std::string data);
        void send_packet(std::string data, int channel);
        bool is_connected();
        void setCallBack(void(*f)(ENetEvent&));

    private:
        void recv_daemon();
        void start_recv_daemon();
        void stop_recv_daemon();

        ENetHost* cli;
        ENetPeer* sv;
        ENetAddress sv_addr;
        ENetEvent ev;
        std::thread* recv_daemon_thread;

        void (*func)(ENetEvent&) = nullptr;

        int max_channels = 1;
        bool listening = false;
    };

    class Client {
    public:
        Client();
        ~Client();

        bool create(int n_connections, int n_channels);
        void set_server_addr(const char* ip, int port, int channels);
        bool connect();
        bool connect(const char* ip, int port, int channels);
        void disconnect();
        bool get_packet(std::string& data);
        bool get_packet(PacketInfo& alldata);
        void send_packet(std::string data);
        void send_packet(std::string data, int channel);
        bool is_connected();

    private:
        void recv_daemon();
        void start_recv_daemon();
        void stop_recv_daemon();

        ENetHost* cli;
        ENetPeer* sv;
        ENetAddress sv_addr;
        ENetEvent ev;
        std::thread* recv_daemon_thread;

        int max_channels = 1;
        bool listening = false;

        Queue<PacketInfo> buffer;
    };

    class F_Server {
    public:
        F_Server();
        ~F_Server();

        bool create(int connections, int channels, int port);
        bool create(int connections, int channels, const char* ip, int port);
        void listen();
        void stop_listen();
        void broadcast_packet(std::string data);
        void broadcast_packet(int chan, std::string data);
        void send_packet(ENetPeer* peer, std::string data);
        void send_packet(ENetPeer* peer, int chan, std::string data);
        void setCallBack(void(*f)(ENetEvent&));

    private:
        bool create_host(int n_connections, int n_channels);

        void listen_daemon();

        ENetHost* sv;
        ENetAddress sv_addr;
        ENetEvent ev;
        std::thread* listen_daemon_thread;

        int max_conn = 1;
        bool listening = false;
        void (*func)(ENetEvent&) = nullptr;
    };

    class Server {
    public:
        Server();
        ~Server();

        bool create(int connections, int channels, int port);
        bool create(int connections, int channels, const char* ip, int port);
        void listen();
        void stop_listen();
        bool get_packet(std::string& data);
        bool get_packet(PacketInfo& alldata);
        void broadcast_packet(std::string data);
        void broadcast_packet(int chan, std::string data);
        void send_packet(std::string host_id, std::string data);
        void send_packet(std::string host_id, int chan, std::string data);

    private:
        bool create_host(int n_connections, int n_channels);

        void listen_daemon();

        ENetHost* sv;
        ENetAddress sv_addr;
        ENetEvent ev;
        std::thread* listen_daemon_thread;

        int max_conn = 1;
        bool listening = false;
        Queue<PacketInfo> packet_buffer;

        std::map<std::string, ENetPeer*> clients;
    };

};