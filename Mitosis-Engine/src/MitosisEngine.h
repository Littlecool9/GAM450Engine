/**
 * @file MitosisEngine.h
 * @author sumin.park
 * @brief The only header that apps using the engine will utilize,
 *		  and will only be utilized by those apps - the engine will not.
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

#include "Engine/Application.h"
#include "Engine/Logging/AppLog.h"
#include "Engine/Logging/Assert.h"
#include "Engine/Math/Vec3.h"
#include "Engine/SceneSystem/Scene.h"
#include "Engine/EntitySystem/GameObject/GameObject.h"
#include "Engine/EntitySystem/Component/Transform.h"
#include "Engine/EntitySystem/Component/MeshRenderer.h"

#include "Engine/Resource/Material.h"
#include "Engine/Resource/Mesh.h"
