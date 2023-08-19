set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS
        "main.c"
        "utils.c"
        "nvs_stuff.c"
        "gap.c"
        "gatt_and_ota.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
        "."
)

register_component()