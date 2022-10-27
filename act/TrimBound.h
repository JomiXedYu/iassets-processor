#pragma once
#include <iassetp.h>

namespace iassetp::act
{
    class TrimBound : public AssetHandler
    {
    public:
        TrimBound()
        {}

        virtual void Execute(ProcessorContext& ctx) override
        {
            for (auto& img : ctx.input)
            {
                if (img.channel_num < 4)
                {
                    ctx.output.push_back(img);
                    continue;
                }

                auto io = img.operate();

                int bleft = -1, bright = -1;
                int btop = -1, bbottom = -1;

                for (size_t y = 0; y < img.height; y++)
                {
                    for (size_t x = 0; x < img.weight; x++)
                    {
                        auto c = io.at_rgba(x, y);
                        if (c.a != 0)
                        {
                            if (bleft == -1 || x < bleft)
                            {
                                bleft = x;
                            }
                            if (bright == -1 || x > bright)
                            {
                                bright = x;
                            }
                            if (btop == -1 || y < btop)
                            {
                                btop = y;
                            }
                            if (bbottom == -1 || y > bbottom)
                            {
                                bbottom = y;
                            }
                        }
                    }
                }
                int new_weight = bright + 1 - bleft;
                int new_height = bbottom + 1 - btop;
                image out{ img.channel_num, new_weight, new_height, img.attr };
                out.operate().draw(io, 0, 0, bleft, btop);
                ctx.output.push_back(std::move(out));
            }
        }
    };
}