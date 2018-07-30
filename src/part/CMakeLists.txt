add_definitions(-DTRANSLATION_DOMAIN=\"kddrescueviewpart\")

set(kddrescueview_PART_SRCS
   kddrescueviewpart.cpp
   rescuemap.cpp
   rescuestatus.cpp
   blockwidget.cpp
)

add_library(kddrescueviewpart MODULE ${kddrescueview_PART_SRCS})

target_link_libraries(kddrescueviewpart
    KF5::I18n
    KF5::Parts
)

install(TARGETS kddrescueviewpart  DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES kddrescueviewpart.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES kddrescueviewpartui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kddrescueviewpart)
