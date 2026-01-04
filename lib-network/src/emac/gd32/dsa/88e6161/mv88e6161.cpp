/*
 * mv88e6161.cpp
 */

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "port.h"
#include "mv88e6161.h"
#include "debug.h"

namespace dsa
{
static constexpr uint16_t MV88E6XXX_PHY_STS_DUPLEX = (1U << 13);
static constexpr uint16_t MV88E6XXX_PHY_STS_LINK = (1U << 10);

bool PhyGetStatus(uint32_t phy_index, dsa::PhyStatus& phy_status)
{
    DEBUG_ENTRY
    DEBUG_PRINTF("phy_index=%u", phy_index);

    if (phy_index >= MV88E6161_NUM_INTERNAL_PHYS)
    {
        DEBUG_EXIT
        return false;
    }

    uint16_t value;

    if (PhyRead(phy_index, 17, value))
    {
        DEBUG_EXIT
        return false;
    }

    phy_status.link = ((value & MV88E6XXX_PHY_STS_LINK) == MV88E6XXX_PHY_STS_LINK) ? dsa::Link::STATE_UP : dsa::Link::STATE_DOWN;
    phy_status.duplex = ((value & MV88E6XXX_PHY_STS_DUPLEX) == MV88E6XXX_PHY_STS_DUPLEX) ? dsa::Duplex::DUPLEX_FULL : dsa::Duplex::DUPLEX_HALF;

    DEBUG_EXIT
    return true;
}

bool PortGetStatus(uint32_t port_number, PhyStatus& phy_status)
{
    DEBUG_ENTRY
    DEBUG_PRINTF("port_number=%u", port_number);

    if (port_number >= MV88E6161_NUM_PORTS)
    {
        DEBUG_EXIT
        return false;
    }

    uint16_t value;

    if (mv88e6xxx_port_read(port_number, MV88E6XXX_PORT_STS, value) < 0)
    {
        DEBUG_EXIT
        return false;
    }

    phy_status.link = ((value & MV88E6XXX_PORT_STS_LINK) == MV88E6XXX_PORT_STS_LINK) ? dsa::Link::STATE_UP : dsa::Link::STATE_DOWN;
    phy_status.duplex = ((value & MV88E6XXX_PORT_STS_DUPLEX) == MV88E6XXX_PORT_STS_DUPLEX) ? dsa::Duplex::DUPLEX_FULL : dsa::Duplex::DUPLEX_HALF;
    phy_status.flowcontrol = ((value & MV88E6XXX_PORT_STS_FLOW_CTL) == MV88E6XXX_PORT_STS_FLOW_CTL);

    switch (value & MV88E6XXX_PORT_STS_SPEED_MASK)
    {
        case MV88E6XXX_PORT_STS_SPEED_10:
            phy_status.speed = dsa::Speed::SPEED10;
            break;
        case MV88E6XXX_PORT_STS_SPEED_100:
            phy_status.speed = dsa::Speed::SPEED100;
            break;
        case MV88E6XXX_PORT_STS_SPEED_1000:
            phy_status.speed = dsa::Speed::SPEED1000;
            break;
        default:
            DEBUG_EXIT
            return false;
            break;
    }

    DEBUG_EXIT
    return true;
}
} // namespace dsa

namespace remoteconfig::dsa
{
static const char* check_link(const ::dsa::Link link, const char* pString)
{
    return link == ::dsa::Link::STATE_UP ? pString : "";
}

uint32_t JsonGetPortstatus(char* out_buffer, uint32_t out_buffer_size)
{
    const auto kSize = static_cast<int32_t>(out_buffer_size);
    auto nLength = static_cast<int32_t>(1);

    out_buffer[0] = '[';

    for (uint32_t port_number = 0; port_number < ::dsa::MV88E6161_NUM_PORTS; port_number++)
    {
        ::dsa::PhyStatus phy_status;
        if (PortGetStatus(port_number, phy_status))
        {
            assert((nSize - nLength) > 0);
            nLength += snprintf(&out_buffer[nLength], static_cast<size_t>(kSize - nLength), "{\"port\":\"%u\",\"link\":\"%s\",\"speed\":\"%s\",\"duplex\":\"%s\",\"flowcontrol\":\"%s\"},", port_number,
                                ::dsa::phy_string_get_link(phy_status.link), check_link(phy_status.link, ::dsa::phy_string_get_speed(phy_status.speed)), check_link(phy_status.link, ::dsa::phy_string_get_duplex(phy_status.duplex)),
                                check_link(phy_status.link, ::dsa::phy_string_get_flowcontrol(phy_status.flowcontrol)));
        }
    }

    assert(nLength < static_cast<int32_t>(nOutBufferSize));
    // FIXME When port_get_status fails for all ports
    out_buffer[nLength - 1] = ']';

    return static_cast<uint32_t>(nLength);
}

uint32_t JsonGetVlantable(char* pOutBuffer, const uint32_t nOutBufferSize)
{
    const auto nSize = static_cast<int32_t>(nOutBufferSize);
    auto nLength = static_cast<int32_t>(1);

    pOutBuffer[0] = '[';

    for (uint32_t nPortNumber = 0; nPortNumber < ::dsa::MV88E6161_NUM_PORTS; nPortNumber++)
    {
        uint16_t nValue;
        if (::mv88e6xxx_port_read(nPortNumber, MV88E6XXX_PORT_BASE_VLAN, nValue) >= 0)
        {
            assert((nSize - nLength) > 0);
            nLength += snprintf(&pOutBuffer[nLength], static_cast<size_t>(nSize - nLength), "{\"port\":\"%u\",\"VLANTable\":\"0x%.2x\"},", nPortNumber, nValue & MV88E6XXX_PORT_BASE_VLAN_TABLE_MASK);
        }
    }

    assert(nLength < static_cast<int32_t>(nOutBufferSize));
    // FIXME When mv88e6xxx_port_read fails for all ports
    pOutBuffer[nLength - 1] = ']';

    return static_cast<uint32_t>(nLength);
}
} // namespace remoteconfig::dsa
