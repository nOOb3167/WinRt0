#pragma once

#include <format>
#include <string>
#include <string_view>

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
	uint8_t m_length;
};


struct OcPacket
{
	static const uint8_t magic = '\xcd';


	static OcPacket FromReceived(const std::string& s)
	{
		CHL(s.size() >= 5);
		Header h = { .m_magic = (uint8_t)s[0], .m_flags = (uint8_t)s[1], .m_command = (uint8_t)s[2], .m_unk00 = (uint8_t)s[3], .m_length = (uint8_t)s[4], };
		CHL(h.m_magic == OcPacket::magic);
		std::string data = s.substr(5, std::string::npos);
		CHL(h.m_length == data.size());
		return { .m_header = h, .m_data = data, .m_data_raw = s, };
	}


	Header m_header;
	std::string m_data;
	std::string m_data_raw;
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
