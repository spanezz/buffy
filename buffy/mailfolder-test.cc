#include "buffy/utils/tests.h"
#include "buffy/utils/sys.h"
#include "buffy/utils/string.h"
#include "buffy/mailfolder.h"
#include "buffy/mailfolder/maildir.h"
#include "buffy/mailfolder/mailbox.h"
#include <sys/stat.h>  // mkdir
#include <sys/types.h>
#include <cstdio>
#include <unistd.h>

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

Tests test("mailfolder");

void Tests::register_tests() {

add_method("mbox", []() {
    std::vector<std::string> dirs;
    dirs.push_back(TEST_DATA_DIR "/mbox");
    dirs.push_back(TEST_DATA_DIR "/mbox/");
    for (const auto& i: dirs)
    {
        auto folders = buffy::MailFolder::enumerateFolders(i);

        wassert(actual(folders.size()) == 2u);

        std::shared_ptr<buffy::MailFolder> empty = folders[0]->name() == "empty.mbox" ? folders[0] : folders[1];
        std::shared_ptr<buffy::MailFolder> test = folders[0]->name() == "empty.mbox" ? folders[1] : folders[0];

        wassert(actual(empty->path()) == TEST_DATA_DIR "/mbox/empty.mbox");
        wassert(actual(empty->name()) == "empty.mbox");
        wassert(actual(empty->type()) == "Mailbox");
        wassert(actual(test->path()) == TEST_DATA_DIR "/mbox/test.mbox");
        wassert(actual(test->name()) == "test.mbox");
        wassert(actual(test->type()) == "Mailbox");

        empty->updateStatistics();

        wassert(actual(empty->getMsgTotal()) == 0);
        wassert(actual(empty->getMsgUnread()) == 0);
        wassert(actual(empty->getMsgNew()) == 0);
        wassert(actual(empty->getMsgFlagged()) == 0);
        wassert(actual(empty->changed()) == false);

        test->updateStatistics();

        wassert(actual(test->getMsgTotal()) == 3);
        wassert(actual(test->getMsgUnread()) == 0);
        wassert(actual(test->getMsgNew()) == 0);
        wassert(actual(test->getMsgFlagged()) == 1);
        wassert(actual(test->changed()) == false);
    }
});

add_method("mboxgz", [] {
    std::vector<std::string> dirs;
    dirs.push_back(TEST_DATA_DIR "/mboxgz");
    dirs.push_back(TEST_DATA_DIR "/mboxgz/");
    for (const auto& i: dirs)
    {
        // An empty database should return empty sets, but not fail
        auto folders = buffy::MailFolder::enumerateFolders(i);

        wassert(actual(folders.size()) == 2u);

        std::shared_ptr<buffy::MailFolder> empty = folders[0]->name() == "empty.mbox.gz" ? folders[0] : folders[1];
        std::shared_ptr<buffy::MailFolder> test = folders[0]->name() == "empty.mbox.gz" ? folders[1] : folders[0];

        wassert(actual(empty->path()) == TEST_DATA_DIR "/mboxgz/empty.mbox.gz");
        wassert(actual(empty->name()) == "empty.mbox.gz");
        wassert(actual(empty->type()) == "Mailbox");
        wassert(actual(test->path()) == TEST_DATA_DIR "/mboxgz/test.mbox.gz");
        wassert(actual(test->name()) == "test.mbox.gz");
        wassert(actual(test->type()) == "Mailbox");

        empty->updateStatistics();

        wassert(actual(empty->getMsgTotal()) == 0);
        wassert(actual(empty->getMsgUnread()) == 0);
        wassert(actual(empty->getMsgNew()) == 0);
        wassert(actual(empty->getMsgFlagged()) == 0);
        wassert(actual(empty->changed()) == false);

        test->updateStatistics();

        wassert(actual(test->getMsgTotal()) == 3);
        wassert(actual(test->getMsgUnread()) == 0);
        wassert(actual(test->getMsgNew()) == 0);
        wassert(actual(test->getMsgFlagged()) == 1);
        wassert(actual(test->changed()) == false);
    }
});

add_method("maildir", []() {
    std::vector<std::string> dirs;
    dirs.push_back(TEST_DATA_DIR "/maildir");
    dirs.push_back(TEST_DATA_DIR "/maildir/");
    for (const auto& i: dirs)
    {
        // An empty database should return empty sets, but not fail
        auto folders = buffy::MailFolder::enumerateFolders(i);

        wassert(actual(folders.size()) == 4u);

        std::shared_ptr<buffy::MailFolder> root;
        std::shared_ptr<buffy::MailFolder> empty;
        std::shared_ptr<buffy::MailFolder> test;
        std::shared_ptr<buffy::MailFolder> loop1;
        int missed = 0;
        for (int j = 0; j < 4; ++j)
        {
            //cerr << *i << " NAME: " << folders[j].name() << endl;
            if (folders[j]->name() == "maildir")
                root = folders[j];
            else if (folders[j]->name() == "maildir.empty")
                empty = folders[j];
            else if (folders[j]->name() == "maildir.test")
                test = folders[j];
            else if (folders[j]->name() == "maildir.loop1")
                loop1 = folders[j];
            else
                ++missed;
        }
        wassert(actual(missed) == 0);
        wassert_true(root);
        wassert_true(empty);
        wassert_true(test);
        wassert_true(loop1);

        wassert(actual(root->path()) == TEST_DATA_DIR "/maildir");
        wassert(actual(root->name()) == "maildir");
        wassert(actual(root->type()) == "Maildir");
        wassert(actual(empty->path()) == TEST_DATA_DIR "/maildir/empty");
        wassert(actual(empty->name()) == "maildir.empty");
        wassert(actual(empty->type()) == "Maildir");
        wassert(actual(test->path()) == TEST_DATA_DIR "/maildir/test");
        wassert(actual(test->name()) == "maildir.test");
        wassert(actual(test->type()) == "Maildir");
        wassert(actual(loop1->path()) == TEST_DATA_DIR "/maildir/loop1");
        wassert(actual(loop1->name()) == "maildir.loop1");
        wassert(actual(loop1->type()) == "Maildir");

        root->updateStatistics();

        wassert(actual(root->getMsgTotal()) == 0);
        wassert(actual(root->getMsgUnread()) == 0);
        wassert(actual(root->getMsgNew()) == 0);
        wassert(actual(root->getMsgFlagged()) == 0);
        wassert(actual(root->changed()) == false);

        empty->updateStatistics();

        wassert(actual(empty->getMsgTotal()) == 0);
        wassert(actual(empty->getMsgUnread()) == 0);
        wassert(actual(empty->getMsgNew()) == 0);
        wassert(actual(empty->getMsgFlagged()) == 0);
        wassert(actual(empty->changed()) == false);

        test->updateStatistics();

        wassert(actual(test->getMsgTotal()) == 3);
        wassert(actual(test->getMsgUnread()) == 0);
        wassert(actual(test->getMsgNew()) == 0);
        wassert(actual(test->getMsgFlagged()) == 1);
        wassert(actual(test->changed()) == false);

        loop1->updateStatistics();

        wassert(actual(loop1->getMsgTotal()) == 0);
        wassert(actual(loop1->getMsgUnread()) == 0);
        wassert(actual(loop1->getMsgNew()) == 0);
        wassert(actual(loop1->getMsgFlagged()) == 0);
        wassert(actual(loop1->changed()) == false);
    }
});

add_method("remove_mbox", []() {
    // Test that it is ok to remove a mail folder when its MailFolder
    // exists

    sys::Tempdir workdir;
    std::string testfile = str::joinpath(workdir.name(), "empty.mbox");

    // Create an empty mailbox
    sys::write_file(testfile, std::string());

    auto mf = std::make_shared<buffy::mailfolder::Mailbox>(testfile);

    mf->updateStatistics();

    wassert(actual(mf->getMsgTotal()) == 0);
    wassert(actual(mf->getMsgUnread()) == 0);
    wassert(actual(mf->getMsgNew()) == 0);
    wassert(actual(mf->getMsgFlagged()) == 0);

    sys::unlink(testfile);

    wassert_true(mf->changed());

    mf->updateStatistics();

    wassert(actual(mf->getMsgTotal()) == 0);
    wassert(actual(mf->getMsgUnread()) == 0);
    wassert(actual(mf->getMsgNew()) == 0);
    wassert(actual(mf->getMsgFlagged()) == 0);
});

add_method("remove_maildir", []() {
    // Test that it is ok to remove a mail folder when its MailFolder
    // exists

    sys::Tempdir workdir;

    // Create an empty maildir
    workdir.mkdirat("empty");
    sys::Path empty(str::joinpath(workdir.name(), "empty"));
    empty.mkdirat("cur");
    empty.mkdirat("new");

    auto mf = std::make_shared<buffy::mailfolder::Maildir>(empty.name());

    mf->updateStatistics();

    wassert(actual(mf->getMsgTotal()) == 0);
    wassert(actual(mf->getMsgUnread()) == 0);
    wassert(actual(mf->getMsgNew()) == 0);
    wassert(actual(mf->getMsgFlagged()) == 0);

    empty.rmdirat("cur");
    empty.rmdirat("new");
    workdir.rmdirat("empty");

    wassert_true(mf->changed());

    mf->updateStatistics();

    wassert(actual(mf->getMsgTotal()) == 0);
    wassert(actual(mf->getMsgUnread()) == 0);
    wassert(actual(mf->getMsgNew()) == 0);
    wassert(actual(mf->getMsgFlagged()) == 0);
});

}

}
