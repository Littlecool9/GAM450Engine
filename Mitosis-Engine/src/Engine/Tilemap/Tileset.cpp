/**
 * @file Tileset.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#include "pch.h"
#include "Engine/Renderer.h"
#include "Tileset.h"
#include <filesystem>
#include "Engine/Tilemap/Chunk.h"

namespace mts::Tilemap
{
    // TODO: For some reason the data in json file can't be larger than the one defined here, or it
    // may not render, fix it later
    int Tileset::TILE_PIXEL_WIDTH = 1;
    int Tileset::TILE_PIXEL_HEIGHT = 1;

    std::vector<Tileset*> Tileset::loadedTilesets;

    Tileset* Tileset::GetTilesetById(int id)
    {
        for (Tileset* tileset : loadedTilesets)
        {
            if (tileset->id == static_cast<uint32_t>(id))
            {
                return tileset;
            }
        }
        return nullptr;
    }

    void Tileset::LoadTilesetFromFile(const std::string& filepath)
    {
        mts::JsonDocument doc;
        if (!doc.LoadFromFile(filepath))
        {
            std::filesystem::path currentPath = std::filesystem::current_path();
            LOG_INFO("Current Path: {}", currentPath.string());
            LOG_WARN("Failed to load tileset file: {}", filepath.c_str());
            return;
        }
        Tileset* newTileset = new Tileset();
        newTileset->Deserialize(doc.Root());

        Tileset* existingTileset = GetTilesetById(static_cast<int>(newTileset->id));
        if (existingTileset != nullptr)
        {
            LOG_WARN("Tileset with ID {} is already loaded. Skipping duplicate load from file: {}",
                     newTileset->id,
                     filepath.c_str());
            delete newTileset;
            return;
        }
        loadedTilesets.push_back(newTileset);
        LOG_INFO("Loaded tileset: {}", filepath.c_str());

        // Update Chunk size
        // Under this implementation, the size of each tile and chunk depends on the last loaded
        // tileset. It could only be solved after figuring out the relation between tilesets and
        // tilemaps
        Chunk::CHUNK_PIXEL_WIDTH = Chunk::CHUNK_WIDTH * Tileset::TILE_PIXEL_WIDTH;
        Chunk::CHUNK_PIXEL_HEIGHT = Chunk::CHUNK_HEIGHT * Tileset::TILE_PIXEL_HEIGHT;
        Chunk::CHUNK_UNIT_WIDTH = Chunk::CHUNK_PIXEL_WIDTH / static_cast<float>(mts::PixelsPerUnit);
        Chunk::CHUNK_UNIT_HEIGHT =
            Chunk::CHUNK_PIXEL_HEIGHT / static_cast<float>(mts::PixelsPerUnit);
    }

    void Tileset::UnloadAllTileset()
    {
        for (Tileset* tileset : loadedTilesets)
        {
            delete tileset;
        }
        loadedTilesets.clear();
    }

    const TileProperties& Tileset::Get(TileID id) const
    {
        auto it = tileDefinitions.find(id);
        if (it != tileDefinitions.end())
        {
            return it->second;
        }
        static TileProperties defaultProperties;
        return defaultProperties;  // return default if not found
    }

    void TileProperties::Deserialize(JsonValue const& jsonObj, Tileset const& parent)
    {
        if (jsonObj.Has("Name"))
        {
            name = jsonObj["Name"].GetString();
        }
        if (jsonObj.Has("IndexX"))
        {
            int indexX = jsonObj["IndexX"].GetInt();
            uvOffsetX = indexX * (1.0f / parent.ATLAS_TILE_COUNT_PER_ROW);
        }
        if (jsonObj.Has("IndexY"))
        {
            int indexY = jsonObj["IndexY"].GetInt();
            uvOffsetY = indexY * (1.0f / parent.ATLAS_TILE_COUNT_PER_COL);
        }
        if (jsonObj.Has("Tag"))
        {
            tag = jsonObj["Tag"].GetString();
        }
        if (jsonObj.Has("IsCollidable"))
        {
            isCollidable = jsonObj["IsCollidable"].GetBool();
        }
        if (jsonObj.Has("IsAnimated"))
        {
            isAnimated = jsonObj["IsAnimated"].GetBool();
        }

        if (jsonObj.Has("IsFullTile"))
        {
            isFullTile = jsonObj["IsFullTile"].GetBool();
        }
    }

    void Tileset::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("TilePixelWidth"))
        {
            TILE_PIXEL_WIDTH = jsonObj["TilePixelWidth"].GetInt();
        }
        if (jsonObj.Has("TilePixelHeight"))
        {
            TILE_PIXEL_HEIGHT = jsonObj["TilePixelHeight"].GetInt();
        }
        if (jsonObj.Has("AtlasTileCountPerRow"))
        {
            ATLAS_TILE_COUNT_PER_ROW = jsonObj["AtlasTileCountPerRow"].GetFloat();
        }
        if (jsonObj.Has("AtlasTileCountPerCol"))
        {
            ATLAS_TILE_COUNT_PER_COL = jsonObj["AtlasTileCountPerCol"].GetFloat();
        }

        if (jsonObj.Has("Name"))
        {
            tilesetName = jsonObj["Name"].GetString();
        }
        if (jsonObj.Has("TilesetId"))
        {
            id = jsonObj["TilesetId"].GetInt();
        }
        if (jsonObj.Has("TileDefinitions"))
        {
            JsonValue tileDefs = jsonObj["TileDefinitions"];
            if (tileDefs.IsArray())
            {
                for (size_t i = 0; i < tileDefs.Size(); ++i)
                {
                    JsonValue tileDef = tileDefs[i];
                    if (tileDef.Has("TileID"))
                    {
                        TileID id = static_cast<TileID>(tileDef["TileID"].GetInt());
                        TileProperties properties;
                        properties.Deserialize(tileDef, *this);
                        tileDefinitions[id] = properties;
                    }
                }
            }
        }
    }

    void Tileset::Serialize(JsonDocument& doc) const
    {
        // TODO: Implement serialization logic if needed
    }
}  // namespace mts::Tilemap