set(sources
	src/log.cpp
	src/config.cpp
	src/util.cpp
	src/thread.cpp
	src/mutex.cpp
	src/fiber.cpp
	src/scheduler.cpp
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
	include/fishjoy/fiber.hpp
	include/fishjoy/scheduler.hpp
)

set(test_sources
#  	src/log_test.cpp
#  	src/log_units_test.cpp
#  	src/test_thread.cpp
#	src/test_config.cpp
#	src/test_fiber.cpp
#	src/test_fiber2.cpp
  	src/test_scheduler.cpp
)
