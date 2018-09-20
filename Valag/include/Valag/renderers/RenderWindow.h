#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Vulkan/vulkan.h>

#include <string>
#include <vector>
#include <map>

#include "Valag/Types.h"
#include "Valag/vulkanImpl/vulkanImpl.h"

namespace vlg
{

class AbstractRenderer;

class RenderWindow
{
    friend class VApp;

    public:
        RenderWindow();
        virtual ~RenderWindow();

        bool create(size_t w, size_t h, const std::string &name, bool fullscreen = false, size_t framesCount = 1);
        bool init(); //Can only be done after initializing Vulkan
        void destroy();

        bool attachRenderer(AbstractRenderer *renderer);
        bool detachRenderer(RendererName renderer);

        size_t      getFramesCount();
        size_t      getSwapchainSize();
        size_t      getCurrentFrameIndex();
        VkExtent2D  getSwapchainExtent();
        VkFormat    getSwapchainImageFormat();
        const std::vector<VFramebufferAttachment> &getSwapchainAttachments();
        const std::vector<VFramebufferAttachment> &getSwapchainDepthAttachments();
        /*const std::vector<VkImageView> &getSwapchainImageViews();
        const std::vector<VkImageView> &getDepthStencilImageViews();*/

        AbstractRenderer* getRenderer(RendererName renderer);

    protected:
        uint32_t    acquireNextImage();
    // void        submitToGraphicsQueue(VkCommandBuffer commandBuffer, VkSemaphore finishedRenderingSemaphore);
        void        submitToGraphicsQueue(std::vector<VkCommandBuffer> &commandBuffers, std::vector<VkSemaphore> &waitSemaphores);
        void        display();

        bool    checkVideoMode(size_t w, size_t h, GLFWmonitor *monitor);
        bool    createGLFWindow(size_t w, size_t h, const std::string &name, bool fullscreen);
        bool    createSurface();

        VkSurfaceFormatKHR  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR    chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D          chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
        bool                createSwapchain();
        //bool                createDepthStencil();
        //bool                createImageViews();
        bool                createSemaphoresAndFences();

        VkSurfaceKHR &getSurface();
        GLFWwindow  *getWindowPtr();


    private:
        GLFWwindow         *m_window;
        VkSurfaceKHR        m_surface;

        VkSwapchainKHR                       m_swapchain;
        std::vector<VFramebufferAttachment>  m_swapchainAttachments;
        /*std::vector<VkImage>        m_swapchainImages;
        std::vector<VkImageView>    m_swapchainImageViews;*/
        /*VkFormat    m_swapchainImageFormat;
        VkExtent2D  m_swapchainExtent;*/

        ///Maybe I should turn this into FramebufferAttachment
        std::vector<VFramebufferAttachment> m_depthStencilAttachments;
        /*std::vector<VImage>         m_depthStencilImages;
        std::vector<VkImageView>    m_depthStencilImagesViews;*/


        size_t      m_framesCount;
        uint32_t    m_curImageIndex;
        size_t      m_curFrameIndex;
        std::vector<VkSemaphore>                 m_imageAvailableSemaphore;
        //std::vector<std::vector<VkSemaphore>>    m_finishedRenderingSemaphores;
        std::vector<VkSemaphore>                 m_finishedRenderingSemaphores;
        std::vector<VkFence>                     m_inFlightFences;

        std::map<RendererName, AbstractRenderer*>  m_attachedRenderers;
};

}

#endif // RENDERWINDOW_H
