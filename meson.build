project('buffy', 'cpp', version: '2.0', license : 'GPL-2.0-or-later', default_options: ['warning_level=3', 'cpp_std=c++17'])

add_global_arguments('-Wextra', '-Wno-unused-parameter', language : 'cpp')

# Dependencies
# qt5 = import('qt5')
# qt5core_dep = dependency('qt5', modules: 'Core')
# qt5gui_dep = dependency('qt5', modules: 'Gui')
# qt5widgets_dep = dependency('qt5', modules: 'Widgets')
glib_dep = dependency('glib-2.0')
zlib_dep = dependency('zlib')
gtkmm_dep = dependency('gtkmm-3.0')

subdir('backend')

buffy_sources = [
    'buffy.cpp',
#     'folders.cpp',
#     'folderview.cpp',
#     'sorterfilter.cpp',
#     'tray.cpp',
    'main.cpp',
]

# buffy_sources += qt5.preprocess(
#     moc_headers: ['folders.h', 'folderview.h', 'sorterfilter.h', 'tray.h', 'buffy.h'],
#     qresource: ['resources.qrc'],
#     ui_files: ['buffy.ui'],
# )

toplevel_inc = include_directories('.')

executable('buffy', buffy_sources,
       link_with: [libbuffy],
       include_directories: toplevel_inc,
       install: true,
       dependencies: [
           # qt5core_dep, qt5gui_dep, qt5widgets_dep,
           gtkmm_dep,
       ],
)


# add_executable(buffy ${BUFFY_SOURCES})
# target_include_directories(buffy PRIVATE ${CMAKE_SOURCE_DIR})
# target_link_libraries(buffy ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES} libbuffy)
# target_include_directories(buffy PUBLIC ${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
# target_compile_options(buffy PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})
# install(TARGETS buffy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
