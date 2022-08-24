set(sources
	#utils
	src/utils/file.cpp
	src/utils/util.cpp
	#streams
	src/streams/stream.cpp
	src/streams/socket_stream.cpp
	#net
	src/net/address.cpp
	src/net/socket.cpp
	src/net/bytearray.cpp
	src/net/fd_manager.cpp
	src/net/tcp_server.cpp
	src/net/uri.cpp

	src/log/log.cpp
	src/log/log_file.cpp
	src/log/log_buffer.cpp


	src/config/config.cpp
	src/config/env.cpp

	src/thread/thread.cpp
	src/thread/semaphore.cpp

	src/fiber/fiber.cpp
	src/fiber/scheduler.cpp
	src/fiber/iomanager.cpp
	src/fiber/timer.cpp
	src/fiber/hook.cpp

	src/http/http-parser/http_parser.cpp
	src/http/http.cpp
	src/http/http_connection.cpp
	src/http/http_parser.cpp
	src/http/http_server.cpp
	src/http/http_session.cpp
	src/http/servlet.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
	include/fishjoy/utils.hpp
	include/fishjoy/streams.hpp
	include/fishjoy/net.hpp
	include/fishjoy/thread.hpp
	include/fishjoy/log.hpp
)

set(test_sources
  	src/test_log.cpp
  	src/test_thread.cpp
	src/test_config.cpp
	src/test_env.cpp
	src/test_fiber.cpp
	src/test_fiber2.cpp
  	src/test_scheduler.cpp
	src/test_iomanager.cpp
	src/test_hook.cpp
)
