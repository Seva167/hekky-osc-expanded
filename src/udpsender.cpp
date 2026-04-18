#include "hekky-osc.hpp"

namespace hekky {
    namespace osc {
        const bool UdpSender::IsAlive() {
            return m_isAlive;
        }

        UdpSender::UdpSender() : OSC_NetworkSocket(network::OSC_NetworkProtocol::UDP), m_address(""), m_portOut(0), m_isAlive(false)
#ifdef HEKKYOSC_WINDOWS
            , m_destinationAddress({ 0 })
#endif
        {}

        UdpSender::UdpSender(const std::string& ipAddress, uint32_t portOut, network::OSC_NetworkProtocol protocol)
            : OSC_NetworkSocket(protocol), m_address(ipAddress), m_portOut(portOut)
#ifdef HEKKYOSC_WINDOWS
            , m_destinationAddress({ 0 })
#endif
        {
            m_isAlive = false;
#ifdef HEKKYOSC_WINDOWS
            int result;

            // Get the destination as a native network address
            m_destinationAddress.sin_family = AF_INET;
            result = inet_pton(AF_INET, m_address.c_str(), &m_destinationAddress.sin_addr.s_addr);
            if (result == 0) {
                HEKKYOSC_ASSERT(result == 0, "Invalid IP Address!");
                return;
            }
            else if (result == -1) {
#ifdef HEKKYOSC_DOASSERTS
                int errorCode = WSAGetLastError();
                HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode)+ "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton#return-value.\n");
#endif
                HEKKYOSC_ASSERT(result == -1, "Failed to set IP Address!");
                return;
            }
            m_destinationAddress.sin_port = htons(m_portOut);

            // Create native socket
            OSC_NetworkSocket::Open();

            if (m_protocol == network::OSC_NetworkProtocol::TCP)
            {
                result = connect(m_nativeSocket, (sockaddr*)&m_destinationAddress, sizeof(m_destinationAddress));
                if (result == SOCKET_ERROR) {
#ifdef HEKKYOSC_DOASSERTS
                    int errorCode = WSAGetLastError();
                    HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind#return-value.\n");
#endif
                    HEKKYOSC_ASSERT(result == SOCKET_ERROR, "Failed to connect to network socket!");
                    return;
                }
            }

            // If we reached this point, we have successfully initialized a network socket!
            m_isAlive = true;
#endif
        }

        UdpSender::~UdpSender() {
            if (m_isAlive) {
                Close();
            } else {
#ifdef HEKKYOSC_DOASSERTS
                HEKKYOSC_ASSERT(false, "Tried destroying OSC Server, but the server is not running!");
#endif
            }
        }

        void UdpSender::Close() {
            HEKKYOSC_ASSERT(m_isAlive == true, "Tried closing OSC Server, but the OSC Server is not running! Has the OSC Server already been destroyed?");
            OSC_NetworkSocket::Close();

            m_isAlive = false;
        }

        void UdpSender::Send(char* data, int size) {
#ifdef HEKKYOSC_WINDOWS
            HEKKYOSC_ASSERT(OSC_NetworkSocket::IsOpen(), "Tried sending a packet, but the native socket is null! Has the socket been initialized?");
            HEKKYOSC_ASSERT(m_isAlive == true, "Tried sending a packet, but the server isn't running!");

            // Skip if the data is 0 or somehow negative (should be theoretically impossible since this is an unsigned integer)
            if (size < 1)
                return;

            // Send data over the socket
            sendto(m_nativeSocket, data, size, 0, (sockaddr*)&m_destinationAddress, sizeof(m_destinationAddress));
#endif
        }

        void UdpSender::Send(OscPacket& packet) {
#ifdef HEKKYOSC_WINDOWS
            HEKKYOSC_ASSERT(OSC_NetworkSocket::IsOpen(), "Tried sending a packet, but the native socket is null! Has the socket been initialized?");
            HEKKYOSC_ASSERT(m_isAlive == true, "Tried sending a packet, but the server isn't running!");

            int size = 0;
            char* data = packet.GetBytes(size, m_protocol == network::OSC_NetworkProtocol::TCP);

            // Send data over the socket
            Send(data, size);
#endif
        }
    }
}