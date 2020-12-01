
#pragma once

#include <AzCore/EBus/EBus.h>

namespace TestFirstGem
{
    class TestFirstGemRequests
        : public AZ::ComponentBus
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        //////////////////////////////////////////////////////////////////////////

        // Put your public methods here
		virtual AZStd::string BarMeAFoo(int foo) = 0;
		virtual AZStd::string BarMeAnAutoFoo() = 0;
	};
    using TestFirstGemRequestBus = AZ::EBus<TestFirstGemRequests>;
} // namespace TestFirstGem
