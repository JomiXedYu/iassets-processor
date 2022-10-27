#pragma once
#include <iassetp.h>

namespace iassetp::io
{
    class LoadImage : public AssetHandler
    {
    public:
        string spath;
        LoadImage(const string& path) : spath(path) {}

        virtual void Execute(ProcessorContext& ctx) override
        {
            image img = load_img_from_file(spath.c_str());
            img.set_attr("fn", spath);
            ctx.output.push_back(std::move(img));
        }
    };

    class DeferredLoadImage : public AssetHandler
    {
    public:
        string spath;
        DeferredLoadImage(const string& path) : spath(path) {}

        virtual void Execute(ProcessorContext& ctx) override
        {
            image img;
            img.set_attr("fn", spath);
            ctx.output.push_back(std::move(img));
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
                image img = load_img_from_file(item.c_str());
                img.set_attr("fn", item);
                ctx.output.push_back(std::move(img));
            }
        }
    };

    class SaveImages : public AssetHandler
    {
    public:
        string spath;
        string out_attr;

        SaveImages(const string& attr = "output_fn") : out_attr(attr) {}
        SaveImages(const spath& path) : spath(path) {}

        virtual void Execute(ProcessorContext& ctx) override
        {
            for (size_t i = 0; i < ctx.input.size(); i++)
            {
                string filename;
                if (!this->out_attr.empty())
                {
                    if (ctx.input[i].get_attr<string>(this->out_attr, &filename))
                    {
                        throw;
                    }
                }
                else
                {
                    filename = std::move(this->spath);
                }
                save_img(filename.c_str(), ctx.input[i].operate());
                ctx.output.push_back(std::move(ctx.input[i]));
            }
        }
    };

    class SaveImagesToFolder : public AssetHandler
    {
    public:
        string spath;
        string ext;
        string name_reg;

        string out_attr;

        SaveImagesToFolder(const string& attr = "output_folder") : out_attr(attr) {}
        SaveImagesToFolder(const string& spath, const string& ext, const string& name_reg)
            : spath(spath), ext(ext), name_reg(name_reg) { }

        virtual void Execute(ProcessorContext& ctx) override
        {
            for (size_t i = 0; i < ctx.input.size(); i++)
            {
                string filename = spath + "/" + to_string(i) + ext;
                save_img(filename.c_str(), ctx.input[i].operate());
                ctx.output.push_back(std::move(ctx.input[i]));
            }
        }
    };
}