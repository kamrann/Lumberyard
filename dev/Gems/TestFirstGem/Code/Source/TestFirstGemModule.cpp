
#include "TestFirstGem_precompiled.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>

#include "TestFirstGemGameComponent.h"

#include <IGem.h>

namespace TestFirstGem
{
    class TestFirstGemModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(TestFirstGemModule, "{2D748833-5EF2-4BE1-9533-EECB330C843A}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(TestFirstGemModule, AZ::SystemAllocator, 0);

        TestFirstGemModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                TestFirstGemGameComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                //azrtti_typeid<TestFirstGemSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(TestFirstGem_8d0f4a8c0de0473689393ded85cbfa62, TestFirstGem::TestFirstGemModule)
