#include "buffy/utils/tests.h"
#include "buffy/config.h"
#include "buffy/mailfolder/mailbox.h"
#include <cstdlib>
#include <unistd.h>

using namespace buffy::utils::tests;

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

#if 0
struct buffy_config_shar {
    config::Config conf;
    buffy_config_shar() : conf("nonexisting-config.txt") {
    }
    ~buffy_config_shar() {
        unlink("test-config-saved.txt");
    }
};

template<> template<> void to::test<1>() { test1(); }
template<> template<> void to::test<2>() { test2(); }

// Check mail programs
template<> template<>
void to::test<3>()
{
    vector<string> programs = conf.mailPrograms();
    ensure_equals(programs.size(), 2u);

    ensure_equals(conf.selectedMailProgram().name(), "mutt");

    conf.selectMailProgram("Other");
    ensure_equals(conf.selectedMailProgram().name(), "Other");
    ensure_equals(conf.mailProgram("mutt").selected(), false);
}

// Check exotic characters in names
template<> template<>
void to::test<4>()
{
    string funky = "fo%%bar.baz >< foo/bar\\baz 2.0";
    ensure_equals(conf.application(funky).get("bar"), "");
    conf.application(funky).set("bar", "cippo2");
    ensure_equals(conf.application(funky).get("bar"), "cippo2");
}

// Check loading an existing configuration
template<> template<>
void to::test<5> ()
{
    config::Config conf0("test-config.txt");

    ensure(!conf0.view().read());
    ensure(!conf0.view().empty());
    ensure(conf0.view().important());
    ensure_equals(conf0.general().interval(), 600);

    vector<string> locations = conf0.locations();
    ensure_equals(locations.size(), 4u);

#if 0
    /*
       gen_ensure(locations.find("/var/mail/enrico") != locations.end());
       gen_ensure(locations.find("/home/enrico/Maildir") != locations.end());
       gen_ensure(locations.find("/home/enrico/Mail") != locations.end());
       gen_ensure(locations.find("/home/enrico/mail") != locations.end());
       */
#endif

    vector<string> programs = conf0.mailPrograms();
    ensure_equals(programs.size(), 2u);

    ensure_equals(conf0.selectedMailProgram().name(), "mutt");
}

// Check persistance when saving and reloading an existing configuration
template<> template<>
void to::test<6> ()
{
    config::Config conf0;
    stringstream str;
    str << getpid();
    string testString(str.str());
    buffy::MailFolder testFolder(new buffy::mailfolder::Mailbox("mbox/empty.mbox"));

    // Set a few nonstandard values, then save them
    vector<string> locations;
    conf0.location("foo");
    conf0.location("bar");
    conf0.location("baz");
    conf0.application("test").set("pid", testString);
    conf0.folder(testFolder).setForceView(true);
    conf0.folder(testFolder).setForceHide(false);
    conf0.save("test-config-saved.txt");
    //system("cat test-config-saved.txt");

    // Load the config file that we just saved
    config::Config conf1("test-config-saved.txt");
    ensure_equals(conf1.application("test").get("pid"), testString);
    ensure_equals(conf1.folder(testFolder).forceview(), true);
    ensure_equals(conf1.folder(testFolder).forcehide(), false);
    ensure_equals(conf1.locations().size(), 4u);
}

// Loading an empty file should not die
template<> template<>
void to::test<7> ()
{
    system("rm -f foo.txt; touch foo.txt");
    config::Config conf("foo.txt");
}
#endif

}

}
