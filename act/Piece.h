#pragma once
#include <iassetp.h>

namespace iassetp::act
{
    class Piece : public AssetHandler
    {
    public:
        int row;
        int column;
        Piece(int row, int column) : row(row), column(column) { }
        virtual void Execute(ProcessorContext& ctx) override
        {
            auto& first_img = ctx.input.at(0);
            int32_t block_size = first_img.buf.byte_size();

            image out = create_img(first_img.channel_num, first_img.weight * column, first_img.height * row);

            int x = 0, y = 0;
            for (size_t i = 0; i < ctx.input.size(); i++)
            {
                out.operate().draw(ctx.input.at(i).operate(), x * first_img.weight, y * first_img.height);
                x++;
                if (x == column)
                {
                    y++;
                    x = 0;
                }
            }

            ctx.output.push_back(std::move(out));
        }
    };
}