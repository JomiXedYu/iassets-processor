#pragma once
#include <iassetp.h>

namespace iassetp::chnl
{
    class SplitChannel : public AssetHandler
    {
    public:
        virtual void Execute(ProcessorContext& ctx) override
        {
            auto& in = ctx.input.at(0);
            for (size_t i = 0; i < in.channel_num; i++)
            {
                ctx.output.push_back(create_img(1, in.height, in.height));
            }
        }
    };
}
