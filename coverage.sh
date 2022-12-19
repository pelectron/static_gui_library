# This file builds the tests of sgl and generates a coverage report for them
# The coverage report files will be generated in build-cov/meson-logs.
# if meson complains about not being able to locate gcovr, install it with pip, i.e.
# 'pip3 install gcovr'

# change the browser to your preferred one for viewing the report
BROWSER=firefox

# setting up build directory
meson setup build-cov -Db_coverage=true -Dtest=enabled
# building tests
meson compile -C build-cov
# executing tests
./build-cov/tests/test_main
# building coverage report
ninja coverage -C build-cov
# viewing the results
$BROWSER build-cov/meson-logs/coveragereport/index.html
