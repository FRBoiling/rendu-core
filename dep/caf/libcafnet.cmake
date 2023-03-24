set(net_dir ${CMAKE_CURRENT_SOURCE_DIR}/net)

CollectHeaderFiles(
    ${net_dir}/net
    caf_net_headers
)

set(caf_net_files
    ${net_dir}/src/detail/convert_ip_endpoint.cpp
    ${net_dir}/src/detail/rfc6455.cpp
    ${net_dir}/src/net/abstract_actor_shell.cpp
    ${net_dir}/src/net/actor_shell.cpp
    ${net_dir}/src/net/binary/default_trait.cpp
    ${net_dir}/src/net/binary/frame.cpp
    ${net_dir}/src/net/binary/lower_layer.cpp
    ${net_dir}/src/net/binary/upper_layer.cpp
    ${net_dir}/src/net/datagram_socket.cpp
    ${net_dir}/src/net/generic_lower_layer.cpp
    ${net_dir}/src/net/generic_upper_layer.cpp
    ${net_dir}/src/net/http/header.cpp
    ${net_dir}/src/net/http/lower_layer.cpp
    ${net_dir}/src/net/http/serve.cpp
    ${net_dir}/src/net/http/method.cpp
    ${net_dir}/src/net/http/request.cpp
    ${net_dir}/src/net/http/response.cpp
    ${net_dir}/src/net/http/serve.cpp
    ${net_dir}/src/net/http/server.cpp
    ${net_dir}/src/net/http/status.cpp
    ${net_dir}/src/net/http/upper_layer.cpp
    ${net_dir}/src/net/http/v1.cpp
    ${net_dir}/src/net/ip.cpp
    ${net_dir}/src/net/length_prefix_framing.cpp
    ${net_dir}/src/net/middleman.cpp
    ${net_dir}/src/net/multiplexer.cpp
    ${net_dir}/src/net/network_socket.cpp
    ${net_dir}/src/net/pipe_socket.cpp
    ${net_dir}/src/net/pollset_updater.cpp
    ${net_dir}/src/net/prometheus/server.cpp
    ${net_dir}/src/net/socket.cpp
    ${net_dir}/src/net/socket_event_layer.cpp
    ${net_dir}/src/net/socket_manager.cpp
    ${net_dir}/src/net/ssl/acceptor.cpp
    ${net_dir}/src/net/ssl/connection.cpp
    ${net_dir}/src/net/ssl/context.cpp
    ${net_dir}/src/net/ssl/dtls.cpp
    ${net_dir}/src/net/ssl/format.cpp
    ${net_dir}/src/net/ssl/password.cpp
    ${net_dir}/src/net/ssl/startup.cpp
    ${net_dir}/src/net/ssl/tls.cpp
    ${net_dir}/src/net/ssl/transport.cpp
    ${net_dir}/src/net/ssl/verify.cpp
    ${net_dir}/src/net/stream_oriented.cpp
    ${net_dir}/src/net/stream_socket.cpp
    ${net_dir}/src/net/stream_transport.cpp
    ${net_dir}/src/net/tcp_accept_socket.cpp
    ${net_dir}/src/net/tcp_stream_socket.cpp
    ${net_dir}/src/net/this_host.cpp
    ${net_dir}/src/net/udp_datagram_socket.cpp
    ${net_dir}/src/net/web_socket/client.cpp
    ${net_dir}/src/net/web_socket/connect.cpp
    ${net_dir}/src/net/web_socket/default_trait.cpp
    ${net_dir}/src/net/web_socket/frame.cpp
    ${net_dir}/src/net/web_socket/framing.cpp
    ${net_dir}/src/net/web_socket/handshake.cpp
    ${net_dir}/src/net/web_socket/lower_layer.cpp
    ${net_dir}/src/net/web_socket/server.cpp
    ${net_dir}/src/net/web_socket/upper_layer.cpp
    )

rendu_add_library(
    DIR
    ${net_dir}
    PROJECT
    ${PROJECT_NAME}
    NAME
    caf_net
    HDRS
    ${caf_net_headers}
    SRCS
    ${caf_net_files}
    SETTING
    rendu-dependency-interface
    DEPS
    dep::caf_core
    dep::openssl
    LINKOPTS
)

