/**
 * @file Chunk.h
 * @author Jackey Cen
 * @brief Chunk represents a section of the tilemap to optimize memory and performance
 * @brief A chunk is the basic unit for loading/unloading and updating the tilemap
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include "Tile.h"
#include "Tileset.h"
#include <glad/glad.h>

namespace mts
{
    class Texture;
    class Mesh;
    class Shader;
    class GameObject;
}  // namespace mts

namespace mts::Tilemap
{
    struct ChunkPos
    {
        // The pos here is the left-bottom tile index of the chunk in the tilemap
        // the index is relative to the tilemap origin (0,0)
        int32_t x, y;
        bool operator==(const ChunkPos& other) const { return x == other.x && y == other.y; }
        bool operator<(const ChunkPos& other) const
        {
            if (x != other.x)
                return x < other.x;
            return y < other.y;
        }
    };

    struct TileVertex
    {
        glm::vec2 pos;  // the tile position in the world space
        glm::vec2 uv;   // uv offset of a tile in the tileset atlas
    };

    struct ColliderRect
    {
        float x, y, w, h;  // x, y is the top-left corner
    };

}  // namespace mts::Tilemap

namespace std
{
    template <>
    struct hash<mts::Tilemap::ChunkPos>
    {
        size_t operator()(const mts::Tilemap::ChunkPos& p) const
        {
            return std::hash<int32_t>{}(p.x) ^ (std::hash<int32_t>{}(p.y) << 1);
        }
    };
}  // namespace std

namespace mts::Tilemap
{
    class Tilemap;
    class TilemapRenderer;

    class Chunk
    {
       public:

        // The unit here is tile count
        // Chunk size has to be dividable by the tile pixel size
        // or zigzag rendering artifacts may occur
        static constexpr int CHUNK_WIDTH = 8;  // in tiles
        static constexpr int CHUNK_HEIGHT = 8;  // in tiles
        static constexpr int TOTAL_TILES = CHUNK_WIDTH * CHUNK_HEIGHT;   // CHUNK_WIDTH * CHUNK_HEIGHT
        static int CHUNK_PIXEL_WIDTH;  // in pixels
        static int CHUNK_PIXEL_HEIGHT;  // in pixels
        static float CHUNK_UNIT_WIDTH;  // in world units
        static float CHUNK_UNIT_HEIGHT;  // in world units

       public:

        Chunk(ChunkPos pos) : mPos(pos) {}

        // @brief: Loads the tile data for this chunk from disk or other sources
        bool Load();
        // @brief: Unloads the chunk and frees GPU resources
        // @brief: Must be called in the main thread with OpenGL context
        bool Unload();

        // @brief: Builds the mesh for this chunk with OpenGL calls
        // @brief: Must be called in the main thread with OpenGL context
        void BuildMesh();
        GLuint GetVAO() const { return mVAO; }
        GLsizei GetIndexCount() const { return mIndexCount; }
        void BindTilemap(Tilemap* tilemap) { mParentTilemap = tilemap; }

        void BuildColliders();
        void ClearColliders();

        void SetTile(int localX, int localY, TileID tileID);
        TileID GetTile(int localX, int localY) const;
        bool IsDirty() const { return mIsDirty; }
        void SetClean() { mIsDirty = false; }
        void SetPos(ChunkPos pos) { mPos = pos; }
        ChunkPos GetPos() const { return mPos; }
        const TileID* GetData() const { return mTiles; }
        bool IsLoaded() const { return mIsLoaded; }

        void DebugDrawCollider();

       private:
        void GenerateVertices();
        std::vector<ColliderRect> GenerateColliderRects();

        // State flags
        bool mIsDirty = true;    // Flag indicating if the render buffer needs to be rebuilt
        bool mIsLoaded = false;  // Whether the chunk is ready for rendering

        // Data: contiguous memory storage for TileID
        TileID mTiles[TOTAL_TILES] = {0};
        ChunkPos mPos;
        Tilemap* mParentTilemap = nullptr;
        Tileset* mTileset = nullptr;

        // Render helpers: handles to GPU buffers
        // OpenGL handles
        GLuint mVAO = 0;
        GLuint mVBO = 0;
        GLuint mEBO = 0;
        GLsizei mIndexCount = 0;
        std::vector<TileVertex> mcpuVertices;
        std::vector<unsigned int> mcpuIndices;

        std::vector<GameObject*> mColliderObjects;
        bool mVisited[CHUNK_HEIGHT][CHUNK_WIDTH];
    };
}  // namespace mts::Tilemap