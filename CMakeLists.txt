cmake_minimum_required(VERSION 3.1)
project (sensornetwork)


find_package (Threads)

include_directories(inc rapidxml mysql/mysqlconnectorc/include)
link_directories(mysql/mysqlconnectorc/lib)

add_executable(sensornetwork
  # headers
  inc/Common.h
  inc/Core.h
  inc/InComing.h
  inc/LiveDataBase.h
  inc/LoadConfig.h
  inc/RPck.h
  inc/StartUpWSN.h
  inc/utils.h  
  inc/WSNCheck1.h
  inc/WSNCheck2.h
  inc/WSNCheck3.h
  
  # sources
  src/main.cpp
  src/utils.c
  src/Core.cpp
  src/InComing.cpp
  src/LiveDataBase.cpp
  src/LoadConfig.cpp
  src/RPck.cpp
  src/SPck.cpp 
  src/StartUpWSN.cpp
  src/WSNCheck1.cpp
  src/WSNCheck2.cpp
  src/WSNCheck3.cpp

  # rapidxml
  rapidxml/rapidxml.hpp
  rapidxml/rapidxml_utils.hpp
  rapidxml/rapidxml_print.hpp
  rapidxml/rapidxml_iterators.hpp
)

target_link_libraries (sensornetwork ${CMAKE_THREAD_LIBS_INIT} mysql)

set(CMAKE_INSTALL_PREFIX /opt)
install(TARGETS sensornetwork DESTINATION ${CMAKE_INSTALL_PREFIX})
