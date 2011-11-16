#include "stdafx.h"
#include "lwTinyFrame/lwHTTPClient.h"
#include "lwTinyFrame/lwUtil.h"

namespace lw{

	void HttpMsgBuf::write(const char* p, int size){
		if ( _size + size > _buffSize ){
			while ( _buffSize < _size + size ){
				_buffSize *= 2;
			}
			char* buff0 = _buff;
			_buff = new char[_buffSize];
			memcpy(_buff, buff0, _size);
			delete [] buff0;
			_pRead = _buff;
		}
		memcpy(_buff+_size, p, size);
		_size += size;
	}
	void HttpMsgBuf::writeChar(char c){
		write((char*)&c, sizeof(char));
	}
	void HttpMsgBuf::writeShort(short s){
		write((char*)&s, sizeof(short));
	}
	void HttpMsgBuf::writeInt(int n){
		write((char*)&n, sizeof(int));
	}
	void HttpMsgBuf::writeFloat(float f){
		write((char*)&f, sizeof(float));
	}
	void HttpMsgBuf::writeString(const wchar_t* str){
		W2UTF8 str8(str);
		writeInt(str8.size());
		write(str8.data(), str8.size());
	}
	void HttpMsgBuf::writeUtf8(const char* str){
		int sz = (int)strlen(str);
		writeInt(sz);
		write(str, sz);
	}
	void HttpMsgBuf::writeBlob(const char* p, int size){
		writeInt(size);
		write(p, size);
	}
	void HttpMsgBuf::readReset(){
		_pRead = _buff;
	}
	char HttpMsgBuf::readChar(){
		if ( _pRead + sizeof(char) > _buff + _size ){
			lwerror("out of bound");
			return 0;
		}
		char* p = (char*)_pRead;
		_pRead += sizeof(char);
		return *p;
	}
	short HttpMsgBuf::readShort(){
		if ( _pRead + sizeof(short) > _buff + _size ){
			lwerror("out of bound");
			return 0;
		}
		short* p = (short*)_pRead;
		_pRead += sizeof(short);
		return *p;
	}

	int HttpMsgBuf::readInt(){
		if ( _pRead + sizeof(int) > _buff + _size ){
			lwerror("out of bound");
			return 0;
		}
		int* p = (int*)_pRead;
		_pRead += sizeof(int);
		return *p;
	}
	float HttpMsgBuf::readFloat(){
		if ( _pRead + sizeof(int) > _buff + _size ){
			lwerror("out of bound");
			return 0;
		}
		float* p = (float*)_pRead;
		_pRead += sizeof(float);
		return *p;
	}
	const char* HttpMsgBuf::readString(){
		int len = (int)strlen(_pRead);
		if ( _pRead + len >= _buff + _size ){
			lwerror("out of bound");
			return NULL;
		}
		char* p = (char*)_pRead;
		_pRead += len + 1;
		return p;
	}
	void HttpMsgBuf::readBlob(const char*& pData, int& dataSize){
		dataSize = readInt();
		if ( dataSize < 0 ){
			lwerror("size error");
			return;
		}
		if ( _pRead + dataSize > _buff + _size ){
			lwerror("out of bound");
			return;
		}
		pData = (char*)_pRead;
		_pRead += dataSize;
	}


	static void CALLBACK requestCallback (
											HINTERNET hInternet,
											DWORD_PTR dwContext,
											DWORD dwInternetStatus,
											LPVOID lpvStatusInformation,
											DWORD dwStatusInformationLength
											){
		lw::Locker l(CriticalSection::getGlobal());
		HTTPMsg* pMsg = (HTTPMsg*) dwContext;
		switch(dwInternetStatus)
		{
		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
			//lwinfo("WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE");
			//pMsg->getClient()->onMsgComplete(pMsg);
			pMsg->onSendComplete();
			break;
		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
			//lwinfo("WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE");
			pMsg->onHeadersAvailable();
			break;
		case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
			//lwinfo("WINHTTP_CALLBACK_STATUS_READ_COMPLETE");
			pMsg->onReadComplete(dwStatusInformationLength);
			break;
		case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
			//lwinfo("WINHTTP_CALLBACK_STATUS_REQUEST_ERROR");
			lwerror( "Error " << GetLastError( ) << " has occurred");
			pMsg->getClient()->deleteMsg(pMsg);
			break;
		case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
			//lwinfo("WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING");
			break;

		default:
			;
		}
	}

	HTTPMsg::HTTPMsg(const wchar_t* objName, HTTPClient* pClient)
	:_pClient(pClient), _buff(BUFF_SIZE), _hRequest(0){
		lwassert(objName);
		_objName = objName;
	}

	HTTPMsg::~HTTPMsg(){
		if( _hRequest ){
			WinHttpSetStatusCallback( _hRequest,
				NULL,
				WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,
				NULL );
			WinHttpCloseHandle( _hRequest );
		}
	}

	void HTTPMsg::send(){
		_pClient->sendMsg(this);
	}

	void HTTPMsg::onSendComplete(){
		_buff.reset();
		WinHttpReceiveResponse(_hRequest, NULL);
	}

