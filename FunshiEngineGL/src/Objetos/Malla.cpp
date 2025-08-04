#include "Malla.h"
#include <stdexcept>

Malla::Malla(VulkanContext* context, float size, float step)
    : context(context), size(size), step(step) {
    generarVertices();
    crearBuffers();
}

Malla::~Malla() {
    destruirBuffers();
}

void Malla::generarVertices() {
    vertices.clear();
    float half = size / 2.0f;
    glm::vec3 lineColor(0.7f, 0.7f, 0.7f);

    // Generar líneas paralelas al eje X (variando Z)
    for (float z = -half; z <= half; z += step) {
        // Línea desde (-half, 0, z) hasta (half, 0, z)
        vertices.push_back({
            {-half, 0.0f, z},  // position
            {0.0f, 1.0f, 0.0f}, // normal (apunta hacia arriba)
            lineColor,          // color
            {0.0f, 0.0f},       // texCoord
            {1.0f, 0.0f, 0.0f}, // tangent
            {0.0f, 0.0f, 1.0f}  // bitangent
            });
        vertices.push_back({
            {half, 0.0f, z},
            {0.0f, 1.0f, 0.0f},
            lineColor,
            {1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
            });
    }

    // Generar líneas paralelas al eje Z (variando X)
    for (float x = -half; x <= half; x += step) {
        // Línea desde (x, 0, -half) hasta (x, 0, half)
        vertices.push_back({
            {x, 0.0f, -half},
            {0.0f, 1.0f, 0.0f},
            lineColor,
            {0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
            });
        vertices.push_back({
            {x, 0.0f, half},
            {0.0f, 1.0f, 0.0f},
            lineColor,
            {1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
            });
    }
}

void Malla::crearBuffers() {
    crearVertexBuffer();
}

void Malla::crearVertexBuffer() {
    VkDevice device = context->getDevice()->getDevice();
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Buffer temporal (staging)
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    context->getDevice()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    // Mapear y copiar datos
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    // Crear buffer en GPU
    context->getDevice()->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory
    );

    // Copiar datos
    context->getDevice()->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    // Limpiar staging
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Malla::destruirBuffers() {
    VkDevice device = context->getDevice()->getDevice();
    if (vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vertexBuffer = VK_NULL_HANDLE;
    }
    if (vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, vertexBufferMemory, nullptr);
        vertexBufferMemory = VK_NULL_HANDLE;
    }
}

void Malla::dibujar(VkCommandBuffer commandBuffer) {
    if (vertexBuffer == VK_NULL_HANDLE) return;

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}