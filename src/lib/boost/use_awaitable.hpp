// Custom boost include file update to fix duplicate symbols of
// asio::dummy_return<void> encountered for MSVC for v1.75, see:
// https://github.com/chriskohlhoff/asio/pull/584

// ReSharper disable CppUnusedIncludeDirective

//
// impl/use_awaitable.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IMPL_USE_AWAITABLE_HPP
#define BOOST_ASIO_IMPL_USE_AWAITABLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

// -- begin custom include prerequisites --
#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/basic_raw_socket.hpp>
#include <boost/asio/basic_seq_packet_socket.hpp>
#include <boost/asio/basic_serial_port.hpp>
#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/basic_socket_iostream.hpp>
#include <boost/asio/basic_socket_streambuf.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/basic_streambuf.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffered_read_stream_fwd.hpp>
#include <boost/asio/buffered_read_stream.hpp>
#include <boost/asio/buffered_stream_fwd.hpp>
#include <boost/asio/buffered_stream.hpp>
#include <boost/asio/buffered_write_stream_fwd.hpp>
#include <boost/asio/buffered_write_stream.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/defer.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/execution.hpp>
#include <boost/asio/execution/allocator.hpp>
#include <boost/asio/execution/any_executor.hpp>
#include <boost/asio/execution/blocking.hpp>
#include <boost/asio/execution/blocking_adaptation.hpp>
#include <boost/asio/execution/bulk_execute.hpp>
#include <boost/asio/execution/bulk_guarantee.hpp>
#include <boost/asio/execution/connect.hpp>
#include <boost/asio/execution/context.hpp>
#include <boost/asio/execution/context_as.hpp>
#include <boost/asio/execution/execute.hpp>
#include <boost/asio/execution/executor.hpp>
#include <boost/asio/execution/invocable_archetype.hpp>
#include <boost/asio/execution/mapping.hpp>
#include <boost/asio/execution/occupancy.hpp>
#include <boost/asio/execution/operation_state.hpp>
#include <boost/asio/execution/outstanding_work.hpp>
#include <boost/asio/execution/prefer_only.hpp>
#include <boost/asio/execution/receiver.hpp>
#include <boost/asio/execution/receiver_invocation_error.hpp>
#include <boost/asio/execution/relationship.hpp>
#include <boost/asio/execution/schedule.hpp>
#include <boost/asio/execution/scheduler.hpp>
#include <boost/asio/execution/sender.hpp>
#include <boost/asio/execution/set_done.hpp>
#include <boost/asio/execution/set_error.hpp>
#include <boost/asio/execution/set_value.hpp>
#include <boost/asio/execution/start.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/generic/basic_endpoint.hpp>
#include <boost/asio/generic/datagram_protocol.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/asio/generic/seq_packet_protocol.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/handler_continuation_hook.hpp>
#include <boost/asio/handler_invoke_hook.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/io_service_strand.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v4_iterator.hpp>
#include <boost/asio/ip/address_v4_range.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/ip/address_v6_iterator.hpp>
#include <boost/asio/ip/address_v6_range.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/asio/ip/network_v6.hpp>
#include <boost/asio/ip/bad_address_cast.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_entry.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/icmp.hpp>
#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/resolver_base.hpp>
#include <boost/asio/ip/resolver_query_base.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/unicast.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/is_applicable_property.hpp>
#include <boost/asio/is_executor.hpp>
#include <boost/asio/is_read_buffered.hpp>
#include <boost/asio/is_write_buffered.hpp>
#include <boost/asio/local/basic_endpoint.hpp>
#include <boost/asio/local/connect_pair.hpp>
#include <boost/asio/local/datagram_protocol.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/multiple_exceptions.hpp>
#include <boost/asio/packaged_task.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/posix/basic_descriptor.hpp>
#include <boost/asio/posix/basic_stream_descriptor.hpp>
#include <boost/asio/posix/descriptor.hpp>
#include <boost/asio/posix/descriptor_base.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/prefer.hpp>
#include <boost/asio/query.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_at.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/require.hpp>
#include <boost/asio/require_concept.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/serial_port_base.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/asio/static_thread_pool.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/system_context.hpp>
#include <boost/asio/system_executor.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/time_traits.hpp>
// -- end custom include prerequisites --

#include <boost/asio/detail/config.hpp>
#include <boost/asio/async_result.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {

template <typename Executor, typename T>
class awaitable_handler_base
  : public awaitable_thread<Executor>
{
public:
  typedef void result_type;
  typedef awaitable<T, Executor> awaitable_type;

  // Construct from the entry point of a new thread of execution.
  awaitable_handler_base(awaitable<void, Executor> a, const Executor& ex)
    : awaitable_thread<Executor>(std::move(a), ex)
  {
  }

  // Transfer ownership from another awaitable_thread.
  explicit awaitable_handler_base(awaitable_thread<Executor>* h)
    : awaitable_thread<Executor>(std::move(*h))
  {
  }

protected:
  awaitable_frame<T, Executor>* frame() noexcept
  {
    return static_cast<awaitable_frame<T, Executor>*>(this->top_of_stack_);
  }
};

template <typename, typename...>
class awaitable_handler;

