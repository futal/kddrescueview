set(kddrescueview_SRCS
   main.cpp
   kddrescueviewshell.cpp
)

add_executable(kddrescueview ${kddrescueview_SRCS})

target_link_libraries(kddrescueview
    KF5::I18n
    KF5::Parts
)

install(TARGETS kddrescueview ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kddrescueview.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES kddrescueviewui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kddrescueview)
