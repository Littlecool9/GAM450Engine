/**
 * @file Tile.h
 * @author Jackey Cen
 * @brief Tile represents a tile definition shared by many cells. It is NOT a runtime tile instance.
 * @brief Currently only holds TileID, more properties like gameplay or lifecycle related logic can be added later
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

#include <string>

namespace mts::Tilemap
{
    using TileID = uint16_t;

    class Tile
    {
       public:
        TileID GetID() const { return mId; }
       private:
        TileID mId;

        // uv offset in the tileset texture atlas
    };

}  // namespace mts::Tilemap