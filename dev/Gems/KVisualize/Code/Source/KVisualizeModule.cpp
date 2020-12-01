
#include "KVisualize_precompiled.h"
#include <platform_impl.h>

#include <AzCore/Memory/SystemAllocator.h>
#include <sstream>

#include "KVisualizeSystemComponent.h"
#include "VisComp/KVisualizerComponent.h"

#include <IGem.h>

namespace KVisualize
{
    class KVisualizeModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(KVisualizeModule, "{282ECB02-6CFF-4400-BDB6-05345201BD79}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(KVisualizeModule, AZ::SystemAllocator, 0);

        KVisualizeModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                KVisualizeSystemComponent::CreateDescriptor(),
				KVisualizerComponent::CreateDescriptor(),
			});
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<KVisualizeSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(KVisualize_e74ab1f145ac43d99fab306ffd9b47da, KVisualize::KVisualizeModule)
