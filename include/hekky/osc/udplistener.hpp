#pragma once

#include "hekky/osc/platform.hpp"
#include "hekky/osc/asserts.hpp"
#include "hekky/osc/oscpacket.hpp"
#include "hekky/osc/oscmessage.hpp"

#include "hekky/osc/udpsender.hpp"

#include <string>
#include <atomic>
#include <thread>

#ifdef HEKKYOSC_WINDOWS

// Winsock API
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#endif

namespace hekky {
	namespace osc {

		class UdpListener : network::OSC_NetworkSocket {
		public:
			UdpListener();

			/// <summary>
			/// Creates a UDP socket listener.
			/// </summary>
			/// <param name="ipAddress">Listen IP Address</param>
			/// <param name="port">Listen port</param>
			/// <param name="timeout">Timeout for waiting on new message, 0 to wait forever</param>
			/// <param name="callback">Pointer to callback that gets called on every new message, set it to null to disable asynchronous message receiving</param>
			UdpListener(const std::string& ipAddress, uint32_t portIn, uint32_t timeout, void (*callback)(OscMessage&), network::OSC_NetworkProtocol protocol = network::OSC_NetworkProtocol::UDP);

			/// <summary>
			/// Destroys this UDP socket connection, if it's alive.
			/// </summary>
			~UdpListener();

			/// <summary>
			/// Closes this UDP socket connection. The listener will stop receiving new messages.
			/// </summary>
			void Close() override;

			/// <summary>
			/// Receive next message synchronously. This is a blocking call.
			/// </summary>
			/// <param name="message">Pointer to the message object</param>
			/// <returns>
			/// 0 if successful.
			/// -1 if timeout was exceeded while receiving message.
			/// -2 if this listener was closed.
			/// -3 if other reason.
			/// </returns>
			int Receive(OscMessage* message);

			/// <summary>
			/// Returns whether the server is alive or not
			/// </summary>
			const bool IsAlive();
		private:
			void receiveLoop();

			bool m_isAlive;
			std::string m_address;
			uint32_t m_portIn;
			std::thread m_receiveThread;
			std::atomic<bool> m_receiveThreadRun;

			void (*m_receiveCallback)(OscMessage&);

#ifdef HEKKYOSC_WINDOWS
			sockaddr_in m_listenAddress;
#endif
		};
	}
}