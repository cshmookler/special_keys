# Declare project information
project(
    'special_keys',
    'cpp',
    version : files('VERSION'),
    default_options : [ 'cpp_std=c++17' ],
)

# Project root and build directories
root_dir = meson.project_source_root()
build_dir = meson.project_build_root()
src_dir = root_dir / 'src'
tests_dir = root_dir / 'tests'

# Insert the project version into the version header file
conf_data = configuration_data()
conf_data.set('version', meson.project_version())
configure_file(
    configuration : conf_data,
    input : src_dir / 'version.hpp.in',
    output : 'version.hpp',
)

dep_alsa = dependency(
    'alsa',
    required : true,
    method : 'auto',
)

exe_special_keys = executable(
    'special_keys',
    files(
        src_dir / 'main.cpp',
        src_dir / 'version.cpp',
        src_dir / 'function.cpp',
        src_dir / 'audio.cpp'
    ),
    dependencies : [ dep_alsa ],
    link_args : '-lstatus_bar_notify',
    install : true,
)

dep_gtest_main = dependency(
    'gtest_main',
    required : false,
    method : 'auto',
)

if dep_gtest_main.found()
    test_version = executable(
        'version',
        files(
            tests_dir / 'version.test.cpp',
            src_dir / 'version.cpp',
        ),
        dependencies : dep_gtest_main,
    )
    test('version', test_version)
else
    warning('Skipping tests due to missing dependencies')
endif
