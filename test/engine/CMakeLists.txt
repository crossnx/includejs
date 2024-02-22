if(INCLUDEJS_BACKEND STREQUAL "V8")
  # TODO(RaisinTen): Start testing the V8 backend.
  return()
endif()

add_executable(includejs_engine_unit
  engine_binding_test.cc
  engine_evaluate_test.cc
  engine_stacktraces_test.cc
  engine_value_object_test.cc
  engine_value_array_test.cc
  engine_value_function_test.cc
  engine_value_undefined_test.cc
  engine_value_null_test.cc)

sourcemeta_includejs_add_compile_options(includejs_engine_unit)

target_link_libraries(includejs_engine_unit
  PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(includejs_engine_unit
  PRIVATE includejs::engine)
gtest_discover_tests(includejs_engine_unit)
set_target_properties(includejs_engine_unit
  PROPERTIES FOLDER "IncludeJS/Engine")
