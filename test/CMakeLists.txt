find_package(doctest CONFIG REQUIRED)

add_executable(test_raidsim
    test_fight.cpp
)

target_link_libraries(test_raidsim
  PRIVATE
    raidsim_core
    doctest::doctest
)

add_test(NAME test_raidsim COMMAND test_raidsim)
