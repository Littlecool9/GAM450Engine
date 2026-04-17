/**
 * @file Tileset.h
 * @author Jackey Cen
 * @brief This is a map between the definition of a tile and the ID that is used in tile map
 * @brief decoupling the tile data with the rendering and gameplay logical layer
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <unordered_map>
#include "Engine/Serialization/Deserializable.h"
#include "Tile.h"
#include <string>

namespace mts::Tilemap
{
    class Tileset;
    struct TileProperties
    {
        std::string name;

        float uvOffsetX = 0.0f;  // UV offset X in the tileset texture atlas
        float uvOffsetY = 0.0f;  // UV offset Y in the tileset texture atlas

        std::string tag;            // gameplay tag
        bool isCollidable = false;  // physics layer
        bool isAnimated = false;    // whether the tile has animation frames

        // Consider adding system-specific properties struct later

        // whether the tile occupies the full tile space (for collision and rendering)
        bool isFullTile = true;

        void Deserialize(JsonValue const& jsonObj, Tileset const& parent);
    };

    class Tileset : public Deserializable
    {
       public:
        // Global tile pixel size for all tilesets
        static int TILE_PIXEL_WIDTH;
        static int TILE_PIXEL_HEIGHT;
        static std::vector<Tileset*> loadedTilesets;  // Global registry of loaded tilesets

       public:
        static Tileset* GetTilesetById(int id);
        static void LoadTilesetFromFile(const std::string& filepath);
        static void UnloadAllTileset();

        // A tileset may be newed by MTS_ALLOC_T
        // I hope this function macro can help to avoid memory leak
        MTS_CLASS_NEW_DELETE();

       public:
        Tileset() = default;

        const TileProperties& Get(TileID id) const;

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        float ATLAS_TILE_COUNT_PER_ROW = 1.0f;
        float ATLAS_TILE_COUNT_PER_COL = 1.0f;

       private:
        std::string tilesetName;  // Name of the tileset
        uint32_t id = 1;
        std::unordered_map<TileID, TileProperties>
            tileDefinitions;      // Map of tile ID to Tile definitions
    };

}  // namespace mts::Tilemap