
#pragma once

#include <AzCore/Component/Component.h>
#include <LmbrCentral/Rendering/RenderNodeBus.h>

#include <smartptr.h>
#include <IStatObj.h>
#include <IEntityRenderState.h>

#include <TestFirstGem/TestFirstGemBus.h>

namespace TestFirstGem
{
    class TestFirstGemGameComponent
        : public AZ::Component
        , protected TestFirstGemRequestBus::Handler
		, public LmbrCentral::RenderNodeRequestBus::Handler
	{
    public:
		AZ_COMPONENT(TestFirstGemGameComponent, "{F5E95308-2FE9-4CBA-98FA-E10BA701EF87}");
			//"{04BA9E05-3814-4C03-A56B-A6435D652F52}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// TestFirstGemRequestBus interface implementation

		virtual AZStd::string BarMeAFoo(int foo) override;
		virtual AZStd::string BarMeAnAutoFoo() override;
		////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// RenderNodeRequestBus
		IRenderNode* GetRenderNode() override;
		float GetRenderNodeRequestBusOrder() const override;
		static const float s_renderNodeRequestBusOrder;
		//////////////////////////////////////////////////////////////////////////

	protected:
		bool InitMesh();

	protected:
		int m_autoFoo = 0;

		using MeshPtr = _smart_ptr<IStatObj>;
		MeshPtr m_mesh = nullptr;

		using RenderNodePtr = AZStd::unique_ptr<IRenderNode>;
		RenderNodePtr m_renderNode = nullptr;
	};
}
