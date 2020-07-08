#include "buffy/utils/tests.h"
#include "buffy/utils/sys.h"
#include "buffy/utils/string.h"
#include "buffy/mailfolder/maildir.h"

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
    auto test = buffy::mailfolder::Maildir::accessFolder(TEST_DATA_DIR "/maildir/empty");
    wassert(actual(test->name()) == "empty");
    wassert(actual(test->path()) == TEST_DATA_DIR "/maildir/empty");
    wassert(actual(test->type()) == "Maildir");

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
    buffy::mailfolder::Maildir::enumerateFolders(TEST_DATA_DIR "/maildir/empty", [&](auto) { ++count; });
    wassert(actual(count) == 1u);
});

add_method("non_empty", []() {
    auto test = buffy::mailfolder::Maildir::accessFolder(TEST_DATA_DIR "/maildir/test");
    wassert(actual(test->name()) == "test");
    wassert(actual(test->path()) == TEST_DATA_DIR "/maildir/test");
    wassert(actual(test->type()) == "Maildir");

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
    buffy::mailfolder::Maildir::enumerateFolders(TEST_DATA_DIR "/maildir/test", [&](auto) { ++count; });
    wassert(actual(count) == 1u);
});

add_method("is_broken_symlink", []() {
    // Check a maildir whose root is a broken symlink
    sys::Tempdir workdir;
    workdir.symlinkat("does-not-exist", "broken");
    std::string testfile = str::joinpath(workdir.name(), "broken");

    auto test = buffy::mailfolder::Maildir::accessFolder(testfile);
    wassert(actual((bool)test) == false);

    unsigned count = 0;
    buffy::mailfolder::Maildir::enumerateFolders(testfile, [&](auto) { ++count; });
    wassert(actual(count) == 0u);
});

add_method("contains_broken_symlink", []() {
    // Check a maildir whose cur, new and tmp directories are broken symlinks
    sys::Tempdir workdir;
    workdir.symlinkat("does-not-exist", "cur");
    workdir.symlinkat("does-not-exist", "new");
    workdir.symlinkat("does-not-exist", "tmp");

    auto test = buffy::mailfolder::Maildir::accessFolder(workdir.name());
    wassert(actual((bool)test) == false);

    unsigned count = 0;
    buffy::mailfolder::Maildir::enumerateFolders(workdir.name(), [&](auto) { ++count; });
    wassert(actual(count) == 0u);
});

add_method("is_loop_symlink", []() {
    sys::Tempdir workdir;
    workdir.symlinkat("loop", "loop");
    std::string testfile = str::joinpath(workdir.name(), "loop");

    auto test = buffy::mailfolder::Maildir::accessFolder(testfile);
    wassert(actual((bool)test) == false);

    unsigned count = 0;
    buffy::mailfolder::Maildir::enumerateFolders(testfile, [&](auto) { ++count; });
    wassert(actual(count) == 0u);
});

add_method("contains_loop_symlink", []() {
    // Check a maildir which has a submaildir that is a symlink pointing up
    sys::Tempdir workdir;
    workdir.mkdirat("cur");
    workdir.mkdirat("new");
    workdir.mkdirat("tmp");
    workdir.symlinkat(workdir.name().c_str(), "loop");

    auto test = buffy::mailfolder::Maildir::accessFolder(workdir.name());
    wassert(actual((bool)test) == true);

    unsigned count = 0;
    buffy::mailfolder::Maildir::enumerateFolders(workdir.name(), [&](auto) { ++count; });
    wassert(actual(count) == 1u);
});

add_method("enumerate", []() {
    unsigned count = 0;
    buffy::mailfolder::Maildir::enumerateFolders(TEST_DATA_DIR "/maildir", [&](auto) { ++count; });
    wassert(actual(count) == 4u);
});

}

}