template <typename Executor>
class awaitable_handler<Executor>
  : public awaitable_handler_base<Executor, void>
{
public:
  using awaitable_handler_base<Executor, void>::awaitable_handler_base;

  void operator()()
  {
    this->frame()->attach_thread(this);
    this->frame()->return_void();
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor>
class awaitable_handler<Executor, boost::system::error_code>
  : public awaitable_handler_base<Executor, void>
{
public:
  using awaitable_handler_base<Executor, void>::awaitable_handler_base;

  void operator()(const boost::system::error_code& ec)
  {
    this->frame()->attach_thread(this);
    if (ec)
      this->frame()->set_error(ec);
    else
      this->frame()->return_void();
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor>
class awaitable_handler<Executor, std::exception_ptr>
  : public awaitable_handler_base<Executor, void>
{
public:
  using awaitable_handler_base<Executor, void>::awaitable_handler_base;

  void operator()(std::exception_ptr ex)
  {
    this->frame()->attach_thread(this);
    if (ex)
      this->frame()->set_except(ex);
    else
      this->frame()->return_void();
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename T>
class awaitable_handler<Executor, T>
  : public awaitable_handler_base<Executor, T>
{
public:
  using awaitable_handler_base<Executor, T>::awaitable_handler_base;

  template <typename Arg>
  void operator()(Arg&& arg)
  {
    this->frame()->attach_thread(this);
    this->frame()->return_value(std::forward<Arg>(arg));
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename T>
class awaitable_handler<Executor, boost::system::error_code, T>
  : public awaitable_handler_base<Executor, T>
{
public:
  using awaitable_handler_base<Executor, T>::awaitable_handler_base;

  template <typename Arg>
  void operator()(const boost::system::error_code& ec, Arg&& arg)
  {
    this->frame()->attach_thread(this);
    if (ec)
      this->frame()->set_error(ec);
    else
      this->frame()->return_value(std::forward<Arg>(arg));
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename T>
class awaitable_handler<Executor, std::exception_ptr, T>
  : public awaitable_handler_base<Executor, T>
{
public:
  using awaitable_handler_base<Executor, T>::awaitable_handler_base;

  template <typename Arg>
  void operator()(std::exception_ptr ex, Arg&& arg)
  {
    this->frame()->attach_thread(this);
    if (ex)
      this->frame()->set_except(ex);
    else
      this->frame()->return_value(std::forward<Arg>(arg));
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename... Ts>
class awaitable_handler
  : public awaitable_handler_base<Executor, std::tuple<Ts...>>
{
public:
  using awaitable_handler_base<Executor,
    std::tuple<Ts...>>::awaitable_handler_base;

  template <typename... Args>
  void operator()(Args&&... args)
  {
    this->frame()->attach_thread(this);
    this->frame()->return_values(std::forward<Args>(args)...);
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename... Ts>
class awaitable_handler<Executor, boost::system::error_code, Ts...>
  : public awaitable_handler_base<Executor, std::tuple<Ts...>>
{
public:
  using awaitable_handler_base<Executor,
    std::tuple<Ts...>>::awaitable_handler_base;

  template <typename... Args>
  void operator()(const boost::system::error_code& ec, Args&&... args)
  {
    this->frame()->attach_thread(this);
    if (ec)
      this->frame()->set_error(ec);
    else
      this->frame()->return_values(std::forward<Args>(args)...);
    this->frame()->pop_frame();
    this->pump();
  }
};

template <typename Executor, typename... Ts>
class awaitable_handler<Executor, std::exception_ptr, Ts...>
  : public awaitable_handler_base<Executor, std::tuple<Ts...>>
{
public:
  using awaitable_handler_base<Executor,
    std::tuple<Ts...>>::awaitable_handler_base;

  template <typename... Args>
  void operator()(std::exception_ptr ex, Args&&... args)
  {
    this->frame()->attach_thread(this);
    if (ex)
      this->frame()->set_except(ex);
    else
      this->frame()->return_values(std::forward<Args>(args)...);
    this->frame()->pop_frame();
    this->pump();
  }
};

} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

#if defined(_MSC_VER)
template <typename T>
T dummy_return()
{
  return std::move(*static_cast<T*>(nullptr));
}

template <>
inline void dummy_return() // <-- "inline" fixes duplicate symbols at link time
{
}
#endif // defined(_MSC_VER)

template <typename Executor, typename R, typename... Args>
class async_result<use_awaitable_t<Executor>, R(Args...)>
{
public:
  typedef typename detail::awaitable_handler<
      Executor, typename decay<Args>::type...> handler_type;
  typedef typename handler_type::awaitable_type return_type;

  template <typename Initiation, typename... InitArgs>
  static return_type initiate(Initiation initiation,
      use_awaitable_t<Executor> u, InitArgs... args)
  {
    (void)u;

    co_await [&](auto* frame)
      {
        BOOST_ASIO_HANDLER_LOCATION((u.file_name_, u.line_, u.function_name_));
        handler_type handler(frame->detach_thread());
        std::move(initiation)(std::move(handler), std::move(args)...);
        return static_cast<handler_type*>(nullptr);
      };

    for (;;) {} // Never reached.
#if defined(_MSC_VER)
    co_return dummy_return<typename return_type::value_type>();
#endif // defined(_MSC_VER)
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IMPL_USE_AWAITABLE_HPP
