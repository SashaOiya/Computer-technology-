# CMake generated Testfile for 
# Source directory: /home/sasha/program/HWT/test/e2e_tests
# Build directory: /home/sasha/program/HWT/build/test/e2e_tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TreeE2E "bash" "/home/sasha/program/HWT/test/e2e_tests/test.sh" "/home/sasha/program/HWT/build/hwt" "/home/sasha/program/HWT/build/set" "/home/sasha/program/HWT/test/e2e_tests/data")
set_tests_properties(TreeE2E PROPERTIES  _BACKTRACE_TRIPLES "/home/sasha/program/HWT/test/e2e_tests/CMakeLists.txt;1;add_test;/home/sasha/program/HWT/test/e2e_tests/CMakeLists.txt;0;")
