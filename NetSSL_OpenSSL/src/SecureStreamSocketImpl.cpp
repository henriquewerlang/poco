//
// SecureStreamSocketImpl.cpp
//
// $Id: //poco/1.3/NetSSL_OpenSSL/src/SecureStreamSocketImpl.cpp#12 $
//
// Library: NetSSL_OpenSSL
// Package: SSLSockets
// Module:  SecureStreamSocketImpl
//
// Copyright (c) 2006-2010, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/SecureStreamSocketImpl.h"
#include "Poco/Net/SSLException.h"
#include "Poco/Thread.h"


namespace Poco {
namespace Net {


SecureStreamSocketImpl::SecureStreamSocketImpl(Context::Ptr pContext):
	_impl(new StreamSocketImpl, pContext),
	_lazyHandshake(false)
{
}


SecureStreamSocketImpl::SecureStreamSocketImpl(StreamSocketImpl* pStreamSocket, Context::Ptr pContext):
	_impl(pStreamSocket, pContext),
	_lazyHandshake(false)
{
	pStreamSocket->duplicate();
	reset(_impl.sockfd());
}


SecureStreamSocketImpl::~SecureStreamSocketImpl()
{
	reset();
}


SocketImpl* SecureStreamSocketImpl::acceptConnection(SocketAddress& clientAddr)
{
	throw Poco::InvalidAccessException("Cannot acceptConnection() on a SecureStreamSocketImpl");
}


void SecureStreamSocketImpl::acceptSSL()
{
	_impl.acceptSSL();
}


void SecureStreamSocketImpl::connect(const SocketAddress& address)
{
	_impl.connect(address, !_lazyHandshake);
	reset(_impl.sockfd());
}


void SecureStreamSocketImpl::connect(const SocketAddress& address, const Poco::Timespan& timeout)
{
	_impl.connect(address, timeout, !_lazyHandshake);
	reset(_impl.sockfd());
}
	

void SecureStreamSocketImpl::connectNB(const SocketAddress& address)
{
	_impl.connectNB(address);
	reset(_impl.sockfd());
}


void SecureStreamSocketImpl::connectSSL()
{
	_impl.connectSSL(!_lazyHandshake);
}
	

void SecureStreamSocketImpl::bind(const SocketAddress& address, bool reuseAddress)
{
	throw Poco::InvalidAccessException("Cannot bind() a SecureStreamSocketImpl");
}

	
void SecureStreamSocketImpl::listen(int backlog)
{
	throw Poco::InvalidAccessException("Cannot listen() on a SecureStreamSocketImpl");
}
	

void SecureStreamSocketImpl::close()
{
	reset();
	_impl.close();
}
	

int SecureStreamSocketImpl::sendBytes(const void* buffer, int length, int flags)
{
	const char* p = reinterpret_cast<const char*>(buffer);
	int remaining = length;
	int sent = 0;
	bool blocking = getBlocking();
	while (remaining > 0)
	{
		int n = _impl.sendBytes(p, remaining, flags);
		if (n < 0 && !blocking) return n;
		p += n; 
		sent += n;
		remaining -= n;
		if (blocking && remaining > 0)
			Poco::Thread::yield();
		else
			break;
	}
	return sent;
}


int SecureStreamSocketImpl::receiveBytes(void* buffer, int length, int flags)
{
	return _impl.receiveBytes(buffer, length, flags);
}


int SecureStreamSocketImpl::sendTo(const void* buffer, int length, const SocketAddress& address, int flags)
{
	throw Poco::InvalidAccessException("Cannot sendTo() on a SecureStreamSocketImpl");
}


int SecureStreamSocketImpl::receiveFrom(void* buffer, int length, SocketAddress& address, int flags)
{
	throw Poco::InvalidAccessException("Cannot receiveFrom() on a SecureStreamSocketImpl");
}


void SecureStreamSocketImpl::sendUrgent(unsigned char data)
{
	throw Poco::InvalidAccessException("Cannot sendUrgent() on a SecureStreamSocketImpl");
}


int SecureStreamSocketImpl::available()
{
	return _impl.available();
}


void SecureStreamSocketImpl::shutdownReceive()
{
}

	
void SecureStreamSocketImpl::shutdownSend()
{
}

	
void SecureStreamSocketImpl::shutdown()
{
	_impl.shutdown();
}


bool SecureStreamSocketImpl::secure() const
{
	return true;
}


bool SecureStreamSocketImpl::havePeerCertificate() const
{
	X509* pCert = _impl.peerCertificate();
	if (pCert)
	{
		X509_free(pCert);
		return true;
	}
	else return false;
}


X509Certificate SecureStreamSocketImpl::peerCertificate() const
{
	X509* pCert = _impl.peerCertificate();
	if (pCert)
		return X509Certificate(pCert);
	else
		throw SSLException("No certificate available");
}


void SecureStreamSocketImpl::setLazyHandshake(bool flag)
{
	_lazyHandshake = flag;
}

	
bool SecureStreamSocketImpl::getLazyHandshake() const
{
	return _lazyHandshake;
}


void SecureStreamSocketImpl::verifyPeerCertificate()
{
	_impl.verifyPeerCertificate();
}


void SecureStreamSocketImpl::verifyPeerCertificate(const std::string& hostName)
{
	_impl.verifyPeerCertificate(hostName);
}


int SecureStreamSocketImpl::completeHandshake()
{
	return _impl.completeHandshake();
}


} } // namespace Poco::Net
