#pragma once
#include <vector>
#include <map>
#include <string>
#include "jimg.h"

using namespace std;
using namespace jimg;

struct ProcessorContext
{
    std::vector<image> input;
    std::vector<image> output;
};


class AssetHandler
{
public:
    virtual void Execute(ProcessorContext& ctx) = 0;
};

class ImageProcessor
{
public:
    std::vector<AssetHandler*> handler;

    ImageProcessor* AddHandler(AssetHandler*&& h)
    {
        handler.push_back(h);
        return this;
    }

    void Execute()
    {
        ProcessorContext p;
        for (auto item : handler)
        {
            item->Execute(p);
            p.input.swap(p.output);
            p.output.clear();
        }
    }
    ~ImageProcessor()
    {
        for (auto item : handler)
            delete item;
    }
};