/**
 * @file Tilemap.h
 * @author Jackey Cen
 * @brief Tilemap should include grid data, layer and behavior logic, processing render logic to
 * send to renderer
 * @brief This class is the middle layer to convey gameplay logic to rendering, modification logic
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "TilemapRenderer.h"
#include "ChunkManager.h"
#include "ChunkLoader.h"
#include "Tile.h"
#include "Engine/EntitySystem/Component/Component.h"
#include "Engine/Renderer.h"
#include <functional>
#include <unordered_set>

namespace mts
{
    // Forward declaration
    struct RenderCamera;
    class Shader;

}  // namespace mts

namespace mts::Tilemap
{
    struct ChunkRenderData
    {
        // Currently assuming one texture atlas per chunk for now
        Texture* texture = nullptr;
        Shader* shader = nullptr;
        // Mesh* mesh = nullptr;         // No mesh because chunk holds its own VAO/VBO/EBO

        bool isValid() const { return texture != nullptr && shader != nullptr; }
    };

    // The unit here is tile count
    using TileOperation = std::function<void(TileID)>;

    // Currently assuming there will only be one tile map component for the whole game scene
    class Tilemap : public Component
    {
       public:
        Tilemap()
            : mChunkLoader(),
              mChunkManager(this, mChunkLoader),
              mRenderer(this),
              visibleChunks(mChunkManager.GetVisibleChunks())
        {
        }

        ~Tilemap();

        virtual void Init() override;

        virtual void Update(float dt) override;

        virtual void Render() override;

        virtual void DebugRender() override;

        void Deserialize(JsonValue const& jsonObj) override;
        void Serialize(JsonDocument& doc) const override;

        // TODO: some functions to operate the tile in the map

        // @brief: the coord system is based on the origin at bottom-left corner of the tilemap
        TileID GetTileAt(int x, int y) const;
        void SetTileAt(int x, int y, TileID tileID);
        void OperateTileInArea(int startX, int startY, int endX, int endY, TileOperation op);
        void OperateTile(int x, int y, TileOperation op);

        Tileset* GetTilesetBasedOnTileID(TileID id) const;
        // @brief: Get the range of cell grid indices that are currently visible in world space
        // @return: pair of (min, max) (unit in pixel) in world space
        std::pair<int, int> GetGridPixelWidthRangeInWorldSpace() const;
        std::pair<int, int> GetGridPixelHeightRangeInWorldSpace() const;
        ChunkPos WorldToChunkPos(float worldX, float worldY);
        ChunkPos WorldToChunkPos(const glm::vec3& worldPos);
        glm::vec3 ChunkPosToWorldPos(const ChunkPos& chunkPos) const;
        bool IsValidChunkPos(const ChunkPos& chunkPos) const;
        void LoadRenderData(const std::string& texturePath,
                            const std::string& shaderPath);
        ChunkRenderData& GetChunkRenderData() { return mChunkRenderData; }
        bool IsValidTilePosition(int x, int y) const;
        glm::vec2 GetLeftBottomWorldPos() const { return mLeftBottomWorldPos; }
        glm::vec2 GetRightTopWorldPos() const
        {
            return mLeftBottomWorldPos + glm::vec2(GetGridPixelWidthRangeInWorldSpace().second,
                                                   GetGridPixelHeightRangeInWorldSpace().second) /
                                             mts::PixelsPerUnit;
        }

        bool testflag = true;

       private:
        TilemapRenderer mRenderer;
        ChunkManager mChunkManager;
        ChunkLoader mChunkLoader;

        std::vector<std::vector<TileID>> tileGrid;  // 2D grid representing tile IDs
        std::vector<std::string>
            tilesetSources;  // List of tileset texture sources used in the tilemap
        std::unordered_map<int, std::set<TileID>>
            tilesetTileIDMap;  // Map of tileset index to set of TileIDs

        // potential large performance issue here if the tilemap is too large
        // @brief: reference to the visible chunks from chunk manager for rendering
        // std::vector<std::shared_ptr<Chunk>> visibleChunks;
        std::unordered_set<Chunk*> visibleChunks;

        bool mIsChunkDirty = false;
        std::vector<ChunkPos> dirtyChunks;

        std::string mTilemapName;
        bool mIsSegmentBased = false;

        // World position of the left-bottom corner of the tilemap
        glm::vec2 mLeftBottomWorldPos = glm::vec2(0.0f, 0.0f);

        std::string defaultTextureSource;
        std::string defaultShaderSource;
        ChunkRenderData mChunkRenderData;
    };

}  // namespace mts::Tilemap