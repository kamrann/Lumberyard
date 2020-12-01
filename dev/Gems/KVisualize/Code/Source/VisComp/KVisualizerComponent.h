
#pragma once

#include <AzCore/Component/Component.h>
#include <AzFramework/Asset/SimpleAsset.h>
#include <LmbrCentral/Rendering/RenderNodeBus.h>
#include <LmbrCentral/Rendering/MaterialAsset.h>

#include <smartptr.h>


struct IStatObj;
struct IRenderNode;

namespace KVisualize
{
    class KVisualizerComponent
        : public AZ::Component
        //, protected KVisualizerComponentRequestBus::Handler
		, public LmbrCentral::RenderNodeRequestBus::Handler
	{
    public:
		AZ_COMPONENT(KVisualizerComponent, "{71F738F8-52B1-48EF-A57E-0366960FB11E}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

	public:
		KVisualizerComponent();
		~KVisualizerComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// KVisualizerComponentRequestBus

		////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// RenderNodeRequestBus

		IRenderNode* GetRenderNode() override;
		float GetRenderNodeRequestBusOrder() const override;
		static const float s_renderNodeRequestBusOrder;
		//////////////////////////////////////////////////////////////////////////

	protected:
		void OnMaterialChanged();

	protected:
		// @TODO: How does this differ from AZ::Data::Asset< > ?
		AzFramework::SimpleAssetReference< LmbrCentral::MaterialAsset > m_material;

		using RenderNodePtr = AZStd::unique_ptr<IRenderNode>;
		RenderNodePtr m_renderNode = nullptr;
	};
}
