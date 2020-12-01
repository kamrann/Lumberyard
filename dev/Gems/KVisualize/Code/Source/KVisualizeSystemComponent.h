
#pragma once

#include <AzCore/Component/Component.h>

#include <KVisualize/KVisualizeBus.h>

namespace KVisualize
{
    class KVisualizeSystemComponent
        : public AZ::Component
        , protected KVisualizeRequestBus::Handler
    {
    public:
        AZ_COMPONENT(KVisualizeSystemComponent, "{C4C75A9E-6219-42DB-8724-03060611E6B1}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // KVisualizeRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
