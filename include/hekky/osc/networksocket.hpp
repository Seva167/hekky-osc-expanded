#pragma once

#include "hekky/osc/platform.hpp"
#include "hekky/osc/asserts.hpp"
#include "hekky/osc/oscpacket.hpp"
#include "hekky/osc/oscmessage.hpp"

#include <string>

#ifdef HEKKYOSC_WINDOWS

// Winsock API
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// Tell the linker to link to the winsock library
#pragma comment(lib, "Ws2_32.lib")

#endif

namespace hekky {
	namespace osc {
		namespace network {
			/// <summary>
			/// Which protocol to use. Defaults to UDP.
			/// </summary>
			typedef enum {
				UDP,
				TCP,
			} OSC_NetworkProtocol;

			/// <summary>
			/// A class which represents system network sockets.
			/// </summary>
			class OSC_NetworkSocket {
			public:
				OSC_NetworkSocket();

				OSC_NetworkSocket(OSC_NetworkProtocol protocol);

				/// <summary>
				/// Destroys this socket, if it's open.
				/// </summary>
				~OSC_NetworkSocket();
			protected:
				/// <summary>
				/// Opens this socket.
				/// </summary>
				void Open();

				/// <summary>
				/// Closes this socket.
				/// </summary>
				virtual void Close();

				/// <summary>
				/// Returns whether the socket is created
				/// </summary>
				const bool IsOpen();
			private:
				static uint64_t m_openSockets;

#ifdef HEKKYOSC_WINDOWS
				static bool m_wsaInitialized;
#endif
			protected:
				OSC_NetworkProtocol m_protocol;

#ifdef HEKKYOSC_WINDOWS
				SOCKET m_nativeSocket;
#endif

			};
		}
	}
}