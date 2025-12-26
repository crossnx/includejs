if(NOT JavaScriptCore_FOUND)
  if(APPLE)
    add_library(javascript_core INTERFACE IMPORTED)
    target_link_libraries(javascript_core INTERFACE "-framework Foundation")
    target_link_libraries(javascript_core INTERFACE "-framework JavaScriptCore")
    add_library(JavaScriptCore::JavaScriptCore ALIAS javascript_core)
    set(JavaScriptCore_FOUND ON)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(javascript_core INTERFACE IMPORTED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JAVASCRIPTCORE REQUIRED javascriptcoregtk-4.1)
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${JAVASCRIPTCORE_INCLUDE_DIRS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_COMPILE_OPTIONS ${JAVASCRIPTCORE_CFLAGS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_LINK_OPTIONS ${JAVASCRIPTCORE_LDFLAGS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_LINK_LIBRARIES ${JAVASCRIPTCORE_LINK_LIBRARIES})
    add_library(JavaScriptCore::JavaScriptCore ALIAS javascript_core)
    set(JavaScriptCore_FOUND ON)
  endif()
endif()
