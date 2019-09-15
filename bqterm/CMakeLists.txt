SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(Qt5Network "${QT_MINIMUM_VERSION}" REQUIRED)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE bqterm.icns)
    add_executable(bqterm MACOSX_BUNDLE
        ./bqterm.cpp
        ./macos_utils.m
        ./res/bqterm.icns)
    set_source_files_properties(./res/bqterm.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_link_libraries(bqterm "-framework Cocoa")
else ()
    add_executable(bqterm ./bqterm.cpp)
    install(TARGETS bqterm
        RUNTIME DESTINATION bin)
endif ()

target_link_libraries(bqterm ${QTERMWIDGET_LIBRARY_NAME} Qt5::Network)

