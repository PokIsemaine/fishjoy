set(sources
    src/log.cpp
	src/config.cpp
	src/util.cpp
	src/thread.cpp
	src/mutex.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/fishjoy/log.hpp
	include/fishjoy/singleton.hpp
	include/fishjoy/config.hpp
	include/fishjoy/thread.hpp
	include/fishjoy/noncopyable.hpp
	include/fishjoy/mutex.hpp
)

set(test_sources
  src/log_test.cpp
  src/log_units_test.cpp
  src/test_thread.cpp
)
