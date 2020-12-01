
#include "TestProject2_precompiled.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "TestProject2SystemComponent.h"

namespace TestProject2
{
    void TestProject2SystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<TestProject2SystemComponent, AZ::Component>()
                ->Version(0)
                ->SerializerForEmptyClass();

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<TestProject2SystemComponent>("TestProject2", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void TestProject2SystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("TestProject2Service"));
    }

    void TestProject2SystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("TestProject2Service"));
    }

    void TestProject2SystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void TestProject2SystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    void TestProject2SystemComponent::Init()
    {
    }

    void TestProject2SystemComponent::Activate()
    {
        TestProject2RequestBus::Handler::BusConnect();
    }

    void TestProject2SystemComponent::Deactivate()
    {
        TestProject2RequestBus::Handler::BusDisconnect();
    }
}
