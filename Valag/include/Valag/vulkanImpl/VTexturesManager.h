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

    std::list<size_t>   availableLayers;
};

class VTexturesManager : public Singleton<VTexturesManager>
{
    public:
        friend class Singleton<VTexturesManager>;
        friend class VApp;

        //static bool bindTexture(AssetTypeID id, size_t frameIndex, int *texArrayID);

        VkDescriptorSetLayout   getDescriptorSetLayout();
        VkDescriptorSet         getDescriptorSet(size_t frameIndex);
        size_t                  getDescriptorSetVersion(size_t frameIndex);

        void checkUpdateDescriptorSets(size_t frameIndex);

        static bool allocTexture(uint32_t width, uint32_t height, VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);

        static void freeTexture(VTexture &texture);

    protected:
        VTexturesManager();
        virtual ~VTexturesManager();

        //bool bindTextureImpl(AssetTypeID id, size_t frameIndex, int *texArrayID);
        bool allocTextureImpl(uint32_t width, uint32_t height, VBuffer source, CommandPoolName cmdPoolName, VTexture *texture);
        bool createTextureArray(uint32_t width, uint32_t height);

        void freeTextureImpl(VTexture &texture);

        bool    createDescriptorSetLayouts();
        bool    createSampler();
        bool    createDescriptorPool();
        bool    createDescriptorSets();
        bool    createDummyTexture();

        void    updateDescriptorSet(size_t frameIndex);

        bool    init();
        void    cleanup();

    private:
       // std::map<VkExtent2D, VTexture2DArray> m_allocatedTextureArrays;
        std::multimap<std::pair<uint32_t,uint32_t>, size_t> m_extentToArray;
        std::vector<VTexture2DArray> m_allocatedTextureArrays;
        std::vector<VkDescriptorImageInfo>  m_imageInfos;


        VTexture m_dummyTexture;

        //std::map<AssetTypeID, size_t> m_texturesArray;

        //std::list<size_t>                   m_availableImageInfos;
        //std::vector<VkDescriptorImageInfo>  m_imageInfos;

        //std::map<AssetTypeID, size_t> m_texturesToAdd;

        VkSampler                       m_sampler;
        std::vector<bool>               m_needToUpdateDescSet;
        std::vector<size_t>             m_descSetVersion;
        std::vector<VkDescriptorSet>    m_descriptorSets;
        VkDescriptorSetLayout           m_descriptorSetLayout;
        VkDescriptorPool                m_descriptorPool;

    public:
        static const size_t TEXTURES_ARRAY_SIZE;
        static const size_t MAX_LAYER_PER_TEXTURE;
};

}

#endif // VTEXTURESMANAGER_H