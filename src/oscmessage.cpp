#include "hekky/osc/oscmessage.hpp"
#include "hekky/osc/utils.hpp"
#include <math.h>

namespace hekky {
	namespace osc {
		OscMessage::OscMessage()
			: m_address(), m_type(), m_readonly(true), m_readIndex(0), m_typeIndex(0)
		{}

		OscMessage::OscMessage(const std::string& address)
			: m_address(address), m_type(","), m_readonly(false), m_readIndex(0), m_typeIndex(1)
		{
			HEKKYOSC_ASSERT(address.length() > 1, "The address is invalid!");
			HEKKYOSC_ASSERT(address[0] == '/', "The address is invalid! It should start with a '/'!");
			m_data.reserve(constants::OSC_MINIMUM_PACKET_BYTES);
		}

		OscMessage::OscMessage(char* data, int size)
			: m_readonly(true), m_typeIndex(1)
		{
			m_data = std::vector<char>(data, data + size);
			m_address = std::string(data);
			m_readIndex = utils::GetAlignedStringLength(m_address);
			m_type = std::string(data + m_readIndex);
			m_readIndex += utils::GetAlignedStringLength(m_type);
		}

		OscMessage::~OscMessage() {
			m_data.clear();
		}

		OscMessage OscMessage::PushBlob(char* data, size_t size) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_data.insert(m_data.begin(), data, data + size);
			m_type += "b";
			return *this;
		}

