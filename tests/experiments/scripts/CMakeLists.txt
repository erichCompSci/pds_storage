cmake_minimum_required(VERSION 2.8)

ADD_CUSTOM_TARGET( copy_scripts ALL )

file(GLOB files "*.sh")
foreach(the_file ${files})
get_filename_component(current_script
                       ${the_file}
                       ABSOLUTE
                      )
ADD_CUSTOM_COMMAND( TARGET copy_scripts
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${current_script} ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()


