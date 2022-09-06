#pragma once
#include "iassetp.h"

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

class PieceSequence : public AssetHandler
{
public:
    int row;
    int column;
    PieceSequence(int row, int column) : row(row), column(column) { }
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

class LoadImagesFromFolder : public AssetHandler
{
public:
    string spath;
    LoadImagesFromFolder(const string& path) : spath(path) {}
    virtual void Execute(ProcessorContext& ctx) override
    {
        using namespace std::filesystem;

        std::vector<string> filenames;
        for (auto& it : directory_iterator(path(spath)))
        {
            if (it.is_directory()) continue;
            auto ext = it.path().extension();
            if (ext == ".jpg" || ext == ".png" || ext == ".tga" || ext == "bmp")
            {
                filenames.push_back(it.path().string());
            }
        }
        std::sort(filenames.begin(), filenames.end(), seqimg_name_sort());

        for (auto& item : filenames)
        {
            ctx.output.push_back(load_img_from_file(item.c_str()));
        }
    }
};


class SaveImagesToFolder : public AssetHandler
{
public:
    string spath;
    string ext;
    string name_reg;

    SaveImagesToFolder(const string& spath, const string& ext, const string& name_reg)
        : spath(spath), ext(ext), name_reg(name_reg) { }

    virtual void Execute(ProcessorContext& ctx) override
    {
        for (size_t i = 0; i < ctx.input.size(); i++)
        {
            string filename = spath + "/" + to_string(i) + ext;
            save_img(filename.c_str(), ctx.input.at(i).operate());
            ctx.output.push_back(std::move(ctx.input.at(i)));
        }
    }
};