	void HTTPMsg::onHeadersAvailable(){
		DWORD dwStatusCode = 0;
		DWORD dwSize = sizeof(DWORD);

		if (::WinHttpQueryHeaders(_hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
									NULL, &dwStatusCode, &dwSize, NULL) == FALSE){
			lwerror("WinHttpQueryHeaders failed");
			_pClient->deleteMsg(this);
			return;
		}

		if ((dwStatusCode == HTTP_STATUS_DENIED) || (dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)){
			lwerror("the proxy or server requires authentication");
			_pClient->deleteMsg(this);
			return;
		}

		if (dwStatusCode != HTTP_STATUS_OK){
			lwerror("failed to fetch");
			_pClient->deleteMsg(this);
			return;
		}

		DWORD dwContentLength = 0;
		DWORD dwCLSize = sizeof(DWORD);

		//if (::WinHttpQueryHeaders(_hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, 
		//							NULL, &dwContentLength, &dwCLSize, NULL) == FALSE)
		//{
		//	lwerror("WinHttpQueryHeaders failed");
		//	_pClient->deleteMsg(this);
		//	return;
		//}
		//else
		//{
		//	_contentLength = dwContentLength;
		//	_recvBuff = new char[_contentLength];
		//	_pRecv = _recvBuff;
		//}


		if (::WinHttpReadData(_hRequest,
			_recvBuffTemp,
			BUFF_SIZE,
			NULL) == FALSE)
		{
			lwerror("failed to read data");
			_pClient->deleteMsg(this);
			return;
		}
	}

	void HTTPMsg::onReadComplete(int bytesRead){
		if (bytesRead == 0){
			_pClient->addToRespond(this);
			return;
		}
		else{
			_buff.write(_recvBuffTemp, bytesRead);
			if (::WinHttpReadData(_hRequest,
				_recvBuffTemp,
				BUFF_SIZE,
				NULL) == FALSE)
			{
				lwerror("failed to read data");
				_pClient->deleteMsg(this);
				return;
			}
		}
	}

	HTTPClient::HTTPClient(const wchar_t* serverName, unsigned short port)
	:_isConnected(false), _hSession(0), _hConnect(0)
	{
		_serverName = serverName;
		_port = port;
		_hSession = WinHttpOpen( L"lwHTTPClient",  
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME, 
			WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC );

		if( !_hSession ){
			lwerror("WinHttpOpen failed");
			return;
		}
		if ( !connect() ){
			lwerror("connect failed");
			return;
		}
	}

	HTTPClient::~HTTPClient(){
		lw::Locker l(CriticalSection::getGlobal());
		std::vector<HTTPMsg*>::iterator it = _msgs.begin();
		std::vector<HTTPMsg*>::iterator itEnd = _msgs.end();
		for ( ; it != itEnd; ++it ){
			delete *it;
		}
		_msgs.clear();
		if( _hConnect ){
			WinHttpCloseHandle( _hConnect );
		}
		if( _hSession ){
			WinHttpCloseHandle( _hSession );
		}
	}

	bool HTTPClient::connect(){
		if( _hConnect ){
			WinHttpCloseHandle( _hConnect );
		}
		_hConnect = WinHttpConnect( _hSession, _serverName.c_str(), _port, 0 );
		if ( !_hConnect ){
			lwerror("WinHttpConnect failed");
			_isConnected = false;
			return false;
		}
		_isConnected = true;
		return true;
	}

	void HTTPClient::sendMsg(HTTPMsg* pMsg){
		lwassert(pMsg);
		if ( !_isConnected ){
			connect();
		}
		if ( !_isConnected ){
			return;
		}
		
		HINTERNET hRequest = WinHttpOpenRequest( _hConnect, L"POST", pMsg->getObjName(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES, 
			0 );
		pMsg->setHRequest(hRequest);

		//WinHttpSetOption(hRequest,
		//	WINHTTP_OPTION_CONTEXT_VALUE,
		//	pMsg,
		//	sizeof(pMsg));

		WinHttpSetStatusCallback(hRequest,
			requestCallback,
			WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS |
			WINHTTP_CALLBACK_FLAG_HANDLES,   // to listen to the HANDLE_CLOSING event
			0);

		DWORD size = pMsg->getBuff()->getSize();
		bool bResults = WinHttpSendRequest( hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			pMsg->getBuff()->getBuff(), size, 
			size, (DWORD_PTR)pMsg ) != 0;
		if ( bResults ){
			_msgs.push_back(pMsg);
		}else{
			delete pMsg;
		}
	}

	void HTTPClient::deleteMsg(HTTPMsg* pMsg){
		lw::Locker l(CriticalSection::getGlobal());
		std::vector<HTTPMsg*>::iterator it = _msgs.begin();
		std::vector<HTTPMsg*>::iterator itEnd = _msgs.end();
		for ( ; it != itEnd; ++it ){
			if ( *it == pMsg ){
				delete pMsg;
				_msgs.erase(it);
				return;
			}
		}
	}

	void HTTPClient::addToRespond(HTTPMsg* pMsg){
		lw::Locker l(CriticalSection::getGlobal());
		_respondMsgs.push_back(pMsg);
	}

	void HTTPClient::main(){
		{
			lw::Locker l(CriticalSection::getGlobal());
			std::vector<HTTPMsg*>::iterator it = _respondMsgs.begin();
			std::vector<HTTPMsg*>::iterator itEnd = _respondMsgs.end();
			for ( ; it != itEnd; ++it ){
				(*it)->getBuff()->readReset();
				(*it)->onRespond();
				deleteMsg(*it);
			}
			_respondMsgs.clear();
		}
	}

	HTTPClientTask::HTTPClientTask(const wchar_t* URL, unsigned short port)
	:_URL(URL), _port(port){
		
	}

	HTTPClientTask::~HTTPClientTask(){
		
	}

	void HTTPClientTask::vBegin(){
		_pHTTPClient = new lw::HTTPClient(_URL.c_str(), _port);
	}

	void HTTPClientTask::vEnd(){
		delete _pHTTPClient;
	}

	void HTTPClientTask::vMain(float dt){
		_pHTTPClient->main();
	}

} //namespace lw