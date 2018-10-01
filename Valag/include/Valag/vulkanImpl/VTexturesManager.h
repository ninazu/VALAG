#ifndef VTEXTURESMANAGER_H
#define VTEXTURESMANAGER_H

#include "Valag/vulkanImpl/vulkanImpl.h"
#include "Valag/vulkanImpl/VTexture.h"

#include <map>
#include <list>

/** I could try to work with multiple arrays... and create new descriptor sets **/

namespace vlg
{

struct VTexture2DArray
{
    VkImage         image;
    VkDeviceMemory  memory;
    VkImageView     view;

    size_t      layerCount;
    VkExtent2D  extent;

    std::mutex  mutex;

    std::list<size_t>   availableLayers;
};

class VTexturesManager : public Singleton<VTexturesManager>
{
    public:
        friend class Singleton<VTexturesManager>;
        friend class VApp;


        void checkUpdateDescriptorSets(size_t frameIndex, size_t imageIndex);

        static bool allocTexture(uint32_t width, uint32_t height, VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);
        static void freeTexture(VTexture &texture);


        static VkSampler                sampler();
        static VkDescriptorSetLayout    descriptorSetLayout();
        static VkDescriptorSet          descriptorSet(size_t frameIndex);
        static VkDescriptorSet          imgDescriptorSet(size_t imageIndex);
        static size_t                   descriptorSetVersion(size_t frameIndex);
        static size_t                   imgDescriptorSetVersion(size_t imageIndex);

    protected:
        VTexturesManager();
        virtual ~VTexturesManager();

        bool allocTextureImpl(uint32_t width, uint32_t height, VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);
        size_t createTextureArray(uint32_t width, uint32_t height);

        void freeTextureImpl(VTexture &texture);

        bool    createDescriptorSetLayouts();
        bool    createSampler();
        bool    createDescriptorPool(size_t framesCount, size_t imagesCount);
        bool    createDescriptorSets(size_t framesCount, size_t imagesCount);
        bool    createDummyTexture();

        void    updateDescriptorSet(size_t frameIndex);
        void    updateImgDescriptorSet(size_t imageIndex);
        void    writeDescriptorSet(VkDescriptorSet &descSet);

        bool    init(size_t framesCount, size_t imagesCount);
        void    cleanup();

        VkDescriptorSetLayout   getDescriptorSetLayout();
        VkDescriptorSet         getDescriptorSet(size_t frameIndex);
        VkDescriptorSet         getImgDescriptorSet(size_t imageIndex);
        size_t                  getDescriptorSetVersion(size_t frameIndex);
        size_t                  getImgDescriptorSetVersion(size_t imageIndex);

    private:
        std::multimap<std::pair<uint32_t,uint32_t>, size_t> m_extentToArray;
        std::vector<VTexture2DArray*>       m_allocatedTextureArrays;
        std::vector<VkDescriptorImageInfo>  m_imageInfos;


        VTexture m_dummyTexture;

        std::mutex m_createImageMutex;


        VkSampler                       m_sampler;
        std::vector<bool>               m_needToUpdateDescSet;
        std::vector<bool>               m_needToUpdateImgDescSet;
        std::vector<size_t>             m_descSetVersion;
        std::vector<size_t>             m_imgDescSetVersion;
        std::vector<VkDescriptorSet>    m_descriptorSets;
        std::vector<VkDescriptorSet>    m_imgDescriptorSets;
        VkDescriptorSetLayout           m_descriptorSetLayout;
        VkDescriptorPool                m_descriptorPool;

    public:
        static const size_t MAX_TEXTURES_ARRAY_SIZE;
        static const size_t MAX_LAYER_PER_TEXTURE;
};

}

#endif // VTEXTURESMANAGER_H
