
#include <yas/detail/tools/buffers.hpp>

#include <yarmi/global_context_base.hpp>
#include <yarmi/session_base.hpp>

#include <stdexcept>
#include <sstream>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace yarmi {

/***************************************************************************/

struct session_wrapper {
	session_base *session;
	std::uint64_t id;

	struct by_session;
	struct by_id;
};

struct global_context_base::pimpl {
	pimpl()
		:session_ids(0)
		,sessions()
		,recv_bytes(0)
		,sent_bytes(0)
		,requests(0)
		,replies(0)
	{}

	std::uint64_t session_ids;

	typedef boost::multi_index::multi_index_container<
		 session_wrapper
		,boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<
				 boost::multi_index::tag<session_wrapper::by_session>
				,boost::multi_index::member<
					session_wrapper, session_base*, &session_wrapper::session
				>
			>
			,boost::multi_index::ordered_unique<
				 boost::multi_index::tag<session_wrapper::by_id>
				,boost::multi_index::member<
					session_wrapper, std::uint64_t, &session_wrapper::id
				>
			>
		>
	> sessions_cont;
	sessions_cont sessions;

	std::uint64_t recv_bytes;
	std::uint64_t sent_bytes;
	std::uint64_t requests;
	std::uint64_t replies;
}; // struct pimpl

/***************************************************************************/

global_context_base::global_context_base()
	:impl(new pimpl)
{}

global_context_base::~global_context_base() {
	delete impl;
}

global_context_base &global_context_base::get_global_context_base() { return *this; }
const global_context_base &global_context_base::get_global_context_base() const { return *this; }

/***************************************************************************/

std::uint64_t global_context_base::add_session(session_base *session) {
	if ( has_session(session) ) {
		std::ostringstream os;
		os << "session 0x" << std::hex << session << " already in sessions list";
		throw std::runtime_error(os.str());
	}

	std::uint64_t idx = impl->session_ids++;
	impl->sessions.insert(session_wrapper{session, idx});

	return idx;
}

void global_context_base::del_session(session_base *session) {
	if ( !has_session(session) ) {
		std::ostringstream os;
		os << "session 0x" << std::hex << session << " not in sessions list";
		throw std::runtime_error(os.str());
	}

	auto &index = impl->sessions.get<session_wrapper::by_session>();
	auto it = index.find(session);
	index.erase(it);
}

void global_context_base::del_session(std::uint64_t id) {
	if ( !has_session(id) ) {
		std::ostringstream os;
		os << "session with ID " << id << " not in sessions list";
		throw std::runtime_error(os.str());
	}

	auto &index = impl->sessions.get<session_wrapper::by_id>();
	auto it = index.find(id);
	index.erase(it);
}

/***************************************************************************/

void global_context_base::set_id(session_base *session, std::uint64_t id) {
	auto &index = impl->sessions.get<session_wrapper::by_session>();

	auto it = index.find(session);
	if ( it == index.end() ) {
		std::ostringstream os;
		os << "session 0x" << std::hex << session << " not in sessions list";
		throw std::runtime_error(os.str());
	}

	index.erase(it);

	impl->sessions.insert(session_wrapper{session, id});
}

/***************************************************************************/

bool global_context_base::has_session(session_base *session) const {
	const auto &index = impl->sessions.get<session_wrapper::by_session>();
	return index.find(session) != index.end();
}

bool global_context_base::has_session(std::uint64_t id) const {
	const auto &index = impl->sessions.get<session_wrapper::by_id>();
	return index.find(id) != index.end();
}

/***************************************************************************/

std::size_t global_context_base::sessions() const {
	return impl->sessions.size();
}

/***************************************************************************/

void global_context_base::send_to_all(const std::shared_ptr<char> &buffer, std::size_t size) {
	for ( const auto &it: impl->sessions ) {
		it.session->send(buffer, size);
	}
}

/***************************************************************************/

void global_context_base::inc_recv_bytes(std::uint64_t bytes) { impl->recv_bytes += bytes; }
void global_context_base::inc_sent_bytes(std::uint64_t bytes) { impl->sent_bytes += bytes; }
std::uint64_t global_context_base::recv_bytes() const { return impl->recv_bytes; }
std::uint64_t global_context_base::sent_bytes() const { return impl->sent_bytes; }

void global_context_base::inc_requests() { ++(impl->requests); }
void global_context_base::inc_replies() { ++(impl->replies); }
std::uint64_t global_context_base::requests() const { return impl->requests; }
std::uint64_t global_context_base::replies() const { return impl->replies; }

/***************************************************************************/

} // ns yarmi