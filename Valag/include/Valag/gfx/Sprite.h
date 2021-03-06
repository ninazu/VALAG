#ifndef SPRITE_H
#define SPRITE_H

#include "Valag/vulkanImpl/VulkanImpl.h"
#include "Valag/gfx/Drawable.h"
#include "Valag/core/NotificationSender.h"

#include "Valag/vulkanImpl/DynamicUBODescriptor.h"

namespace vlg
{
/** For animated sprite, I should work with array of vertexBuffer, having all their texCoord already saved **/

/** I need to find a way to clear model UBO of sprites... maybe I should keep in memory the list of renderer used ?  **/
/** I should have dynamic UBO as static member probably **/


struct SpriteModelUBO {
    glm::mat4 model;
    glm::vec4 color;
    glm::vec2 texPos;
    glm::vec2 texExt;
};

class Sprite : public NotificationSender, public Drawable
{
    friend class DefaultRenderer;
    friend class SpritesBatch;
    friend class Drawable;

    public:
        Sprite();
        virtual ~Sprite();

        void setSize(glm::vec2 size);
        void setPosition(glm::vec2 position);
        void setPosition(glm::vec3 position);
        void setColor(Color color);
        void setTexture(AssetTypeId textureId);
        void setTextureRect(glm::vec2 pos, glm::vec2 extent);

        AssetTypeId getTexture();
        SpriteModelUBO getModelUBO();

        ///Specifying framebuffer may induce better performances
        VkCommandBuffer getDrawCommandBuffer(DefaultRenderer *renderer, size_t frameIndex, /*VkPipeline pipeline,*/ VkRenderPass renderPass,
                                                uint32_t subpass, VkFramebuffer framebuffer = VK_NULL_HANDLE);

        static VkDescriptorSetLayout getModelDescriptorSetLayout();

    protected:
        void updateModelUBO();
        void uploadModelUBO(size_t frameIndex);

        void cleanup();

        bool checkUpdates(size_t frameIndex);

        bool recordDrawCommandBuffers(DefaultRenderer *renderer, size_t frameIndex, VkRenderPass renderPass,
                                                uint32_t subpass, VkFramebuffer framebuffer = VK_NULL_HANDLE);


        bool recordDrawCmbContent(VkCommandBuffer &commandBuffer, DefaultRenderer *renderer, size_t frameIndex, VkRenderPass renderPass,
                                                uint32_t subpass, VkFramebuffer framebuffer);

    private:
        glm::vec2 m_size;
        glm::vec3 m_position;
        glm::vec4 m_color;

        SpriteModelUBO m_modelUBO;

        /// I could (should ?) switch to pointer to textureAsset for more efficiency...
        AssetTypeId m_texture;
        std::vector<bool>   m_needToCheckLoading;

        glm::vec2 m_texturePosition;
        glm::vec2 m_textureExtent;


        std::vector<bool>       m_needToAllocModel;
        std::vector<bool>       m_needToUploadModelUBO;
        std::vector<size_t>     m_modelUBOIndex;
        std::vector<size_t>     m_modelBufferVersion;
        std::vector<size_t>     m_texDescSetVersion;
        bool                    m_preventDrawing;


    /** Static **/
        static bool initRendering(size_t framesCount);
        static void updateRendering(size_t frameIndex); //Expands UBOs
        static void cleanupRendering();

        static size_t s_framesCount;
        static DynamicUBODescriptor s_modelUBO;
};

}

#endif // SPRITE_H
