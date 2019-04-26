#pragma once

#include <reactor/listener.hxx>
#include <reactor/session.hxx>


#include <basic_router.hxx>
#include <out.hxx>

using namespace _0xdead4ead;

template<class Body>
auto make_response(const boost::beast::http::request<Body>& req,
                   const typename Body::value_type& user_body){

    typename Body::value_type body(user_body);

    auto const body_size = body.size();

    boost::beast::http::response<Body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(boost::beast::http::status::ok, req.version())};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.content_length(body_size);
    res.keep_alive(req.keep_alive());

    return res;
}
