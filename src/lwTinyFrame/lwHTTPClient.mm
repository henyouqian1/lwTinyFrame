#include "stdafx.h"
#include "lwHTTPClient.h"
#include "lwUtil.h"
#include <sstream>

@interface HTTPCallback : NSObject {
    lw::HTTPMsg *pMsg;
}
@end

@implementation HTTPCallback
- (id)initWithMsg:(lw::HTTPMsg*)p
{
    if ( self =[super init] ){
        pMsg=p;
        pMsg->_pObjCCallback=self;
    }
    return self;
}
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    pMsg->getBuff()->reset();
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    pMsg->getBuff()->write((char*)([data bytes]), [data length]);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    pMsg->getBuff()->writeChar(0);
    pMsg->getClient()->addToRespond(pMsg);
    [connection release];
}
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    pMsg->getClient()->deleteMsg(pMsg);
    lwerror("http error");
}

- (void)dealloc
{
    [super dealloc];
}

@end

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
	
	HTTPMsg::HTTPMsg(const wchar_t* objName, HTTPClient* pClient)
	:_pClient(pClient), _buff(BUFF_SIZE){
		lwassert(objName);
		_objName = objName;
    }

	HTTPMsg::~HTTPMsg(){
		[(HTTPCallback*)_pObjCCallback release];
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
		ss << serverName << ":" << port << L"/";
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
        NSString* urlString=[[NSString alloc] initWithUTF8String:ss.str().c_str()];
        NSURL* url=[[NSURL alloc] initWithString:urlString];
        NSURLRequest *theRequest=[NSURLRequest requestWithURL:url
                                                  cachePolicy:NSURLRequestUseProtocolCachePolicy
                                              timeoutInterval:60.0];
        // create the connection with the request
        // and start loading the data
        HTTPCallback* pCallback = [[HTTPCallback alloc] initWithMsg:pMsg];
        NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:pCallback];
        if (theConnection) {
            pMsg->getBuff()->reset();
            _msgs.push_back(pMsg);
        } else {
            delete pMsg;
        }
        [urlString release];
        [url release];
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