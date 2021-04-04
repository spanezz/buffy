# Testable library code

toplevel_inc = include_directories('..')

libbuffy_sources = [
    'utils/subprocess.cc',
    'utils/string.cc',
    'utils/sys.cc',
    'utils/term.cc',
    'utils/tests.cc',
    'utils/testrunner.cc',
    'mailfolder.cc',
    'from.cc',
    'mailbox.cc',
    'maildir.cc',
    'config.cc',
]

libbuffy = library('buffy',
        libbuffy_sources,
        include_directories: toplevel_inc,
        dependencies: [
                glib_dep, zlib_dep
        ])

#
# Unit testing
#

test_buffy_sources = [
    'config-test.cc',
    'mailfolder-test.cc',
    'from-test.cc',
    'maildir-test.cc',
    'mailbox-test.cc',
    'utils/tests-main.cc',
]

testdata_dir = meson.source_root() / 'testdata'

test_buffy = executable('test-buffy', test_buffy_sources,
        include_directories: toplevel_inc,
    cpp_args: '-DTEST_DATA_DIR="' + testdata_dir + '"',
        link_with: [
                libbuffy,
        ],
        dependencies: [
                glib_dep, zlib_dep
        ])

# Test suite
# add_definitions(-DTEST_DATA_DIR="${CMAKE_SOURCE_DIR}/testdata")

test('buffy', test_buffy)
