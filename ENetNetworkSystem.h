#pragma once
#include <string>
#include <sstream>
#include <enet/enet.h>
#include <map>
#include <entityx/entityx.h>
#include "events.h"

class CENetNetworkSystem : public entityx::System<CENetNetworkSystem>, public entityx::Receiver<CENetNetworkSystem>
{
public:
    CENetNetworkSystem();
    virtual ~CENetNetworkSystem();

    static std::string host2ip(uint32_t host);

    virtual void configure(entityx::ptr<entityx::EventManager> events);
    virtual void update(entityx::ptr<entityx::EntityManager> entities, entityx::ptr<entityx::EventManager> events, double dt);

    void receive(const HostEvent &e);
    void receive(const ConnectEvent &e);
    void receive(const DisconnectEvent &e);

    void receive(const ClientSendMessageEvent &e);
    void receive(const ClientBroadcastMessageEvent &e);

private:
    void Disconnect();

    void Broadcast(const std::string &data, bool reliable);
    void Send(const std::string &address, unsigned int port, const std::string &data, bool reliable);
    void Send(ENetPeer *peer, const std::string &data, bool reliable);

    typedef std::pair<std::string, unsigned int> SocketID;
    typedef std::map<SocketID, ENetPeer *> PeerMap;

    bool m_hosting;

    ENetHost *m_host;
    PeerMap m_peers;
};
