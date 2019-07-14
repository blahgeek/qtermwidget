add_executable(bqterm ./bqterm.cpp)
target_link_libraries(bqterm ${QTERMWIDGET_LIBRARY_NAME})

install(TARGETS bqterm
    RUNTIME DESTINATION bin)
