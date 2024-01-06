add_executable(mdljazz main.c pak.c gl_qtex.c glad.c gl_mdl.c)
target_link_libraries(mdljazz SDL2main SDL2-static)
target_include_directories(mdljazz PRIVATE ${PROJECT_SOURCE_DIR}/SDL/include ${CMAKE_CURRENT_SOURCE_DIR}/)
