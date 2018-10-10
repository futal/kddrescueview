add_definitions(-DTRANSLATION_DOMAIN=\"kddrescueviewpart\")

set(kddrescueview_PART_SRCS
    block_position.cpp
    block_size.cpp
    block_status.cpp
    kddrescueviewpart.cpp
    rescue_map.cpp
    rescue_map_view.cpp
    rescue_operation.cpp
    rescue_status.cpp
    rescue_totals.cpp
    square_color.cpp
)

add_library(kddrescueviewpart MODULE ${kddrescueview_PART_SRCS})

target_link_libraries(kddrescueviewpart
    KF5::I18n
    KF5::Parts
)

install(TARGETS kddrescueviewpart  DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES kddrescueviewpart.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES kddrescueviewpartui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kddrescueviewpart)
