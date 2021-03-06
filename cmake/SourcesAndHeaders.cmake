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
	src/timer.cpp
	src/address.cpp
	src/socket.cpp
	src/bytearray.cpp
	src/hook.cpp
	src/fd_manager.cpp
	src/tcp_server.cpp
	src/stream.cpp
	src/streams/socket_stream.cpp
	src/http/http-parser/http_parser.cpp
	src/http/http.cpp
	src/http/http_connection.cpp
	src/http/http_parser.cpp
	src/http/http_server.cpp
	src/http/http_session.cpp
	src/http/servlet.cpp
	src/uri.cpp
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
	include/fishjoy/timer.hpp
	include/fishjoy/address.hpp
	include/fishjoy/socket.hpp
	include/fishjoy/bytearray.hpp
	include/fishjoy/hook.hpp
	include/fishjoy/fd_manager.hpp
	include/fishjoy/tcp_server.hpp
	include/fishjoy/stream.hpp
	include/fishjoy/streams/socket_stream.hpp
	include/fishjoy/http/http-parser/http_parser.hpp
	include/fishjoy/http/http.hpp
	include/fishjoy/http/http_connection.hpp
	include/fishjoy/http/http_parse.hpp
	include/fishjoy/http/http_server.hpp
	include/fishjoy/http/http_session.hpp
	include/fishjoy/http/servlet.hpp
	include/fishjoy/uri.hpp
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
