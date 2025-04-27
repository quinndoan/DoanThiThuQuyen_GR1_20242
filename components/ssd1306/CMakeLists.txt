# set(component_srcs "ssd1306.c" "ssd1306_spi.c")

# # get IDF version for comparison
# set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

# if(idf_version VERSION_GREATER_EQUAL "5.2")
# 	if(CONFIG_LEGACY_DRIVER)
# 		list(APPEND component_srcs "ssd1306_i2c_legacy.c")
# 	else()
# 		list(APPEND component_srcs "ssd1306_i2c_new.c")
# 	endif()
# else()
# 	list(APPEND component_srcs "ssd1306_i2c_legacy.c")
# endif()

# idf_component_register(SRCS "${component_srcs}" PRIV_REQUIRES driver INCLUDE_DIRS ".")

# Tạo danh sách file source cơ bản
set(component_srcs
    ssd1306.c
    ssd1306_spi.c
)

# Gộp version string
set(idf_version_str "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

# Thêm file i2c theo version và cấu hình
if(idf_version_str VERSION_GREATER_EQUAL "5.2")
    if(CONFIG_LEGACY_DRIVER)
        list(APPEND component_srcs ssd1306_i2c_legacy.c)
    else()
        list(APPEND component_srcs ssd1306_i2c_new.c)
    endif()
else()
    list(APPEND component_srcs ssd1306_i2c_legacy.c)
endif()

# Đăng ký component
idf_component_register(
    SRCS
        ${component_srcs}
    INCLUDE_DIRS
        .
    PRIV_REQUIRES
        driver
)

# Thêm các tùy chọn compile flags cho chất lượng code
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Werror
    -Wextra
    -Wall
    -Wshadow
    -Wcast-qual
    -Wswitch-default
    -Wunreachable-code
    -Wuninitialized
    -Winit-self
    -Wpointer-arith
    -Werror-implicit-function-declaration
)
