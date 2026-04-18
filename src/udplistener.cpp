#include "hekky-osc.hpp"

namespace hekky {
	namespace osc {
		const bool UdpListener::IsAlive() {
			return m_isAlive;
		}

		UdpListener::UdpListener() : OSC_NetworkSocket(network::OSC_NetworkProtocol::UDP), m_address(""), m_portIn(0), m_isAlive(false), m_receiveCallback(nullptr)
#ifdef HEKKYOSC_WINDOWS
			, m_listenAddress({ 0 })
#endif
		{}

		UdpListener::UdpListener(const std::string& ipAddress, uint32_t portIn, uint32_t timeout, void (*callback)(OscMessage&), network::OSC_NetworkProtocol protocol)
			: OSC_NetworkSocket(protocol), m_address(ipAddress), m_portIn(portIn), m_receiveCallback(callback)
#ifdef HEKKYOSC_WINDOWS
			, m_listenAddress({ 0 })
#endif
		{
			m_isAlive = false;
            m_receiveThreadRun.store(false);
#ifdef HEKKYOSC_WINDOWS
            int result;

            // Get the destination as a native network address
            m_listenAddress.sin_family = AF_INET;
            result = inet_pton(AF_INET, m_address.c_str(), &m_listenAddress.sin_addr.s_addr);
            if (result == 0) {
                HEKKYOSC_ASSERT(result == 0, "Invalid IP Address!");
                return;
            }
            else if (result == -1) {
#ifdef HEKKYOSC_DOASSERTS
                int errorCode = WSAGetLastError();
                HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton#return-value.\n");
#endif
                HEKKYOSC_ASSERT(result == -1, "Failed to set IP Address!");
                return;
            }
            m_listenAddress.sin_port = htons(m_portIn);

            // Create native socket
            OSC_NetworkSocket::Open();

            result = bind(m_nativeSocket, (sockaddr*)&m_listenAddress, sizeof(m_listenAddress));
            if (result == SOCKET_ERROR) {
#ifdef HEKKYOSC_DOASSERTS
                int errorCode = WSAGetLastError();
                HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind#return-value.\n");
#endif
                HEKKYOSC_ASSERT(result == SOCKET_ERROR, "Failed to bind to network socket!");
                return;
            }

            result = setsockopt(m_nativeSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
            if (result == SOCKET_ERROR) {
#ifdef HEKKYOSC_DOASSERTS
                int errorCode = WSAGetLastError();
                HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt#return-value.\n");
#endif
                HEKKYOSC_ASSERT(result == SOCKET_ERROR, "Failed to set socket options!");
                std::cout << "Error" << std::endl;
                return;
            }

            if (m_receiveCallback != nullptr) {
                m_receiveThreadRun.store(true);
                m_receiveThread = std::thread(&UdpListener::receiveLoop, this);
            }
            m_isAlive = true;
#endif
		}

        UdpListener::~UdpListener() {
            if (m_isAlive) {
                Close();
            }
            else {
#ifdef HEKKYOSC_DOASSERTS
                HEKKYOSC_ASSERT(false, "Tried destroying OSC Server, but the server is not running!");
#endif
            }
        }

        void UdpListener::Close() {
            HEKKYOSC_ASSERT(m_isAlive == true, "Tried closing OSC Server, but the OSC Server is not running! Has the OSC Server already been destroyed?");
            m_receiveThreadRun.store(false);
            if (m_receiveThread.joinable())
                m_receiveThread.join();
            OSC_NetworkSocket::Close();

            m_isAlive = false;
        }

        int UdpListener::Receive(OscMessage* message) {
            char recvBuf[512];
#ifdef HEKKYOSC_WINDOWS
            int size = recv(m_nativeSocket, recvBuf, 512, 0);
            if (size == SOCKET_ERROR) {
                int errorCode = WSAGetLastError();
                switch (errorCode)
                {
                case WSAETIMEDOUT:
                    return -1; // Timeout exceeded
                case WSAENOTSOCK:
                    return -2; // Connection closed
                default:
                    return -3; // Other
                };
            }
#endif

            *message = OscMessage(recvBuf, size);

            return 0;
        }

        void UdpListener::receiveLoop() {
            char recvBuf[512];
#ifdef HEKKYOSC_WINDOWS
            int laLen = sizeof(m_listenAddress);

            while (m_receiveThreadRun.load()) {
                int size = recv(m_nativeSocket, recvBuf, 512, 0);
                if (size == SOCKET_ERROR) {
#ifdef HEKKYOSC_DOASSERTS
                    int errorCode = WSAGetLastError();
                    HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recvfrom#return-value.\n");
#endif
                    HEKKYOSC_ASSERT(size == SOCKET_ERROR, "Failed to receive packet data!");
                    continue;
                }
#endif

                auto msg = OscMessage(recvBuf, size);
                m_receiveCallback(msg);
            }
        }
	}
}