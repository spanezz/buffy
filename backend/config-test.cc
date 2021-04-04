#include "utils/tests.h"
#include "utils/sys.h"
#include "utils/string.h"
#include "config.h"
#include "mailbox.h"
#include <cstdlib>
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

Tests test("config");

void Tests::register_tests() {

add_method("application_value", []() {
    buffy::config::Config conf;
    wassert(actual(conf.application("foo").get("bar")) == "");
    conf.application("foo").addDefault("bar", "baz");
    wassert(actual(conf.application("foo").get("bar")) == "baz");
    conf.application("foo").set("bar", "cippo");
    wassert(actual(conf.application("foo").get("bar")) == "cippo");
});

add_method("application_value_with_path", []() {
    buffy::config::Config conf;
    wassert(actual(conf.application("foo").get("bar/baz")) == "");
    conf.application("foo").addDefault("bar/baz", "lippo");
    wassert(actual(conf.application("foo").get("bar/baz")) == "lippo");
    conf.application("foo").set("bar/baz", "lippo1");
    wassert(actual(conf.application("foo").get("bar/baz")) == "lippo1");
});

add_method("mail_programs", []() {
    buffy::config::Config conf;
    auto programs = conf.mailPrograms();
    wassert(actual(programs.size()) == 2u);

    wassert(actual(conf.selectedMailProgram().name()) == "mutt");

    conf.selectMailProgram("Other");
    wassert(actual(conf.selectedMailProgram().name()) == "Other");
    wassert(actual(conf.mailProgram("mutt").selected()) == false);
});

add_method("exotic_chars", []() {
    buffy::config::Config conf;
    std::string funky = "fo%%bar.baz >< foo/bar\\baz 2.0";
    wassert(actual(conf.application(funky).get("bar")) == "");
    conf.application(funky).set("bar", "cippo2");
    wassert(actual(conf.application(funky).get("bar")) == "cippo2");
});

add_method("load", []() {
    buffy::config::Config conf0(TEST_DATA_DIR "/test-config.txt");

    wassert_false(conf0.view().read());
    wassert_false(conf0.view().empty());
    wassert_true(conf0.view().important());
    wassert(actual(conf0.general().interval()) == 600);

    auto locations = conf0.locations();
    wassert(actual(locations.size()) == 4u);

#if 0
    /*
       gen_ensure(locations.find("/var/mail/enrico") != locations.end());
       gen_ensure(locations.find("/home/enrico/Maildir") != locations.end());
       gen_ensure(locations.find("/home/enrico/Mail") != locations.end());
       gen_ensure(locations.find("/home/enrico/mail") != locations.end());
       */
#endif

    auto programs = conf0.mailPrograms();
    wassert(actual(programs.size()) == 2u);

    wassert(actual(conf0.selectedMailProgram().name()) == "mutt");
});

add_method("persist", []() {
    sys::Tempdir workdir;
    std::string workfile = str::joinpath(workdir.name(), "test-config-saved.txt");

    buffy::config::Config conf0;
    std::string testString(std::to_string(getpid()));
    auto testFolder = std::make_shared<buffy::mailfolder::Mailbox>("mbox/empty.mbox");

    // Set a few nonstandard values, then save them
    std::vector<std::string> locations;
    conf0.location("foo");
    conf0.location("bar");
    conf0.location("baz");
    conf0.application("test").set("pid", testString);
    conf0.folder(testFolder).setForceView(true);
    conf0.folder(testFolder).setForceHide(false);
    conf0.save(workfile);

    // Load the config file that we just saved
    buffy::config::Config conf1(workfile);
    wassert(actual(conf1.application("test").get("pid")) == testString);
    wassert(actual(conf1.folder(testFolder).forceview()) == true);
    wassert(actual(conf1.folder(testFolder).forcehide()) == false);
    wassert(actual(conf1.locations().size()) == 4u);
});

add_method("load_empty", []() {
    sys::Tempdir workdir;
    std::string testfile = str::joinpath(workdir.name(), "foo.txt");
    sys::write_file(testfile, std::string());
    buffy::config::Config conf(testfile);
});

#if 0
struct buffy_config_shar {
    config::Config conf;
    buffy_config_shar() : conf("nonexisting-config.txt") {
    }
    ~buffy_config_shar() {
        unlink("test-config-saved.txt");
    }
};

#endif

}

}
