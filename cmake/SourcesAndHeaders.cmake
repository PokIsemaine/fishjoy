set(sources
	src/log.cpp
	src/config.cpp
	src/util.cpp
	src/thread.cpp
	src/mutex.cpp
	src/fiber.cpp
	src/scheduler.cpp
	src/env.cpp
	src/iomanager.cpp
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
	include/fishjoy/env.hpp
	include/fishjoy/iomanager.hpp
)

set(test_sources
  	src/test_log.cpp
#  	src/test_thread.cpp
	src/test_config.cpp
#	src/test_fiber.cpp
#	src/test_fiber2.cpp
#  	src/test_scheduler.cpp
)
