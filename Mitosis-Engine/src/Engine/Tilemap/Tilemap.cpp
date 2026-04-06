/**
 * @file Tilemap.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Engine/Resource/Shader.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/ResourceManager.h"
#include "Tilemap.h"
#include "Tileset.h"
#include "Chunk.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#define DEFAULT_TILE_ID 0

namespace mts::Tilemap
{

    void Tilemap::LoadRenderData(const std::string& texturePath, const std::string& shaderPath)
    {
        // Load shader
        mChunkRenderData.shader = ResourceManager::GetShader(shaderPath);
        // Load Texture
        mChunkRenderData.texture = ResourceManager::GetTexture(texturePath);
    }

    Tilemap::~Tilemap()
    {
        mChunkLoader.Stop();
        mChunkRenderData.shader = nullptr;
        mChunkRenderData.texture = nullptr;
    }

    void Tilemap::Init()
    {
        // Init the camera related chunk loading
        mChunkLoader.Start();
        if (!mChunkRenderData.isValid())
        {
            LoadRenderData(defaultTextureSource, defaultShaderSource);
        }
    }

    void Tilemap::Update(float dt)
    {
        // TODO: gameplay logic update via layers system
    }

    void Tilemap::Render()
    {
        // Keep chunks up to date every frame regardless of play state
        mChunkManager.UpdateChunks(Renderer::GetActiveCamera());

        if (!mChunkRenderData.isValid())
            LoadRenderData(defaultTextureSource, defaultShaderSource);

        SubmitRenderCommand();
    }

    void Tilemap::SubmitRenderCommand() const
    {
        // submit RenderCommand for each chunks
        for (auto& chunk : mChunkManager.GetVisibleChunks())
        {
            if (!chunk->IsLoaded())
                continue;

            RenderCommand cmd;
            cmd.shader = mChunkRenderData.shader;
            cmd.texture = mChunkRenderData.texture;
            // Tilemap is using its own custom VAO and EBO
            // we set vao and indexCount instead of mesh
            cmd.vao = chunk->GetVAO();
            cmd.indexCount = chunk->GetIndexCount();

            glm::vec3 worldPos = ChunkPosToWorldPos(chunk->GetPos());
            cmd.model = glm::translate(glm::mat4(1.0f), worldPos);

            // default UV and tint for now
            // possible extension for per-chunk or even per-tile variation if needed
            cmd.uvOffset = glm::vec2(0.0f, 0.0f);
            cmd.tintColor = mTintColor;
            cmd.alpha = 1.0f;

            // sort the render command based on the world z position of the chunk
            cmd.sortKey = static_cast<float>(mRenderLayer);
            cmd.hasAlpha = true;

            Renderer::Submit(cmd);
        }
    }

    void Tilemap::Deserialize(JsonValue const& jsonObj)
    {
        if (jsonObj.Has("Name"))
        {
            mTilemapName = jsonObj["Name"].GetString();
        }

        if (jsonObj.Has("UsedTilesets"))
        {
            JsonValue usedTileset = jsonObj["UsedTilesets"];
            if (usedTileset.IsArray())
            {
                for (size_t i = 0; i < usedTileset.Size(); ++i)
                {
                    if (usedTileset[i].Has("TileIDs"))
                    {
                        int tilesetIndex = usedTileset[i]["TilesetIndex"].GetInt();

                        JsonValue tileIDs = usedTileset[i]["TileIDs"];
                        if (tileIDs.IsArray())
                        {
                            for (size_t j = 0; j < tileIDs.Size(); ++j)
                            {
                                TileID id = static_cast<TileID>(tileIDs[j].GetInt());
                                tilesetTileIDMap[tilesetIndex].insert(id);
                            }
                        }
                    }
                }
            }
        }

        // Segment-based tilemap attribute deserialization
        int SegmentWidthTiles = -1;
        int SegmentHeightTiles = -1;
        if (jsonObj.Has("SegmentWidthTiles"))
        {
            SegmentWidthTiles = jsonObj["SegmentWidthTiles"].GetInt();
            mIsSegmentBased = true;
        }
        if (jsonObj.Has("SegmentHeightTiles"))
        {
            SegmentHeightTiles = jsonObj["SegmentHeightTiles"].GetInt();
            mIsSegmentBased = true;
        }

        if (jsonObj.Has("LeftBottomTileTransformPosition"))
        {
            JsonValue startPos = jsonObj["LeftBottomTileTransformPosition"];
            mLeftBottomWorldPos.x = startPos[0].GetFloat();
            mLeftBottomWorldPos.y = startPos[1].GetFloat();
        }
        if (jsonObj.Has("TextureSourcePath"))
        {
            defaultTextureSource = jsonObj["TextureSourcePath"].GetString();
        }
        if (jsonObj.Has("ShaderPath"))
        {
            defaultShaderSource = jsonObj["ShaderPath"].GetString();
        }

        if (jsonObj.Has("TilesetsPath"))
        {
            JsonValue paths = jsonObj["TilesetsPath"];
            if (paths.IsArray())
            {
                for (size_t i = 0; i < paths.Size(); ++i)
                {
                    std::string path = paths[i].GetString();
                    tilesetSources.emplace_back(path);
                    Tileset::LoadTilesetFromFile(path);
                }
            }
        }

        // Deseiralization of grid data
        if (jsonObj.Has("TilemapCellGrid"))
        {
            JsonValue grid = jsonObj["TilemapCellGrid"];
            if (grid.IsArray())
            {
                size_t height = grid.Size();
                tileGrid.resize(height);
                for (int y = static_cast<int>(height) - 1; y >= 0; y--)
                {
                    // Fill up from top to bottom
                    JsonValue row = grid[y];
                    if (row.IsArray())
                    {
                        size_t width = row.Size();
                        tileGrid[height - y - 1].resize(width);
                        for (int x = 0; x < static_cast<int>(width); ++x)
                        {
                            tileGrid[height - y - 1][x] =
                                static_cast<TileID>(row[x].GetInt(DEFAULT_TILE_ID));
                        }
                    }
                }
            }
        }
        else if (jsonObj.Has("Segments"))
        {
            if (SegmentWidthTiles <= 0 || SegmentHeightTiles <= 0)
            {
                // Invalid segment size; cannot proceed
                LOG_WARN("Tilemap deserialization: Invalid segment size.");
                return;
            }

            // assuming the position index of each segment
            // are continuous, start from (0,0) and positive
            JsonValue segments = jsonObj["Segments"];
            if (segments.IsArray())
            {
                for (size_t i = 0; i < segments.Size(); ++i)
                {
                    JsonValue segment = segments[i];
                    if (segment.Has("Position") && segment.Has("GridData"))
                    {
                        JsonValue pos = segment["Position"];
                        int segX = pos[0].GetInt();
                        int segY = pos[1].GetInt();
                        JsonValue gridData = segment["GridData"];
                        if (gridData.IsArray())
                        {
                            size_t height = gridData.Size();
                            for (int y = static_cast<int>(height) - 1; y >= 0; --y)
                            {
                                JsonValue row = gridData[y];
                                if (row.IsArray())
                                {
                                    size_t width = row.Size();
                                    for (int x = 0; x < static_cast<int>(width); ++x)
                                    {
                                        int globalX = segX * SegmentWidthTiles + x;
                                        int globalY = segY * SegmentHeightTiles + y;
                                        // Ensure tileGrid is large enough
                                        if (globalY >= static_cast<int>(tileGrid.size()))
                                        {
                                            tileGrid.resize((segY + 1) * SegmentHeightTiles);
                                        }
                                        if (globalX >= static_cast<int>(tileGrid[globalY].size()))
                                        {
                                            tileGrid[globalY].resize((segX + 1) * SegmentWidthTiles,
                                                                     DEFAULT_TILE_ID);
                                        }
                                        tileGrid[globalY][globalX] =
                                            static_cast<TileID>(row[x].GetInt(DEFAULT_TILE_ID));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        InvalidateGridCache();
        LoadRenderData(defaultTextureSource, defaultShaderSource);
    }

    void Tilemap::Serialize(JsonDocument& doc) const
    {
        doc.WriteString("Type", "Tilemap");
        doc.BeginObject("Data");
        doc.WriteString("Name", mTilemapName.c_str());

        doc.WriteString("TextureSourcePath", defaultTextureSource.c_str());
        doc.WriteString("ShaderPath", defaultShaderSource.c_str());

        // Serialize Tilesets Data:
        doc.BeginArray("TilesetsPath");
        for (const auto& source : tilesetSources)
        {
            doc.WriteString(source.c_str());
        }
        doc.EndArray();  // End of TilesetsPath array

        // Serialize used tilesets
        doc.BeginArray("UsedTilesets");
        for (const auto& [tilesetIndex, tileIDSet] : tilesetTileIDMap)
        {
            doc.BeginObject();
            doc.WriteInt("TilesetIndex", tilesetIndex);
            doc.BeginArray("TileIDs");
            for (TileID id : tileIDSet)
            {
                doc.WriteInt(id);
            }
            doc.EndArray();   // End of TileIDs array
            doc.EndObject();  // End of this tileset object
        }
        doc.EndArray();  // End of UsedTilesets array

        // Serialize grid data
        doc.BeginArray("LeftBottomTileTransformPosition");
        doc.WriteFloat(mLeftBottomWorldPos.x);
        doc.WriteFloat(mLeftBottomWorldPos.y);
        doc.EndArray();  // End of LeftBottomTileTransformPosition array

        doc.BeginArray("TilemapCellGrid");
        for (int rowNumber = static_cast<int>(tileGrid.size()) - 1; rowNumber >= 0; rowNumber--)
        {
            const auto& row = tileGrid[rowNumber];
            doc.BeginArray();
            for (TileID id : row)
            {
                doc.WriteInt(id);
            }
            doc.EndArray();  // End of this row
        }
        doc.EndArray();  // End of TilemapCellGrid array
        // TODO: Serialize segment-based tilemap attributes if applicable
        doc.EndObject();  // End of Data object
    }

    void Tilemap::OperateTile(int x, int y, TileOperation op)
    {
        if (IsValidTilePosition(x, y))
        {
            TileID& tileID = tileGrid[y][x];
            op(tileID);
            mIsChunkDirty = true;
        }
    }

    Tileset* Tilemap::GetTilesetBasedOnTileID(TileID id) const
    {
        for (const auto& [tilesetIndex, tileIDSet] : tilesetTileIDMap)
        {
            if (tileIDSet.find(id) != tileIDSet.end())
            {
                return Tileset::GetTilesetById(tilesetIndex);
            }
        }
        return nullptr;  // Not found
    }

    void Tilemap::OperateTileInArea(int startX, int startY, int endX, int endY, TileOperation op)
    {
        for (int y = startY; y <= endY; ++y)
        {
            for (int x = startX; x <= endX; ++x)
            {
                if (IsValidTilePosition(x, y))
                {
                    TileID& tileID = tileGrid[y][x];
                    op(tileID);
                }
            }
        }
        mIsChunkDirty = true;
    }

    TileID Tilemap::GetTileAt(int x, int y) const
    {
        if (IsValidTilePosition(x, y))
        {
            return tileGrid[y][x];
        }
        return DEFAULT_TILE_ID;
    }

    void Tilemap::SetTileAt(int x, int y, TileID tileID)
    {
        if (IsValidTilePosition(x, y))
        {
            tileGrid[y][x] = tileID;
            mIsChunkDirty = true;
        }
    }

    bool Tilemap::IsValidTilePosition(int x, int y) const
    {
        return y >= 0 && y < static_cast<int>(tileGrid.size()) && x >= 0 &&
               x < static_cast<int>(tileGrid[y].size());
    }

    ChunkPos Tilemap::WorldToChunkPos(float worldX, float worldY)
    {
        int32_t chunkX = static_cast<int32_t>(
            glm::floor(worldX / (Chunk::CHUNK_PIXEL_WIDTH / mts::PixelsPerUnit)));
        int32_t chunkY = static_cast<int32_t>(
            glm::floor(worldY / (Chunk::CHUNK_PIXEL_HEIGHT / mts::PixelsPerUnit)));
        return ChunkPos{chunkX, chunkY};
    }

    ChunkPos Tilemap::WorldToChunkPos(const glm::vec3& worldPos)
    {
        return WorldToChunkPos(worldPos.x, worldPos.y);
    }

    glm::vec3 Tilemap::ChunkPosToWorldPos(const ChunkPos& chunkPos) const
    {
        glm::vec3 res = glm::vec3(
            mLeftBottomWorldPos.x +
                static_cast<float>(chunkPos.x * Chunk::CHUNK_PIXEL_WIDTH / mts::PixelsPerUnit),
            mLeftBottomWorldPos.y +
                static_cast<float>(chunkPos.y * Chunk::CHUNK_PIXEL_HEIGHT / mts::PixelsPerUnit),
            0.0f);
        return res;
    }

    bool Tilemap::IsValidChunkPos(const ChunkPos& chunkPos) const
    {
        // check if the chunk position is within the tilemap bounds
        // A chunk is valid if it overlaps with the tilemap area at all
        std::pair<int, int> widthRange = GetGridPixelWidthRangeInWorldSpace();
        std::pair<int, int> heightRange = GetGridPixelHeightRangeInWorldSpace();
        int32_t minChunkX = static_cast<int32_t>(
            std::floor(static_cast<float>(widthRange.first) / Chunk::CHUNK_PIXEL_WIDTH));
        int32_t maxChunkX =
            static_cast<int32_t>((widthRange.second - 1) / Chunk::CHUNK_PIXEL_WIDTH);
        int32_t minChunkY = static_cast<int32_t>(
            std::floor(static_cast<float>(heightRange.first) / Chunk::CHUNK_PIXEL_HEIGHT));
        int32_t maxChunkY =
            static_cast<int32_t>((heightRange.second - 1) / Chunk::CHUNK_PIXEL_HEIGHT);
        return chunkPos.x >= minChunkX && chunkPos.x <= maxChunkX && chunkPos.y >= minChunkY &&
               chunkPos.y <= maxChunkY;
    }

    void Tilemap::DebugRender()
    {
        // Debug render the tilemap grid
        mChunkManager.DebugDraw();
        for (auto& chunk : mChunkManager.GetVisibleChunks())
        {
            mRenderer.DebugDraw(*chunk);
        }
    }

    std::pair<int, int> Tilemap::GetGridPixelWidthRangeInWorldSpace() const
    {
        if (tileGrid.empty())
            return {0, 0};
        if (mCachedMaxWidth < 0)
        {
            mCachedMaxWidth = 0;
            for (const auto& row : tileGrid)
                if (static_cast<int>(row.size()) > mCachedMaxWidth)
                    mCachedMaxWidth = static_cast<int>(row.size());
        }
        return {
            static_cast<int>(mLeftBottomWorldPos.x),
            static_cast<int>(mLeftBottomWorldPos.x) + mCachedMaxWidth * Tileset::TILE_PIXEL_WIDTH};
    }

    std::pair<int, int> Tilemap::GetGridPixelHeightRangeInWorldSpace() const
    {
        if (tileGrid.empty())
            return {0, 0};
        if (mCachedGridHeight < 0)
            mCachedGridHeight = static_cast<int>(tileGrid.size());
        return {static_cast<int>(mLeftBottomWorldPos.y),
                static_cast<int>(mLeftBottomWorldPos.y) +
                    mCachedGridHeight * Tileset::TILE_PIXEL_HEIGHT};
    }
    Component* Tilemap::Clone(GameObject* newOwner) const
    {
        Tilemap* c = new Tilemap();             // fresh construction — back-pointers correct
        c->mTilemapName         = mTilemapName;
        c->mIsSegmentBased      = mIsSegmentBased;
        c->mLeftBottomWorldPos  = mLeftBottomWorldPos;
        c->defaultTextureSource = defaultTextureSource;
        c->defaultShaderSource  = defaultShaderSource;
        c->tileGrid             = tileGrid;
        c->tilesetSources       = tilesetSources;
        c->tilesetTileIDMap     = tilesetTileIDMap;
        c->InitFromClone(newOwner);
        return c;
    }

}  // namespace mts::Tilemap