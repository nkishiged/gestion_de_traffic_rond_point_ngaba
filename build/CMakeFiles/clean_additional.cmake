# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\gestion_trafic_rond_point_ngaba_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\gestion_trafic_rond_point_ngaba_autogen.dir\\ParseCache.txt"
  "gestion_trafic_rond_point_ngaba_autogen"
  )
endif()
