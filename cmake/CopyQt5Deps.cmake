function(copy_Qt5_deps target_dir)
    include(Cmake/WindowsCopyFiles.cmake)

    set(DLL_DEST "$<TARGET_FILE_DIR:${target_dir}>/")
    set(Qt5_DLL_DIR "${Qt5_DIR}/../../../bin")
    set(Qt5_PLATFORMS_DIR "${Qt5_DIR}/../../../plugins/platforms/")
    set(Qt5_STYLES_DIR "${Qt5_DIR}/../../../plugins/styles/")
    set(Qt5_IMAGEFORMATS_DIR "${Qt5_DIR}/../../../plugins/imageformats/")
    set(Qt5_RESOURCES_DIR "${Qt5_DIR}/../../../resources/")
    set(PLATFORMS ${DLL_DEST}platforms/)
    set(STYLES ${DLL_DEST}styles/)
    set(IMAGEFORMATS ${DLL_DEST}imageformats/)

    windows_copy_files(${target_dir} ${Qt5_DLL_DIR} ${DLL_DEST}
        Qt5Core$<$<CONFIG:Debug>:d>.*
        Qt5Gui$<$<CONFIG:Debug>:d>.*
        Qt5Network$<$<CONFIG:Debug>:d>.*
        Qt5Widgets$<$<CONFIG:Debug>:d>.*
        Qt5Svg$<$<CONFIG:Debug>:d>.*
        Qt5OpenGL$<$<CONFIG:Debug>:d>.*
        Qt5PrintSupport$<$<CONFIG:Debug>:d>.*
        Qt5MultimediaWidgets$<$<CONFIG:Debug>:d>.*
        Qt5QuickWidgets$<$<CONFIG:Debug>:d>.*
        Qt5Multimedia$<$<CONFIG:Debug>:d>.*
        Qt5Quick$<$<CONFIG:Debug>:d>.*
        Qt5Sql$<$<CONFIG:Debug>:d>.*
        Qt5XmlPatterns$<$<CONFIG:Debug>:d>.*
        Qt5Qml$<$<CONFIG:Debug>:d>.*
        Qt5Xml$<$<CONFIG:Debug>:d>.*
    )

    windows_copy_files(${target_dir} ${Qt5_PLATFORMS_DIR} ${PLATFORMS} qwindows$<$<CONFIG:Debug>:d>.*)
    windows_copy_files(${target_dir} ${Qt5_IMAGEFORMATS_DIR} ${IMAGEFORMATS} *.dll)
endfunction(copy_Qt5_deps)