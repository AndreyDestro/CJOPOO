# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Jogo_da_velha_binario_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Jogo_da_velha_binario_autogen.dir\\ParseCache.txt"
  "Jogo_da_velha_binario_autogen"
  )
endif()
