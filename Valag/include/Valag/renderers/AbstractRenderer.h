#ifndef ABSTRACTRENDERER_H
#define ABSTRACTRENDERER_H

#include "Valag/renderers/RenderWindow.h"
#include "Valag/renderers/RenderView.h"
#include "Valag/renderers/RenderGraph.h"

namespace vlg
{

class AbstractRenderer
{
    public:
        AbstractRenderer(RenderWindow *targetWindow, RendererName name, RenderereOrder order);
        virtual ~AbstractRenderer();

        virtual void update(size_t frameIndex);
        virtual void render(uint32_t imageIndex);

        virtual void setView(ViewInfo viewInfo);

        virtual std::vector<FullRenderPass*> getFinalPasses();

        size_t getFramesCount();

        RendererName    getName();

    protected:
        virtual void    prepareRenderPass();

        virtual bool    initRenderGraph();
        virtual bool    createGraphicsPipeline() = 0;
        virtual bool    createRenderView();

        /*virtual bool    createDescriptorSetLayouts();
        virtual bool    createDescriptorPool();
        virtual bool    createDescriptorSets();*/

        virtual bool    init();
        virtual void    cleanup();

        virtual bool    recordPrimaryCmb(uint32_t imageIndex) = 0;


    protected:
        //bool m_useDynamicView;

        RenderWindow   *m_targetWindow;
        RenderView      m_renderView;
        RenderGraph     m_renderGraph;
        size_t          m_defaultPass;

        /*std::vector<VkDescriptorPoolSize>   m_descriptorPoolSizes;
        VkDescriptorPool                    m_descriptorPool;*/

        //I should remove it and use targetWindow->getFrameIndex();
        size_t          m_curFrameIndex;

    private:
        RenderereOrder  m_order;
        RendererName    m_name;

        std::vector<FullRenderPass*> m_finalPasses;
};

}

#endif // ABSTRACTRENDERER_H
