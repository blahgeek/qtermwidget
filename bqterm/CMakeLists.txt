SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


if (APPLE)
    add_executable(bqterm MACOSX_BUNDLE
        ./bqterm.cpp
        ./macos_utils.m)
    target_link_libraries(bqterm "-framework Cocoa")
else ()
    add_executable(bqterm ./bqterm.cpp)
endif ()

target_link_libraries(bqterm ${QTERMWIDGET_LIBRARY_NAME})

# install(TARGETS bqterm
#     RUNTIME DESTINATION bin)
