#pragma once
#include <iassetp.h>
#include <any>

namespace iassetp::attr
{
    class Attr : public AssetHandler
    {
    public:
        string name;
        std::any value;

        Attr(const string& name, const std::any& value) : name(name), value(value) {}

        virtual void Execute(ProcessorContext& ctx) override
        {
            ctx.input.swap(ctx.output);
            for (auto& item : ctx.output)
            {
                item.set_attr(this->name, this->value);
            }
        }
    };
}