#include "folders.h"

using namespace buffy;

namespace {
struct FunctionConsumer : public buffy::Consumer<MailFolder>
{
    std::function<void(MailFolder)> func;

    FunctionConsumer(std::function<void(MailFolder)> func) : func(func) {}
    void consume(MailFolder& folder) override
    {
        func(folder);
    }
};
};

Folders::Folders()
{
}

void Folders::find_folders(std::function<void (MailFolder)> func)
{
    FunctionConsumer consumer(func);
    for (auto loc: config.locations())
        MailFolder::enumerateFolders(loc, consumer);
}
