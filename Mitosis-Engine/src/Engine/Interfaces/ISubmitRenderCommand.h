/**
 * @file ISubmitRenderCommand.h
 * @author Jackey Cen
 * @brief An interface for components that can submit render commands to the Renderer.
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    struct RenderCommand;
    class ISubmitRenderCommand
    {
       public:
        virtual ~ISubmitRenderCommand() = default;
        virtual void SubmitRenderCommand() const = 0;

       public:
        int mRenderLayer = 0;  // Default render layer is 0
    };

}  // namespace mts