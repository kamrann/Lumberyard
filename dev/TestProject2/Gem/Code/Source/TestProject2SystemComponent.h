
#pragma once

#include <AzCore/Component/Component.h>

#include <TestProject2/TestProject2Bus.h>

namespace TestProject2
{
    class TestProject2SystemComponent
        : public AZ::Component
        , protected TestProject2RequestBus::Handler
    {
    public:
        AZ_COMPONENT(TestProject2SystemComponent, "{2405C9B0-517B-4EDB-AA2E-E6623D4694CA}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // TestProject2RequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
