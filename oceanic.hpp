#pragma once

#include <format>
#include <functional>
#include <format>
#include <iostream>
#include <string>

#include <stdint.h>

#include <bluetooth_com_helper.hpp>


namespace bt
{


struct Header
{
	uint8_t m_magic;
	uint8_t m_flags;
	uint8_t m_command;
	uint8_t m_unk00;
};


struct OcPacket
{
	static const uint8_t magic = '\xcd';


	static OcPacket FromReceived(const std::string& s)
	{
		CHL(s.size() >= 5);
		uint8_t length = (uint8_t)s[4];
		Header h = { .m_magic = (uint8_t)s[0], .m_flags = (uint8_t)s[1], .m_command = (uint8_t)s[2], .m_unk00 = (uint8_t)s[3], };
		CHL(h.m_magic == OcPacket::magic);
		std::string data = s.substr(5, std::string::npos);
		CHL(length == data.size());
		return { .m_header = h, .m_data = data, };
	}


	static OcPacket FromCommandRequestAccess()
	{
		using namespace std::string_literals;
		Header h = { .m_magic = OcPacket::magic, .m_flags = 0x40, .m_command = 0xfa, .m_unk00 = 0xf6, };
		std::string data = "\x00\x00\x00\x00\x00\x00\x00\x00\x00"s;
		return { .m_header = h, .m_data = data, };
	}


	std::string GetWireData() const
	{
		uint8_t length = (uint8_t)m_data.size();

		std::string s = "";
		s.append((char*)&m_header.m_magic, 1);
		s.append((char*)&m_header.m_flags, 1);
		s.append((char*)&m_header.m_command, 1);
		s.append((char*)&m_header.m_unk00, 1);
		s.append((char*)&length, 1);
		s.append(m_data);

		return s;
	}


	Header m_header;
	std::string m_data;
};


typedef std::function<OcPacket(void)> fnread_t;
typedef std::function<void(const OcPacket&)> fnwrit_t;


struct OcRequestResponse
{
	OcRequestResponse(fnread_t read, fnwrit_t writ) : m_read(read), m_writ(writ) {}

	void RequestAccess()
	{
		m_writ(OcPacket::FromCommandRequestAccess());
		OcPacket p0 = m_read();
	}

	std::vector<OcPacket> m_request;
	std::vector<OcPacket> m_response;

	fnread_t m_read;
	fnwrit_t m_writ;
};


} // namespace bt


namespace std
{


using namespace bt;


using fsv = formatter<string>;

template <>
struct formatter<Header> : formatter<string> {
	auto format(Header o, format_context& ctx) const
	{
        return fsv::format(std::format("header[cd[{:02x}], flag[{:02x}], cmd[{:02x}], ???[{:02x}]]", o.m_magic, o.m_flags, o.m_command, o.m_unk00), ctx);
    }
};


template <>
struct formatter<OcPacket> : formatter<string>
{
    auto format(OcPacket o, format_context& ctx) const
	{
		return formatter<string>::format(std::format("packet[{}, data[{}]]", o.m_header, ToHex(o.m_data)), ctx);
    }
};


} // namespace std
