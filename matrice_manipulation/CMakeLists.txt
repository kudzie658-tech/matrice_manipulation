# CMakeList.txt : CMake project for matrice_manipulation, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (matrice_manipulation "matrice_manipulation.cpp" "matrice_manipulation.h" "matrice.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET matrice_manipulation PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen installation
set(EIGEN3_INCLUDE_DIR
    "C:/Users/kudzie658/Documents/Development/cpp_config/eigen-5.0.1"
)

add_library(Eigen3::Eigen INTERFACE IMPORTED)

set_target_properties(Eigen3::Eigen PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}"
)

target_link_libraries(matrice_manipulation
    PRIVATE Eigen3::Eigen
)