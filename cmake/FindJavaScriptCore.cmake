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

    # If find_package(JavaScriptCore [version]) is called we only look for that
    # otherwise loop through a list of supported options, pick the first one found
    # and error out if there is no match
    if(JavaScriptCore_FIND_VERSION)
      pkg_check_modules(JAVASCRIPTCORE javascriptcoregtk-${JavaScriptCore_FIND_VERSION} REQUIRED)
    else()
      set(supported_versions 6.0 4.1 4.0)
      foreach(ver ${supported_versions})
        pkg_check_modules(JAVASCRIPTCORE javascriptcoregtk-${ver})
        if(JAVASCRIPTCORE_FOUND)
          set(JavaScriptCore_FIND_VERSION ${ver})
          break()
        endif()
      endforeach()
      if(NOT JAVASCRIPTCORE_FOUND)
        message(FATAL_ERROR "Required package JavaScriptCore was not found")
      endif()
    endif()

    set_property(TARGET javascript_core PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${JAVASCRIPTCORE_INCLUDE_DIRS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_COMPILE_OPTIONS ${JAVASCRIPTCORE_CFLAGS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_LINK_OPTIONS ${JAVASCRIPTCORE_LDFLAGS})
    set_property(TARGET javascript_core PROPERTY
      INTERFACE_LINK_LIBRARIES ${JAVASCRIPTCORE_LINK_LIBRARIES})

    if(JavaScriptCore_FIND_VERSION STREQUAL "6.0")
      # NOTE: this is required since WebkitGTK (which includes JavaScriptCore) only
      # exposes the GLib headers starting in versions after 4.0
      # see: https://github.com/WebKit/WebKit/blob/main/Source/WebKit/gtk/migrating-to-webkitgtk-6.0.md#stop-using-deprecated-apis
      FILE(GLOB headers ${PROJECT_SOURCE_DIR}/vendor/webkitgtk/Source/JavaScriptCore/API/*.h)
      FOREACH(header ${headers})
        cmake_path(GET header FILENAME header_filename)
        configure_file("${header}" "${PROJECT_BINARY_DIR}/javascriptcore-headers/JavaScriptCore/${header_filename}" COPYONLY)
      ENDFOREACH()
      set_property(TARGET javascript_core PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/javascriptcore-headers")
    endif()

    add_library(JavaScriptCore::JavaScriptCore ALIAS javascript_core)
    set(JavaScriptCore_FOUND ON)
  endif()
endif()
