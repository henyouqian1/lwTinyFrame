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
		HTTPMsg(const wchar_t* objName, lw::HTTPClient* pClient, bool useHTTPS = false);
		const wchar_t* getObjName(){
			return _objName.c_str();
		}
		HTTPClient* getClient(){
			return _pClient;
		}
        void addParam(const wchar_t* param);
		void send();
#ifdef WIN32
		void setHRequest(HINTERNET h){
			_hRequest = h;
		}
		char* getRecvBuff(){
			return _recvBuffTemp;
		}
        void onSendComplete();
		void onHeadersAvailable();
		void onReadComplete(int bytesRead);
#endif
#ifdef __APPLE__
        void* _pObjCCallback;
#endif
		virtual void onRespond(){}

		
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
        bool _useHTTPS;

#ifdef WIN32
		HINTERNET _hRequest;
		char _recvBuffTemp[BUFF_SIZE];
#endif

	friend class HTTPClient;
	};

	class HTTPClient{
	public:
        HTTPClient(const wchar_t* host);
		~HTTPClient();
		bool connect();
		void sendMsg(HTTPMsg* pMsg, bool useHTTPS);
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
		std::string _strHost;
#endif
		std::vector<HTTPMsg*> _msgs;
		std::vector<HTTPMsg*> _respondMsgs;
	};

/*
	class HTTPClientTask : public lw::Task{
	public:
		HTTPClientTask(const wchar_t* host);
		~HTTPClientTask();
		virtual void vBegin();
		virtual void vEnd();
		virtual void vMain(float dt);
		lw::HTTPClient* getClient(){
			return _pHTTPClient;
		}

	private:
		lw::HTTPClient* _pHTTPClient;
		std::wstring _strHost;
    };
*/
    
} //namespace lw

#endif //__LW_HTTP_CLIENT_H__