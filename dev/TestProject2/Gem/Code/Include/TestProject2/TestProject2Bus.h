
#pragma once

#include <AzCore/EBus/EBus.h>

namespace TestProject2
{
    class TestProject2Requests
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////

        // Put your public methods here
    };
    using TestProject2RequestBus = AZ::EBus<TestProject2Requests>;
} // namespace TestProject2
