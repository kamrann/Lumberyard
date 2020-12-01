
#include "TestProject2_precompiled.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "TestProject2SystemComponent.h"

#include <IGem.h>

namespace TestProject2
{
    class TestProject2Module
        : public CryHooksModule
    {
    public:
        AZ_RTTI(TestProject2Module, "{1660E40C-82AC-4E0D-9104-32C2B6BBD1E4}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(TestProject2Module, AZ::SystemAllocator, 0);

        TestProject2Module()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                TestProject2SystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<TestProject2SystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(TestProject2_b0d6a5a37ae9431a96ce2c2a3800d2dd, TestProject2::TestProject2Module)
