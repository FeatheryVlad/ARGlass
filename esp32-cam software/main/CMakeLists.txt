set(srcs "main.c"
         "st7735s.c"
         "fontx.c"
         "camera.c"
         "http_client.c"
         "wifi.c"
         "button.c"
         "nav_menu.c"
        # "decode_jpeg.c"
        # "decode_png.c"
    )

set(requires "")

#idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")esp_wifi
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES ${requires}
                    EMBED_TXTFILES howsmyssl_com_root_cert.pem
                    postman_root_cert.pem)