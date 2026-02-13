/**
 * @file Chunk.cpp
 * @author Jackey Cen
 * @brief // TODO: Add a brief description of this class
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#include "pch.h"
#include "Chunk.h"
#include "Tileset.h"
#include "Tilemap.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/Shader.h"
#include "Engine/EntitySystem/GameObject/GameObjectFactory.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/BoxCollider2D.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/PhysicsComponents/Rigidbody2D.h"

#define TILE_PAD 0.003f  // Padding to prevent texture bleeding

namespace mts::Tilemap
{
    // static variable definitions if any
    int Chunk::CHUNK_PIXEL_HEIGHT = CHUNK_HEIGHT * Tileset::TILE_PIXEL_HEIGHT;
    int Chunk::CHUNK_PIXEL_WIDTH = CHUNK_WIDTH * Tileset::TILE_PIXEL_WIDTH;
    float Chunk::CHUNK_UNIT_WIDTH = CHUNK_PIXEL_WIDTH / static_cast<float>(mts::PixelsPerUnit);
    float Chunk::CHUNK_UNIT_HEIGHT = CHUNK_PIXEL_HEIGHT / static_cast<float>(mts::PixelsPerUnit);

    bool Chunk::Load()
    {
        // Step 1: Load tile data for this chunk

        // Here goes option B
        if (mParentTilemap == nullptr)
            return false;
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int x = 0; x < CHUNK_WIDTH; ++x)
            {
                TileID tileID =
                    mParentTilemap->GetTileAt(x + mPos.x * CHUNK_WIDTH, y + mPos.y * CHUNK_HEIGHT);
                mTiles[y * CHUNK_WIDTH + x] = tileID;
                if (tileID != 0 && !mTileset)
                {
                    mTileset = mParentTilemap->GetTilesetBasedOnTileID(tileID);
                }
            }
        }

        // Step 2: Generate vertices for the first time or if dirty
        // Build the CPU - side vertex and index buffers
        GenerateVertices();

        mIsDirty = !(mcpuVertices.empty());  // Mark as dirty to build mesh
        mIsLoaded = true;                    // Data is ready
        return true;
    }

    void Chunk::BuildMesh()
    {
        if (mIsDirty)
        {
            GenerateVertices();
        }

        // Load Vertices and Indices to GPU
        // If no vertices to render, skip mesh building
        if (mcpuVertices.empty())
            return;

        // Create/regenerate OpenGL buffers
        if (mVAO == 0)
            glGenVertexArrays(1, &mVAO);
        if (mVBO == 0)
            glGenBuffers(1, &mVBO);
        if (mEBO == 0)
            glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);

        // VBO Upload
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     mcpuVertices.size() * sizeof(TileVertex),
                     mcpuVertices.data(),
                     GL_STATIC_DRAW);

        // EBO Upload
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mcpuIndices.size() * sizeof(unsigned int),
                     mcpuIndices.data(),
                     GL_STATIC_DRAW);

        // Layout: Pos (vec2)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(TileVertex),
                              (void*) offsetof(TileVertex, pos));

        // Layout: UV (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(TileVertex),
                              (void*) offsetof(TileVertex, uv));

        glBindVertexArray(0);  // Unbind

        mIndexCount = static_cast<GLsizei>(mcpuIndices.size());

        mIsDirty = false;
    }

    bool Chunk::Unload()
    {
        // release OpenGL resources
        if (mVAO != 0)
        {
            glDeleteVertexArrays(1, &mVAO);
            mVAO = 0;
        }
        if (mVBO != 0)
        {
            glDeleteBuffers(1, &mVBO);
            mVBO = 0;
        }
        if (mEBO != 0)
        {
            glDeleteBuffers(1, &mEBO);
            mEBO = 0;
        }

        mIndexCount = 0;
        mIsLoaded = false;

        // Clear CPU-side buffers
        mcpuVertices.clear();
        mcpuIndices.clear();
        ClearColliders();

        // optional Clearing tile data
        // std::memset(mTiles, 0, sizeof(mTiles));

        return true;
    }

    void Chunk::SetTile(int localX, int localY, TileID tileID)
    {
        if (localX < 0 || localX >= CHUNK_WIDTH || localY < 0 || localY >= CHUNK_HEIGHT)
        {
            LOG_WARN("Chunk SetTile: Local position out of bounds.");
            return;
        }
        mTiles[localY * CHUNK_WIDTH + localX] = tileID;
        mIsDirty = true;
    }

    TileID Chunk::GetTile(int localX, int localY) const
    {
        if (localX < 0 || localX >= CHUNK_WIDTH || localY < 0 || localY >= CHUNK_HEIGHT)
        {
            LOG_WARN("Chunk GetTile: Local position out of bounds.");
            return 0;  // Default tile ID
        }
        return mTiles[localY * CHUNK_WIDTH + localX];
    }

    void Chunk::GenerateVertices()
    {
        // if (!mIsDirty && !mcpuVertices.empty())
        //     return;  // No need to regenerate

        mcpuVertices.clear();
        mcpuIndices.clear();

        unsigned int indexOffset = 0;

        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int x = 0; x < CHUNK_WIDTH; ++x)
            {
                TileID id = GetTile(x, y);

                // Skipping empty tiles (ID 0)
                if (id == 0)
                    continue;

                Tileset* tileset = mParentTilemap->GetTilesetBasedOnTileID(id);
                if (!tileset)
                    continue;

                const TileProperties& props = tileset->Get(id);

                // Calculate local position within the chunk
                float localX = x * Tileset::TILE_PIXEL_WIDTH / mts::PixelsPerUnit;
                float localY = y * Tileset::TILE_PIXEL_HEIGHT / mts::PixelsPerUnit;

                float t = localY;
                float b = localY + Tileset::TILE_PIXEL_HEIGHT / mts::PixelsPerUnit;
                float l = localX;
                float r = localX + Tileset::TILE_PIXEL_WIDTH / mts::PixelsPerUnit;

                // Calculate UV (assuming Tileset provides normalized UV offsets and sizes)

                // Notice: Designer need to ensure that TileProperties has UV width and height, or
                // it is fixed
                float uvX = props.uvOffsetX + TILE_PAD;
                float uvY = props.uvOffsetY + TILE_PAD;
                float uvW = 1.0f / tileset->ATLAS_TILE_COUNT_PER_ROW - 2 * TILE_PAD;
                float uvH = 1.0f / tileset->ATLAS_TILE_COUNT_PER_COL - 2 * TILE_PAD;

                // Define vertices
                // Assuming UV (0,0) is top-left
                TileVertex VBL = {{l, b}, {uvX, uvY}};
                TileVertex VBR = {{r, b}, {uvX + uvW, uvY}};
                TileVertex VTL = {{l, t}, {uvX, uvY + uvH}};
                TileVertex VTR = {{r, t}, {uvX + uvW, uvY + uvH}};

                mcpuVertices.push_back(VBL);  // 0
                mcpuVertices.push_back(VBR);  // 1
                mcpuVertices.push_back(VTR);  // 2
                mcpuVertices.push_back(VTL);  // 3

                // Building indices (two tringles: 0-1-3, 1-2-3)
                mcpuIndices.push_back(indexOffset + 0);
                mcpuIndices.push_back(indexOffset + 1);
                mcpuIndices.push_back(indexOffset + 3);
                mcpuIndices.push_back(indexOffset + 1);
                mcpuIndices.push_back(indexOffset + 2);
                mcpuIndices.push_back(indexOffset + 3);

                indexOffset += 4;
            }
        }
    }

    // Greedy Max Rectangle Merging Algorithm
    std::vector<ColliderRect> Chunk::GenerateColliderRects()
    {
        std::vector<ColliderRect> rects;

        // reset visited array
        std::memset(mVisited, 0, sizeof(mVisited));

        if (!mTileset && !mcpuVertices.empty())
        {
            LOG_WARN("Chunk GenerateColliderRects: Tileset not found.");
            return rects;
        }

        // Iterate each tile in the chunk
        for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
            for (int x = 0; x < CHUNK_WIDTH; ++x)
            {
                // Skip all visited or non-collidable tiles
                if (mVisited[y][x])
                    continue;

                TileID id = GetTile(x, y);
                if (id == 0)
                    continue;

                if (!mTileset)
                    mTileset = mParentTilemap->GetTilesetBasedOnTileID(id);
                const TileProperties& props = mTileset->Get(id);

                if (!props.isCollidable)
                    continue;

                // 1. Expand rightwards (Width)
                int width = 1;
                while (x + width < CHUNK_WIDTH)
                {
                    TileID nextId = GetTile(x + width, y);
                    const TileProperties& nextProps = mTileset->Get(nextId);

                    if (!nextProps.isCollidable || mVisited[y][x + width])
                        break;
                    width++;
                }

                // 2 Expand downwards (Height)
                // Examine all tiles in the next row within the current width
                int height = 1;
                while (y + height < CHUNK_HEIGHT)
                {
                    bool rowMatch = true;
                    for (int k = 0; k < width; ++k)
                    {
                        TileID checkId = GetTile(x + k, y + height);
                        const TileProperties& checkProps = mTileset->Get(checkId);

                        if (!checkProps.isCollidable || mVisited[y + height][x + k])
                        {
                            rowMatch = false;
                            break;
                        }
                    }
                    if (!rowMatch)
                        break;
                    height++;
                }

                // 3. Mark these tiles as visited
                for (int h = 0; h < height; ++h)
                {
                    for (int w = 0; w < width; ++w)
                    {
                        mVisited[y + h][x + w] = true;
                    }
                }

                // 4. Generate the rectangle
                // rect.x/y is the top-left corner
                ColliderRect rect;
                rect.x = x * mTileset->TILE_PIXEL_WIDTH / mts::PixelsPerUnit;
                rect.y = y * mTileset->TILE_PIXEL_HEIGHT / mts::PixelsPerUnit;
                rect.w = width * mTileset->TILE_PIXEL_WIDTH / mts::PixelsPerUnit;
                rect.h = height * mTileset->TILE_PIXEL_HEIGHT / mts::PixelsPerUnit;

                rects.push_back(rect);
            }
        }
        return rects;
    }

    void Chunk::BuildColliders()
    {
        if (!mIsLoaded)
            return;

        ClearColliders();
        std::vector<ColliderRect> rects = GenerateColliderRects();

        // Get chunk's world position using the same function as rendering
        glm::vec3 chunkWorldPos = mParentTilemap->ChunkPosToWorldPos(mPos);
        // Build GameObjects with BoxCollider2D for each rectangle
        for (const auto& rect : rects)
        {
            // Step A: Create a child object/temporary object with a collider
            // Its position should be the Chunk's world position + rectangle's center offset
            GameObject* colObj = GameObjectFactory::CreateGameObject();

            // Step B: Calculate the world position center point
            // rect.x/y is the top-left corner, we need the center point for BoxCollider2D
            float worldX = chunkWorldPos.x + rect.x + (rect.w / 2.0f);
            float worldY = chunkWorldPos.y + rect.y + (rect.h / 2.0f);

            auto* transform = colObj->AddComponent<Transform>();
            transform->SetPosition(glm::vec3(worldX, worldY, 0.0f));
            
            RigidBody2D* rb = colObj->AddComponent<RigidBody2D>();
            rb->SetBody(RigidBody2D::BodyType::Static);

            auto* box = colObj->AddComponent<BoxCollider2D>();
            box->SetWidth(rect.w);
            box->SetHeight(rect.h);

            colObj->SetLayer(GameObject::Ground);

            // Step D: Record reference for unloading
            mColliderObjects.push_back(colObj);
        }
    }

    void Chunk::ClearColliders()
    {
        for (auto* obj : mColliderObjects)
        {
            delete obj;
        }
        mColliderObjects.clear();
    }

    void Chunk::DebugDrawCollider()
    {
        for (const auto& colObj : mColliderObjects)
        {
            if (auto* box = colObj->GetComponent<BoxCollider2D>())
            {
                box->DebugRender();
            }
        }
    }

}  // namespace mts::Tilemap