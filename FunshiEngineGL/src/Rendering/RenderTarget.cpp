/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "RenderTarget.h"

using Rendering::Backend::IRenderBackend;
using Rendering::Backend::kInvalidHandle;

RenderTarget::~RenderTarget() { destroy(); }

void RenderTarget::destroy() {
    IRenderBackend& backend = Rendering::Backend::activeBackend();
    if (target_ != kInvalidHandle) {
        backend.destroyRenderTarget(target_);
        target_ = kInvalidHandle;
    }
    colorTex_ = kInvalidHandle;
    width = 0;
    height = 0;
}

void RenderTarget::resize(int w, int h) {
    if (w <= 0 || h <= 0) return;
    if (width == w && height == h) return;

    destroy();

    target_ = Rendering::Backend::activeBackend().createRenderTarget(w, h);
    if (target_ == kInvalidHandle) {
        colorTex_ = kInvalidHandle;
        width = height = 0;
        return;
    }
    colorTex_ = Rendering::Backend::activeBackend().renderTargetColorTexture(
        target_);
    width = w;
    height = h;
}

void RenderTarget::bind() {
    if (target_ == kInvalidHandle) return;
    Rendering::Backend::activeBackend().bindRenderTarget(target_);
}

void RenderTarget::unbind() {
    Rendering::Backend::activeBackend().bindDefaultFramebuffer();
}