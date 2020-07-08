#include "buffy/utils/tests.h"
#include "buffy/utils/sys.h"
#include "buffy/utils/string.h"
#include "buffy/mailfolder/mailbox.h"

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

using namespace buffy::utils::tests;
using namespace buffy::utils;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override;
};

Tests test("mailfolder_mailbox");

void Tests::register_tests() {

add_method("empty", []() {
    auto test = buffy::mailfolder::Mailbox::accessFolder(TEST_DATA_DIR "/mbox/empty.mbox");
    wassert(actual((bool)test) == true);
    wassert(actual(test->name()) == "empty.mbox");
    wassert(actual(test->path()) == TEST_DATA_DIR "/mbox/empty.mbox");
    wassert(actual(test->type()) == "Mailbox");

    wassert(actual(test->getMsgTotal()) == -1);
    wassert(actual(test->getMsgUnread()) == -1);
    wassert(actual(test->getMsgNew()) == -1);
    wassert(actual(test->getMsgFlagged()) == -1);
    wassert(actual(test->changed()) == true);

    test->updateStatistics();
    wassert(actual(test->getMsgTotal()) == 0);
    wassert(actual(test->getMsgUnread()) == 0);
    wassert(actual(test->getMsgNew()) == 0);
    wassert(actual(test->getMsgFlagged()) == 0);
    wassert(actual(test->changed()) == false);

    unsigned count = 0;
    buffy::mailfolder::Mailbox::enumerateFolders(TEST_DATA_DIR "/mbox/empty.mbox", [&](auto) { ++count; });
    wassert(actual(count) == 1u);
});

add_method("non_empty", []() {
    auto test = buffy::mailfolder::Mailbox::accessFolder(TEST_DATA_DIR "/mbox/test.mbox");
    wassert(actual((bool)test) == true);
    wassert(actual(test->name()) == "test.mbox");
    wassert(actual(test->path()) == TEST_DATA_DIR "/mbox/test.mbox");
    wassert(actual(test->type()) == "Mailbox");

    wassert(actual(test->getMsgTotal()) == -1);
    wassert(actual(test->getMsgUnread()) == -1);
    wassert(actual(test->getMsgNew()) == -1);
    wassert(actual(test->getMsgFlagged()) == -1);
    wassert(actual(test->changed()) == true);

    test->updateStatistics();
    wassert(actual(test->getMsgTotal()) == 3);
    wassert(actual(test->getMsgUnread()) == 0);
    wassert(actual(test->getMsgNew()) == 0);
    wassert(actual(test->getMsgFlagged()) == 1);
    wassert(actual(test->changed()) == false);

    unsigned count = 0;
    buffy::mailfolder::Mailbox::enumerateFolders(TEST_DATA_DIR "/mbox/test.mbox", [&](auto) { ++count; });
    wassert(actual(count) == 1u);
});

add_method("broken_symlink", []() {
    // Check a mailbox whose root is a broken symlink
    sys::Tempdir workdir;
    workdir.symlinkat("does-not-exist", "broken.mbox");
    std::string testfile = str::joinpath(workdir.name(), "broken.mbox");

    auto test = buffy::mailfolder::Mailbox::accessFolder(testfile);
    wassert_false((bool)test);

    unsigned count = 0;
    buffy::mailfolder::Mailbox::enumerateFolders(testfile, [&](auto) { ++count; });
    wassert(actual(count) == 0u);
});

add_method("enumerate", []() {
    // Check enumeration of a directory with mailboxes
    unsigned count = 0;
    buffy::mailfolder::Mailbox::enumerateFolders(TEST_DATA_DIR "/mbox", [&](auto) { ++count; });
    wassert(actual(count) == 2u);
});

}

}
