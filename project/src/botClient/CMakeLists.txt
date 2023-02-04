project(bot-client VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
    # headers
    tcpclient.h
    tcpclient.ui
    #sources
    main.cpp
    tcpclient.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(bot-client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(bot-client SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(bot-client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(bot-client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Network)

set_target_properties(bot-client PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(bot-client)
endif()
