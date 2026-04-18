#include "hekky-osc.hpp"

namespace hekky {
	namespace osc {
		namespace network {
			uint64_t OSC_NetworkSocket::m_openSockets = 0;
#ifdef HEKKYOSC_WINDOWS
			bool OSC_NetworkSocket::m_wsaInitialized = false;
#endif

			const bool OSC_NetworkSocket::IsOpen() {
#ifdef HEKKYOSC_WINDOWS
				return m_nativeSocket != INVALID_SOCKET;
#endif
			}

			OSC_NetworkSocket::OSC_NetworkSocket() : m_protocol(OSC_NetworkProtocol::UDP)
#ifdef HEKKYOSC_WINDOWS
				, m_nativeSocket(INVALID_SOCKET)
#endif
			{}

			OSC_NetworkSocket::OSC_NetworkSocket(OSC_NetworkProtocol protocol) : m_protocol(protocol)
#ifdef HEKKYOSC_WINDOWS
				, m_nativeSocket(INVALID_SOCKET)
#endif
			{
#ifdef HEKKYOSC_WINDOWS
				int result = 0;
				if (!m_wsaInitialized) {
					WSADATA wsaData;
					// @TODO: Skip if other connections are open
					result = WSAStartup(MAKEWORD(2, 2), &wsaData);
					if (result != 0) {
						HEKKYOSC_ASSERT(result == 0, "WSAStartup failed");
						return;
					}

					m_wsaInitialized = true;
				}
#endif
			}

			OSC_NetworkSocket::~OSC_NetworkSocket() {
				if (IsOpen())
					Close();

#ifdef HEKKYOSC_WINDOWS
				if (m_openSockets < 1 && m_wsaInitialized) {
					WSACleanup();
				}
#endif
			}

			void OSC_NetworkSocket::Open() {
#ifdef HEKKYOSC_WINDOWS
				if (!m_wsaInitialized) {
					HEKKYOSC_ASSERT(m_wsaInitialized, "Winsock was not initialized");
				}

				// Open the network socket
				m_nativeSocket = socket(AF_INET, m_protocol == network::OSC_NetworkProtocol::UDP ? SOCK_DGRAM : SOCK_STREAM, m_protocol == network::OSC_NetworkProtocol::UDP ? IPPROTO_UDP : IPPROTO_TCP);
				if (!IsOpen()) {
#ifdef HEKKYOSC_DOASSERTS
					int errorCode = WSAGetLastError();
					HEKKYOSC_ERR(std::string("WSA Error code: ") + std::to_string(errorCode) + "\nFor more information, please visit https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket#return-value.\n");
#endif				return;
				}

#endif
				m_openSockets++;
			}


			void OSC_NetworkSocket::Close() {
				HEKKYOSC_ASSERT(IsOpen(), "Tried closing native socket, but the native socket was not open! Has the socket already been destroyed?");

#ifdef HEKKYOSC_WINDOWS
				closesocket(m_nativeSocket);
				m_nativeSocket = INVALID_SOCKET;

				m_openSockets--;
#endif
			}
		}
	}
}