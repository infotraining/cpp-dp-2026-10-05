add_executable(Shapes_tests
    shapes_json_parsing_tests.cpp
    shapes_text_parsing_tests.cpp
)

target_link_libraries(Shapes_tests
    PRIVATE
        ShapesLib
        Catch2::Catch2WithMain
)

catch_discover_tests(Shapes_tests)