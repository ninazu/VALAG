#include "Valag/renderers/InstancingRenderer.h"

#include "Valag/vulkanImpl/vulkanImpl.h"
#include "Valag/utils/Logger.h"
#include "Valag/core/VApp.h"

#include "Valag/assets/AssetHandler.h"
#include "Valag/assets/TextureAsset.h"

#include "Valag/gfx/Sprite.h"
#include "Valag/gfx/SpritesBatch.h"

namespace vlg
{

const char *InstancingRenderer::INSTANCING_VERTSHADERFILE = "instancingShader.vert.spv";
const char *InstancingRenderer::INSTANCING_FRAGSHADERFILE = "instancingShader.frag.spv";

const float InstancingRenderer::DEPTH_SCALING_FACTOR = 1024*1024;


VkVertexInputBindingDescription InstanciedSpriteDatum::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(InstanciedSpriteDatum);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 8> InstanciedSpriteDatum::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 8> attributeDescriptions = {};

    size_t i = 0;
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, model_0);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, model_1);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, model_2);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, model_3);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, color);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, texPos);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, texExtent);
    ++i;

    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = i;
    attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[i].offset = offsetof(InstanciedSpriteDatum, texId);
    ++i;


    return attributeDescriptions;
}

InstancingRenderer::InstancingRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order) : AbstractRenderer(targetWindow, name,order)
{
    this->init();
}

InstancingRenderer::~InstancingRenderer()
{
    this->cleanup();
}

void InstancingRenderer::update(size_t frameIndex)
{
    AbstractRenderer::update(frameIndex);
}


void InstancingRenderer::draw(Sprite* sprite)
{
    SpriteModelUBO modelUBO = sprite->getModelUBO();

    TextureAsset *texAsset = TexturesHandler::instance()->getAsset(sprite->getTexture());

    if(texAsset != nullptr && texAsset->isLoaded())
    {
        VTexture vtexture = texAsset->getVTexture();

       InstanciedSpriteDatum datum = {};
       datum.model_0 = modelUBO.model[0];
       datum.model_1 = modelUBO.model[1];
       datum.model_2 = modelUBO.model[2];
       datum.model_3 = modelUBO.model[3];
       datum.color   = modelUBO.color;
       datum.texExtent = modelUBO.texExt;
       datum.texPos = modelUBO.texPos;
       datum.texId = {vtexture.m_textureId, vtexture.m_textureLayer};

       m_spritesVbos[m_curFrameIndex].push_back(datum);
    }
}


void InstancingRenderer::draw(SpritesBatch* spritesBatch)
{
    spritesBatch->draw(this);
}

bool InstancingRenderer::recordPrimaryCommandBuffer(uint32_t imageIndex)
{
    size_t spritesVertexBufferSize = m_spritesVbos[m_curFrameIndex].uploadVBO();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ;

    if (vkBeginCommandBuffer(m_primaryCMB[m_curFrameIndex], &beginInfo) != VK_SUCCESS)
    {
        Logger::error("Failed to begin recording command buffer");
        return (false);
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_targetWindow->getSwapchainExtent();

    std::array<VkClearValue, 2> clearValues = {};
    if(m_order == Renderer_First || m_order == Renderer_Unique)
    {
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {0.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
    }

    vkCmdBeginRenderPass(m_primaryCMB[m_curFrameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE /*VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS*/);

    if(spritesVertexBufferSize != 0)
    {
        //vkCmdBindPipeline(m_primaryCMB[m_curFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        m_pipeline.bind(m_primaryCMB[m_curFrameIndex]);

        VkDescriptorSet descriptorSets[] = {m_renderView.getDescriptorSet(m_curFrameIndex),
                                            VTexturesManager::instance()->getDescriptorSet(m_curFrameIndex) };

        vkCmdBindDescriptorSets(m_primaryCMB[m_curFrameIndex],VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline.getLayout(),0,2, descriptorSets, 0, nullptr);

        VBuffer vertexBuffer = m_spritesVbos[m_curFrameIndex].getBuffer();
        vkCmdBindVertexBuffers(m_primaryCMB[m_curFrameIndex], 0, 1, &vertexBuffer.buffer, &vertexBuffer.offset);

        vkCmdDraw(m_primaryCMB[m_curFrameIndex], 4, spritesVertexBufferSize, 0, 0);
    }

    vkCmdEndRenderPass(m_primaryCMB[m_curFrameIndex]);

    if (vkEndCommandBuffer(m_primaryCMB[m_curFrameIndex]) != VK_SUCCESS)
    {
        Logger::error("Failed to record primary command buffer");
        return (false);
    }

    return (true);
}

bool InstancingRenderer::init()
{
    m_renderView.setExtent({m_targetWindow->getSwapchainExtent().width,
                            m_targetWindow->getSwapchainExtent().height});
    m_renderView.setDepthFactor(DEPTH_SCALING_FACTOR);

    m_spritesVbos = std::vector<DynamicVBO<InstanciedSpriteDatum> >(m_targetWindow->getFramesCount(), DynamicVBO<InstanciedSpriteDatum>(1024));

    return AbstractRenderer::init();
}

bool InstancingRenderer::createDescriptorSetLayouts()
{
    return (true);
}

bool InstancingRenderer::createGraphicsPipeline()
{
    std::ostringstream vertShaderPath,fragShaderPath;
    vertShaderPath << VApp::DEFAULT_SHADERPATH << INSTANCING_VERTSHADERFILE;
    fragShaderPath << VApp::DEFAULT_SHADERPATH << INSTANCING_FRAGSHADERFILE;

    m_pipeline.createShader(vertShaderPath.str(), VK_SHADER_STAGE_VERTEX_BIT);
    m_pipeline.createShader(fragShaderPath.str(), VK_SHADER_STAGE_FRAGMENT_BIT);

    auto bindingDescription = InstanciedSpriteDatum::getBindingDescription();
    auto attributeDescriptions = InstanciedSpriteDatum::getAttributeDescriptions();
    m_pipeline.setVertexInput(1, &bindingDescription,
                              attributeDescriptions.size(), attributeDescriptions.data());

    m_pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, true);

    m_pipeline.setDefaultExtent(m_targetWindow->getSwapchainExtent());
    m_pipeline.setBlendMode(BlendMode_Alpha);

    m_pipeline.attachDescriptorSetLayout(m_renderView.getDescriptorSetLayout());
    m_pipeline.attachDescriptorSetLayout(VTexturesManager::instance()->getDescriptorSetLayout());

    m_pipeline.setDepthTest(true, true, VK_COMPARE_OP_GREATER);

    return m_pipeline.init(m_renderPass, 0);
}

bool InstancingRenderer::createDescriptorPool()
{
    return (true);
}

bool InstancingRenderer::createDescriptorSets()
{
    return (true);
}

bool InstancingRenderer::createUBO()
{
    return (true);
}

void InstancingRenderer::cleanup()
{
    m_spritesVbos.clear();
    m_pipeline.destroy();
    AbstractRenderer::cleanup();
}

}