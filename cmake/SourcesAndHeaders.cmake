set(sources
    src/log.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/fishjoy/log.hpp
	include/fishjoy/singleton.hpp
)

set(test_sources
  src/log_test.cpp
  src/log_units_test.cpp
)
