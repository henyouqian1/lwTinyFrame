#ifndef __LW_HTTP_CLIENT_H__
#define __LW_HTTP_CLIENT_H__

#include "lwTask.h"
#ifdef WIN32
	#include <winhttp.h>
#endif
#ifdef __APPLE__
	#include <CFNetwork/CFNetwork.h>
#endif

namespace lw{
	
	class HttpMsgBuf{
	public:
		HttpMsgBuf(int initSize):_size(0), _buffSize(initSize){
			_buff = new char[initSize];
			_pRead = _buff;
		}
		~HttpMsgBuf(){
			delete [] _buff;
		}
		
		char* getBuff(){
			return _buff;
		}
		int getSize(){
			return _size;
		}
		int getBuffSize(){
			return _buffSize;
		}
		void reset(){
			_size = 0;
			_pRead = _buff;
		}

		void write(const char* p, int size);
		void writeChar(char c);
		void writeShort(short s);
		void writeInt(int n);
		void writeFloat(float f);
		void writeString(const wchar_t* str);
		void writeUtf8(const char* str);
		void writeBlob(const char* p, int size);
		void readReset();
		char readChar();
		short readShort();
		int readInt();
		float readFloat();
		const char* readString();
		void readBlob(const char*& pData, int& dataSize);

	private:
		char* _buff;
		int _buffSize;
		int _size;
		char* _pRead;
	};

	class HTTPClient;
	class HTTPMsg{
	public:
		HTTPMsg(const wchar_t* objName, lw::HTTPClient* pClient);
		const wchar_t* getObjName(){
			return _objName.c_str();
		}
		HTTPClient* getClient(){
			return _pClient;
		}
		void send();
#ifdef WIN32
		void setHRequest(HINTERNET h){
			_hRequest = h;
		}
		char* getRecvBuff(){
			return _recvBuffTemp;
		}
#endif
#ifdef __APPLE__
		CFStreamClientContext* getContext(){
			return &_cfContext;
		}
		UInt8* getRecvBuff(){
			return _recvBuffTemp;
		}
		void setReadStream(CFReadStreamRef rsr){
			_readStream = rsr;
		}
#endif
		virtual void onRespond(){}

		void onSendComplete();
		void onHeadersAvailable();
		void onReadComplete(int bytesRead);

		HttpMsgBuf* getBuff(){
			return &_buff;
		}
		
		enum {
			BUFF_SIZE = 4096,
		};

	protected:
		virtual ~HTTPMsg();

	protected:
		HttpMsgBuf _buff;
		
		
		std::wstring _objName;
		HTTPClient* _pClient;

#ifdef WIN32
		HINTERNET _hRequest;
		char _recvBuffTemp[BUFF_SIZE];
#endif
#ifdef __APPLE__
		CFStreamClientContext _cfContext;
		UInt8 _recvBuffTemp[BUFF_SIZE];
		CFReadStreamRef _readStream;
#endif

	friend class HTTPClient;
	};

	class HTTPClient{
	public:
        HTTPClient(const wchar_t* addr);
		HTTPClient(const wchar_t* serverName, unsigned short port);
		~HTTPClient();
		bool connect();
		void sendMsg(HTTPMsg* pMsg);
		void deleteMsg(HTTPMsg* pMsg);
		void addToRespond(HTTPMsg* pMsg);
		void main();

	private:
#ifdef WIN32
		std::wstring _serverName;
		unsigned short _port;
		HINTERNET _hSession;
		HINTERNET _hConnect;
		bool _isConnected;
#endif
#ifdef __APPLE__
		std::string _strUrl;
#endif
		std::vector<HTTPMsg*> _msgs;
		std::vector<HTTPMsg*> _respondMsgs;
	};

	class HTTPClientTask : public lw::Task{
	public:
		HTTPClientTask(const wchar_t* URL, unsigned short port);
		~HTTPClientTask();
		virtual void vBegin();
		virtual void vEnd();
		virtual void vMain(float dt);
		lw::HTTPClient* getClient(){
			return _pHTTPClient;
		}

	private:
		lw::HTTPClient* _pHTTPClient;
		std::wstring _URL;
		unsigned short _port;
	};

} //namespace lw

#endif //__LW_HTTP_CLIENT_H__