#include "stdafx.h"
#include "lwHTTPClient.h"
#include "lwUtil.h"
#include <sstream>

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
		if ( _pRead + sizeof(float) > _buff + _size ){
			lwerror("out of bound"<<_size);
			return 0;
		}
		float* p = (float*)_pRead;
		_pRead += sizeof(float);
		return *p;
	}
	const char* HttpMsgBuf::readString(){
		int len = (int)strlen(_pRead);
		if ( _pRead + len > _buff + _size ){
			lwerror("out of bound");
			return NULL;
		}
		char* p = (char*)_pRead;
		_pRead += len + 1;
		return p;
	}
	void HttpMsgBuf::readBlob(const char*& pData, int& dataSize){
		dataSize = readInt();
		if ( dataSize <= 0 ){
			lwerror("size error");
			return;
		}
		if ( _pRead + dataSize >= _buff + _size ){
			lwerror("out of bound");
			return;
		}
		pData = (char*)_pRead;
		_pRead += dataSize;
	}
	
	void* CFClientRetain(void* selfPtr){
		HTTPMsg* object	= (HTTPMsg *) selfPtr;
		return object;
	}

	void CFClientRelease(void* selfPtr){
		
	}
	
	CFStringRef CFClientDescribeCopy(void* selfPtr){
		return NULL;
	}
	
	static CFStreamClientContext sContext = {
		0, nil,
		NULL,
		NULL,
		NULL
	};
	
	void HTTPReadCallback(CFReadStreamRef stream, CFStreamEventType type, void* userData){
		HTTPMsg* pMsg = (HTTPMsg*) userData;
		switch (type) {
			case kCFStreamEventHasBytesAvailable: {
				UInt8 buf[HTTPMsg::BUFF_SIZE];
				CFIndex bytesRead = CFReadStreamRead(stream, buf, HTTPMsg::BUFF_SIZE);
					if (bytesRead > 0) {
						pMsg->getBuff()->write((char*)(buf), bytesRead);
					}
				}
				break;
			case kCFStreamEventErrorOccurred:
				pMsg->getClient()->deleteMsg(pMsg);
				lwerror("kCFStreamEventEndEncountered");
				break;
			case kCFStreamEventEndEncountered:
				pMsg->getClient()->addToRespond(pMsg);
				break;
			default:
				break;
		}
	}


	HTTPMsg::HTTPMsg(const wchar_t* objName, HTTPClient* pClient)
	:_pClient(pClient), _buff(BUFF_SIZE), _readStream(NULL){
		lwassert(objName);
		_objName = objName;
		_cfContext = sContext;
		_cfContext.info = this;
	}

	HTTPMsg::~HTTPMsg(){
		if (_readStream) {
			//   Close the read stream.
			CFReadStreamClose(_readStream);
			//   Deregister the callback client (learned this from WWDC session 805)
			CFReadStreamSetClient(_readStream, 0, NULL, NULL);
			//   Take the stream out of the run loop
			CFReadStreamUnscheduleFromRunLoop(
											  _readStream,
											  CFRunLoopGetCurrent(),
											  kCFRunLoopCommonModes);
			//   Deallocate the stream pointer
			CFRelease(_readStream);
			//   Throw the spent pointer away
			_readStream = NULL;
		}
	}

	void HTTPMsg::onSendComplete(){
		
	}

	void HTTPMsg::onHeadersAvailable(){
		
	}

	void HTTPMsg::onReadComplete(int bytesRead){
		
	}
	void HTTPMsg::send(){
		_pClient->sendMsg(this);
	}
    
    HTTPClient::HTTPClient(const wchar_t* addr)
	{
        W2UTF8 str8(addr);
		_strUrl = str8.data();
	}

	HTTPClient::HTTPClient(const wchar_t* serverName, unsigned short port)
	{
		std::wstringstream ss;
		ss << L"http://" << serverName << ":" << port << L"/";
		W2UTF8 str8(ss.str().c_str());
		_strUrl = str8.data();
	}

	HTTPClient::~HTTPClient(){
		std::vector<HTTPMsg*>::iterator it = _msgs.begin();
		std::vector<HTTPMsg*>::iterator itEnd = _msgs.end();
		for ( ; it != itEnd; ++it ){
			delete *it;
		}
		_msgs.clear();
		
	}

	bool HTTPClient::connect(){
		return false;
	}

	void HTTPClient::sendMsg(HTTPMsg* pMsg){
		std::stringstream ss;
		W2UTF8 objName(pMsg->getObjName());
		ss << _strUrl.c_str() << objName.data();
		//CFStringRef url = CFSTR(ss.str().c_str());
		CFStringRef url = CFStringCreateWithCString(kCFAllocatorDefault, ss.str().c_str(), kCFStringEncodingASCII);
		//CFStringRef url = CFSTR("http://localhost:8080/lwDownload");
		CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, url, NULL);
		CFStringRef requestMethod = CFSTR("POST");
		CFHTTPMessageRef myRequest = CFHTTPMessageCreateRequest(kCFAllocatorDefault, requestMethod, myURL, kCFHTTPVersion1_1);
		
		CFDataRef bodyData = CFDataCreate(kCFAllocatorDefault, (UInt8*)(pMsg->getBuff()->getBuff()), pMsg->getBuff()->getSize());
		pMsg->getBuff()->reset();
		CFHTTPMessageSetBody(myRequest, bodyData);
		
		CFReadStreamRef readStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, myRequest);
		
		pMsg->setReadStream(readStream);
		
		CFRelease(url);
		CFRelease(myURL);
		CFRelease(myRequest);
		CFRelease(bodyData);
		
		bool bResults = CFReadStreamSetClient(readStream,
												kCFStreamEventHasBytesAvailable |
												kCFStreamEventErrorOccurred |
												kCFStreamEventEndEncountered,
												HTTPReadCallback,
												pMsg->getContext());
		if ( bResults ){
			CFReadStreamScheduleWithRunLoop(readStream, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
			bResults = CFReadStreamOpen(readStream);
			if ( !bResults ){
				lwerror("CFReadStreamOpen failed");
			}
			
		}else{
			lwerror("CFReadStreamSetClient failed");
		}
	
		if ( bResults ){
			_msgs.push_back(pMsg);
		}else{
			delete pMsg;
		}
	}

	void HTTPClient::deleteMsg(HTTPMsg* pMsg){
		std::vector<HTTPMsg*>::iterator it = _msgs.begin();
		std::vector<HTTPMsg*>::iterator itEnd = _msgs.end();
		for ( ; it != itEnd; ++it ){
			if ( *it == pMsg ){
				_msgs.erase(it);
				delete pMsg;
				break;
			}
		}
	}
	
	void HTTPClient::addToRespond(HTTPMsg* pMsg){
		//lw::Locker l(CriticalSection::getGlobal());
		_respondMsgs.push_back(pMsg);
	}

	void HTTPClient::main(){
		std::vector<HTTPMsg*>::iterator it = _respondMsgs.begin();
		std::vector<HTTPMsg*>::iterator itEnd = _respondMsgs.end();
		for ( ; it != itEnd; ++it ){
			(*it)->getBuff()->readReset();
			(*it)->onRespond();
			deleteMsg(*it);
		}
		_respondMsgs.clear();
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