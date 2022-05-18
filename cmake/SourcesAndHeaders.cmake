set(sources
    src/log.cpp
	src/config.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/fishjoy/log.hpp
	include/fishjoy/singleton.hpp
	include/fishjoy/config.hpp
)

set(test_sources
  src/log_test.cpp
  src/log_units_test.cpp
)
