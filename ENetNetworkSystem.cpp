#include <string>
#include "ENetNetworkSystem.h"
#include <sstream>
#include "Log.h"

using namespace logging;

std::string CENetNetworkSystem::host2ip(uint32_t host) {
    std::stringstream address;

    address <<  (host & 0x000000ff)        << ".";
    address << ((host & 0x0000ff00) >> 8)  << ".";
    address << ((host & 0x00ff0000) >> 16) << ".";
    address << ((host & 0xff000000) >> 24);

    return address.str();
}

CENetNetworkSystem::CENetNetworkSystem() {
    enet_initialize();
    m_hosting = false;
    m_host = NULL;
}

CENetNetworkSystem::~CENetNetworkSystem() {
    LOG(INFO, "Closing peer connections");
    Disconnect();

    enet_deinitialize();
}

void CENetNetworkSystem::configure(entityx::ptr<entityx::EventManager> events) {
    events->subscribe<HostEvent>(*this);
    events->subscribe<ConnectEvent>(*this);
    events->subscribe<DisconnectEvent>(*this);

    events->subscribe<ClientSendMessageEvent>(*this);
    events->subscribe<ClientBroadcastMessageEvent>(*this);
}

void CENetNetworkSystem::update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt) {
    ENetEvent event;
    if (m_host) {
        while (enet_host_service (m_host, &event, 0) > 0) {
            std::string ip = host2ip(event.peer->address.host);
            unsigned int port = event.peer->address.port;

            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    m_peers[SocketID(ip, port)] = event.peer;
                    events->emit<ClientConnectedEvent>(ip, port);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    events->emit<ClientReceiveMessageEvent>(ip, port, std::string((char *)event.packet->data, event.packet->dataLength));
                    enet_packet_destroy (event.packet);
                    break;
                   
                case ENET_EVENT_TYPE_DISCONNECT:
                    events->emit<ClientDisconnectedEvent>(ip, port);
                    m_peers.erase(m_peers.find(SocketID(ip, port)));
                    break;
            }
        }
    }
}

void CENetNetworkSystem::receive(const HostEvent &e) {
    if (m_host) {
        LOG(ERROR, "Hosting when have active connection already");
    } else {
        ENetAddress enet_address;

        enet_address.host = ENET_HOST_ANY;
        enet_address.port = e.port;
        m_host = enet_host_create(&enet_address, 32, 2, 0, 0);

        if (m_host) {
            LOG(INFO, "ENetServer Started on " << e.port);
            m_hosting = true;
        } else {
            LOG(ERROR, "Failed to create ENet host");
        }
    }
}

void CENetNetworkSystem::receive(const ConnectEvent &e) {
    if (m_host) {
        LOG(ERROR, "Connecting when have active connection already");
    } else {
        m_host = enet_host_create (NULL, 1, 2, 57600 / 8, 14400 / 8);
        if (m_host) {
            LOG(DEBUG, "ENetClient connecting to " << e.address << ":" << e.port);

            ENetAddress enet_address;

            enet_address_set_host (&enet_address, e.address.c_str());
            enet_address.port = e.port;

            enet_host_connect(m_host, &enet_address, 2, 0);
            m_hosting = false;
        } else {
            LOG(ERROR, "Failed to create ENet client");
        }
    }
}

void CENetNetworkSystem::receive(const DisconnectEvent &e) {
    /* TODO React on address and port? */
    Disconnect();
}

void CENetNetworkSystem::receive(const ClientSendMessageEvent &e) {
    Send(e.address, e.port, e.data, e.reliable);
}

void CENetNetworkSystem::receive(const ClientBroadcastMessageEvent &e) {
    Broadcast(e.data, e.reliable);
}

void CENetNetworkSystem::Disconnect() {
    if (m_host) {
        for (PeerMap::iterator itr = m_peers.begin(); itr != m_peers.end(); itr++) {
            enet_peer_reset(itr->second);
        }
        m_peers.clear();

        enet_host_destroy(m_host);
        m_host = NULL;
        m_hosting = false;
    }
}

void CENetNetworkSystem::Broadcast(const std::string &data, bool reliable)
{
    for (PeerMap::iterator itr = m_peers.begin(); itr != m_peers.end(); itr++) {
        Send(itr->second, data, reliable);
    }
}

void CENetNetworkSystem::Send(const std::string &address, unsigned int port, const std::string &data, bool reliable)
{
    PeerMap::iterator itr = m_peers.find(SocketID(address, port));

    if (itr != m_peers.end()) {
        Send(itr->second, data, reliable);
    } else {
        LOG(WARNING, "NO peer for address");
    }
}

void CENetNetworkSystem::Send(ENetPeer *peer, const std::string &data, bool reliable) {
    ENetPacket *packet = enet_packet_create(data.c_str(), data.length(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, 0, packet);
}
