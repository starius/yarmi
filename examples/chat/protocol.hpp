
// Copyright (c) 2013, niXman
// All rights reserved.
//
// This file is part of YARMI(https://github.com/niXman/yarmi) project.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
//
//   Neither the name of the {organization} nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _yarmi__chat__protocol_hpp
#define _yarmi__chat__protocol_hpp

#include <yarmi/yarmi.hpp>

YARMI_CONSTRUCT(
	client_invoker, // name of the client invoker
	((registration,
		((std::string, std::string)) // message : registration key
	))
	((activation,
		((std::string)) // message
	))
	((login,
		((std::string)) // message
	))
	((logout,
		((std::string)) // message
	))
	((users_online,
		((std::vector<std::string>)) // without args
	))
	,
	server_invoker, // name of the server invoker
	((registration,
		((std::string)) // username
	))
	((activation,
		((std::string, std::string, std::string)) // registration key : username : password
	))
	((login,
		((std::string, std::string)) // username : password
	))
	((logout,
		(()) // without args
	))
	((users_online,
		(()) // without args
	))
);

#endif // _yarmi__chat__protocol_hpp
