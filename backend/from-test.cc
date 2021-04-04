#include "utils/tests.h"
#include "from.h"

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

Tests test("mailfolder_from");

void Tests::register_tests() {

add_method("sample", []() {
    wassert_true(buffy::mailfolder::is_from("From www-data@klecker.debian.org Tue Sep 14 14:57:55 2004"));
});

}

}
