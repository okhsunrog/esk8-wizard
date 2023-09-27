set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS
        "main.cpp"
        "utils.cpp"
        "nvs_stuff.cpp"
        "gap.cpp"
        "gatt_and_ota.cpp"
        "led_strip.cpp"
        "can.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS
        "."
)

register_component()
