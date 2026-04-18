#pragma once

#include <stdint.h>

namespace hekky {
	namespace osc {
		struct OscPacket {

		public:

		private:
			virtual char* GetBytes(int& size, bool prependSize) = 0;

			friend class UdpSender;
			friend class UdpListener;
		};

		namespace constants {
			constexpr uint64_t OSC_MINIMUM_PACKET_BYTES = 8;
		}
	}
}