		OscMessage OscMessage::PushFloat32(float data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			if (isinf(data)) {
				m_type += "I";
			} else {
				union {
					float f;
					char c[4];
				} primitiveLiteral = { data };

				if (utils::IsLittleEndian()) {
					primitiveLiteral.f = utils::SwapFloat32(data);
				}

				m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 4);
				m_type += "f";
			}
			return *this;
		}

		OscMessage OscMessage::PushFloat64 (double data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			if (isinf(data)) {
				m_type += "I";
			} else {
				union {
					double d;
					char c[8];
				} primitiveLiteral = { data };

				if (utils::IsLittleEndian()) {
					primitiveLiteral.d = utils::SwapFloat64(data);
				}

				m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 8);
				m_type += "d";
			}
			return *this;
		}

		OscMessage OscMessage::PushInt32(int data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			union {
				int i;
				char c[4];
			} primitiveLiteral = { data };

			if (utils::IsLittleEndian()) {
				primitiveLiteral.i = utils::SwapInt32(data);
			}

			m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 4);
			m_type += "i";
			return *this;
		}

		OscMessage OscMessage::PushInt64(long long data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			union {
				long long i;
				char c[8];
			} primitiveLiteral = { data };

			if (utils::IsLittleEndian()) {
				primitiveLiteral.i = utils::SwapInt64(data);
			}

			m_data.insert(m_data.end(), primitiveLiteral.c, primitiveLiteral.c + 8);
			m_type += "h";
			return *this;
		}

		OscMessage OscMessage::PushBoolean(bool data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_type += (data == true) ? "T" : "F";
			return *this;
		}

		OscMessage OscMessage::PushString(std::string data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			std::copy(data.begin(), data.end(), std::back_inserter(m_data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - data.length(), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushStringRef(const std::string& data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			std::copy(data.begin(), data.end(), std::back_inserter(m_data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - data.length(), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushCStyleStringRef(const char* data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_data.insert(m_data.end(), data, data + strlen(data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - strlen(data), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushCStyleString(char* data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_data.insert(m_data.end(), data, data + strlen(data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - strlen(data), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushWString(std::wstring data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			std::copy(data.begin(), data.end(), std::back_inserter(m_data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - data.length(), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushWStringRef(const std::wstring& data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			std::copy(data.begin(), data.end(), std::back_inserter(m_data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - data.length(), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushCStyleWStringRef(const wchar_t* data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_data.insert(m_data.end(), data, data + wcslen(data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - wcslen(data), 0);
			m_type += "s";
			return *this;
		}

		OscMessage OscMessage::PushCStyleWString(wchar_t* data) {
			HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

			m_data.insert(m_data.end(), data, data + wcslen(data));
			m_data.insert(m_data.end(), utils::GetAlignedStringLength(data) - wcslen(data), 0);
			m_type += "s";
			return *this;
		}

		// Aliases
		OscMessage OscMessage::PushFloat(float data) {
			return PushFloat32(data);
		}
		OscMessage OscMessage::PushDouble(double data) {
			return PushFloat64(data);
		}
		OscMessage OscMessage::PushInt(int data) {
			return PushInt32(data);
		}
		OscMessage OscMessage::PushLongLong(long long data) {
			return PushInt64(data);
		}

		// Generic aliases
		OscMessage OscMessage::Push(float data) {
			return PushFloat32(data);
		}
		OscMessage OscMessage::Push(double data) {
			return PushFloat64(data);
		}
		OscMessage OscMessage::Push(int data) {
			return PushInt32(data);
		}
		OscMessage OscMessage::Push(long long data) {
			return PushInt64(data);
		}
		OscMessage OscMessage::PushBool(bool data) {
			return PushBoolean(data);
		}

		OscMessage OscMessage::Push(std::string data) {
			return PushString(data);
		}
		OscMessage OscMessage::Push(const std::string& data) {
			return PushStringRef(data);
		}
		OscMessage OscMessage::Push(char* data) {
			return PushCStyleString(data);
		}

		OscMessage OscMessage::Push(const char* data) {
			return PushCStyleStringRef(data);
		}

		// Wide strings
		OscMessage OscMessage::Push(std::wstring data) {
			return PushWString(data);
		}
		OscMessage OscMessage::Push(const std::wstring& data) {
			return PushWStringRef(data);
		}
		OscMessage OscMessage::Push(wchar_t* data) {
			return PushCStyleWString(data);
		}
		OscMessage OscMessage::Push(const wchar_t* data) {
			return PushCStyleWStringRef(data);
		}

		// Blob
		OscMessage OscMessage::Push(char* data, size_t size) {
			return PushBlob(data, size);
		}

		float OscMessage::ReadNextFloat32() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 'f' || m_type[m_typeIndex] == 'I', "Tried to read incorrect type!");

			if (m_type[m_typeIndex] == 'f') {
				float out = *reinterpret_cast<float*>(m_data.data() + m_readIndex);
				if (utils::IsLittleEndian())
					out = utils::SwapFloat32(out);
				m_readIndex += 4;
				m_typeIndex++;

				return out;
			}
			else {
				m_typeIndex++;
				return std::numeric_limits<float>().infinity();
			}
		}

		double OscMessage::ReadNextFloat64() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 'd' || m_type[m_typeIndex] == 'I', "Tried to read incorrect type!");

			if (m_type[m_typeIndex] == 'd') {
				double out = *reinterpret_cast<double*>(m_data.data() + m_readIndex);
				if (utils::IsLittleEndian())
					out = utils::SwapFloat64(out);
				m_readIndex += 8;
				m_typeIndex++;

				return out;
			}
			else {
				m_typeIndex++;
				return std::numeric_limits<double>().infinity();
			}
		}

		int OscMessage::ReadNextInt32() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 'i', "Tried to read incorrect type!");

			int out = *reinterpret_cast<int*>(m_data.data() + m_readIndex);
			if (utils::IsLittleEndian())
				out = utils::SwapInt32(out);
			m_readIndex += 4;
			m_typeIndex++;

			return out;
		}

		long long OscMessage::ReadNextInt64() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 'h', "Tried to read incorrect type!");

			long long out = *reinterpret_cast<long long*>(m_data.data() + m_readIndex);
			if (utils::IsLittleEndian())
				out = utils::SwapInt64(out);
			m_readIndex += 8;
			m_typeIndex++;

			return out;
		}

		bool OscMessage::ReadNextBoolean() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 'T' || m_type[m_typeIndex] == 'F', "Tried to read incorrect type!");

			return m_type[m_typeIndex++] == 'T';
		}

		std::string OscMessage::ReadNextString() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 's', "Tried to read incorrect type!");

			auto out = std::string(m_data.data() + m_readIndex);
			m_readIndex += utils::GetAlignedStringLength(out);
			m_typeIndex++;

			return out;
		}

		std::wstring OscMessage::ReadNextWString() {
			HEKKYOSC_ASSERT(m_type[m_typeIndex] == 's', "Tried to read incorrect type!");

			auto out = std::wstring(reinterpret_cast<wchar_t*>(m_data.data() + m_readIndex));
			m_readIndex += utils::GetAlignedStringLength(out);
			m_typeIndex++;

			return out;
		}

		// Internal function
		char* OscMessage::GetBytes(int& size, bool prependSize) {
			std::vector<char> headerData;
			headerData.reserve(m_address.size() + m_type.size() + m_data.size());

			// Append address
			std::copy(m_address.begin(), m_address.end(), std::back_inserter(headerData));
			headerData.insert(headerData.end(), utils::GetAlignedStringLength(m_address) - m_address.length(), 0);

			// Append types
			std::copy(m_type.begin(), m_type.end(), std::back_inserter(headerData));
			headerData.insert(headerData.end(), utils::GetAlignedStringLength(m_type) - m_type.length(), 0);

			// Add header to start of data block
			m_data.insert(m_data.begin(), headerData.begin(), headerData.end());
			
			// Lock this packet
			m_readonly = true;
			size = static_cast<int>(m_data.size());

			if (prependSize)
			{
				union {
					int i;
					char c[4];
				} primitiveLiteral = { size };

				if (utils::IsLittleEndian()) {
					primitiveLiteral.i = utils::SwapInt32(size);
				}

				m_data.insert(m_data.begin(), primitiveLiteral.c, primitiveLiteral.c + 4);
			}

			return m_data.data();
		}
	}
}
