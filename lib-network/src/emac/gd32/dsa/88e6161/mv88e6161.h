#pragma once
/*
 * mv88e6161.h
 */

#include <cstdint>

namespace dsa
{
static constexpr uint32_t MV88E6161_NUM_INTERNAL_PHYS = 5;
static constexpr uint32_t MV88E6161_NUM_PORTS = 6;

enum class Link
{
    STATE_UP,
    STATE_DOWN
};

enum class Duplex
{
    DUPLEX_HALF,
    DUPLEX_FULL
};

enum class Speed
{
    SPEED10,
    SPEED100,
    SPEED1000,
    SPEED10000
};

struct PhyStatus
{
    Link link;
    Duplex duplex;
    Speed speed;
    bool flowcontrol;
};

/**
 * Reading a given PHY register
 * @param address PHY address
 * @param reg PHY register number to read
 * @param value Returned value
 * @return
 */
bool PhyRead(uint32_t address, uint32_t reg, uint16_t& value);

/**
 *
 * @param address PHY address
 * @param reg PHY register number to write
 * @param value Value to write
 * @return true for success, false for failure
 */
bool PhyWrite(uint32_t address, uint32_t reg, uint16_t value);

const char* phy_string_get_link(Link link);
const char* phy_string_get_duplex(Duplex duplex);
const char* phy_string_get_speed(Speed speed);
const char* phy_string_get_flowcontrol(bool flowcontrol);

bool PhyGetStatus(const uint32_t nPhyIndex, dsa::PhyStatus& phy_status);
bool PortGetStatus(const uint32_t nPortIndex, dsa::PhyStatus& phy_status);
} // namespace dsa
