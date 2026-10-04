#########################################
# Dependencies for the project

include(FetchContent)

#########################################
# Catch2

FetchContent_Declare( 
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.15.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

include(CTest)
include(Catch)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# target_link_libraries(target_name PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

#########################################
# GTest

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.18.0
)

FetchContent_MakeAvailable(googletest)

# target_link_libraries(target_name PRIVATE gtest gtest_main)


#########################################
# nlohmann/json

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

# target_link_libraries(target_name PRIVATE nlohmann_json::nlohmann_json)


#########################################
# Boost

# FetchContent_Declare(
#   Boost
#   URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
#   URL_MD5 893b5203b862eb9bbd08553e24ff146a
#   DOWNLOAD_EXTRACT_TIMESTAMP ON
# )

# set(BOOST_INCLUDE_LIBRARIES flyweight)
# FetchContent_MakeAvailable(Boost)

# target_link_libraries(target_name PRIVATE Boost::flyweight)

#########################################
# xyz_prorocols

if(CMAKE_CXX_STANDARD EQUAL 26)
  FetchContent_Declare(
    cc-protocol
    GIT_REPOSITORY https://github.com/jbcoe/cc-protocol.git
    GIT_TAG        main
  )

  FetchContent_MakeAvailable(cc-protocol)
endif()

