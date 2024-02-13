add_executable(sourcemeta_includejs_engine_unit
  engine_binding_test.cc
  engine_evaluate_test.cc
  engine_stacktraces_test.cc)

sourcemeta_includejs_add_compile_options(sourcemeta_includejs_engine_unit)

target_link_libraries(sourcemeta_includejs_engine_unit
  PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(sourcemeta_includejs_engine_unit
  PRIVATE sourcemeta::includejs::engine)
gtest_discover_tests(sourcemeta_includejs_engine_unit)
set_target_properties(sourcemeta_includejs_engine_unit
  PROPERTIES FOLDER "IncludeJS/Engine")
