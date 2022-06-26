set(srcs Wifi.cpp Telnet.cpp IPAddress.cpp ../submodules/libtelnet/libtelnet.c)
set(include_dirs . ../submodules/libtelnet/)
set(requires esp_wifi nvs_flash utility)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE)