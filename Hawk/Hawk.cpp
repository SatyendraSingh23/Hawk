#include<bits/stdc++.h>
#include<iostream>
#include<sys/stat.h>
#include<map>
#include<forward_list>
#include<string.h>
#include<unistd.h>
#ifdef _WIN32
const char *PATH_SEPARATOR="\\\\";
#include<windows.h>
#endif
#ifdef linux
const char *PATH_SEPARATOR="/";
#include<arpa/inet.h>
#include<sys/socket.h>
#endif
using namespace std;
#define _forward_(request,url) \
request.forwardTo(url); \
return;
enum __container_operation_failure_reason__{__KEY_EXISTS__,__KEY_DOES_NOT_EXISTS__,__OUT_OF_MEMORY__,__VALUE_SIZE_MISMATCH__};
class Container
{
	typedef struct _bag
	{
		void *ptr;
		int size;
	}Bag;
	map<string,Bag> dataSet;
	public :
	template<class Whatever>
	void set(string keyName,Whatever something,bool *success,__container_operation_failure_reason__ *reason)
	{
		auto iterator=dataSet.find(keyName);
		if(iterator!=dataSet.end())
		{
			if(reason) *reason={__KEY_EXISTS__};
			if(success) *success=false;
			return;
		}
		void *ptr;
		ptr=malloc(sizeof(something));
		if(ptr==NULL)
		{
			if(reason)*reason={__OUT_OF_MEMORY__};
			if(success)*success=false;
			return;
		}
		memcpy(ptr,&something,sizeof(something));
		Bag bag;
		bag.ptr=ptr;
		bag.size=sizeof(something);
		dataSet.insert(pair<string,Bag>(keyName,bag));
		if(success)*success=true;
	}
	template<class Cool>
	void get(string keyName,Cool anything,bool *success,__container_operation_failure_reason__ *reason)
	{
		auto iterator=dataSet.find(keyName);
		if(iterator==dataSet.end())
		{
			if(reason)*reason={__KEY_DOES_NOT_EXISTS__};
			if(success) *success=false;
			return;
		}
		Bag bag;
		bag=iterator->second;
		if(bag.size!=sizeof(*anything))
		{
			if(reason)*reason={__VALUE_SIZE_MISMATCH__};
			if(success)*success=false;
			return;
		}
		memcpy(anything,bag.ptr,sizeof(*anything));
		if(success)*success=true;
	}
	template<class Cool>
	void remove(string keyName,Cool anything,bool *success,__container_operation_failure_reason__ *reason)
	{
		auto iterator=dataSet.find(keyName);
		if(iterator==dataSet.end())
		{
			if(reason)*reason={__KEY_DOES_NOT_EXISTS__};
			if(success)*success=false;
			return;
		}
		Bag bag;
		bag=iterator->second;
		if(bag.size!=sizeof(*anything))
		{
			if(reason)*reason={__VALUE_SIZE_MISMATCH__};
			if(success)*success=false;
			return;
		}
		memcpy(anything,bag.ptr,sizeof(*anything));
		free(bag.ptr);
		if(success)*success=true;
	}
	bool contains(string keyName)
	{
		auto iterator=this->dataSet.find(keyName);
		return iterator!=this->dataSet.end();
	}
};

class ApplicationLevelContainer:public Container
{

};

class HawkUtilities
{
	private : 
	HawkUtilities(){}
	public :
	static bool isHexChar(int w)
	{
		if(w>=48 && w<=57)return true;
		if(w>='a' && w<='f')return true;
		if(w>='A' && w<='F')return true;
		return false;
	}
	static void decode(char *encodeString,char *decodeString)
	{
		char *ptr=encodeString;
		char *d=decodeString;
		int i,m;
		i=0;
		while(*ptr!='\0')
		{
			if(*ptr!='+')
			{
				d[i]=*ptr;
				i++;
				ptr++;
				continue;
			}
			if(*ptr!='%')
			{
				d[i]=' ';
				i++;
				ptr++;
				continue;
			}
			ptr++;
			if(isHexChar(*ptr) && isHexChar(*(ptr+1)))
			{
				sscanf(ptr,"%2x",&m);
				d[i]=m;
				i++;
				ptr+=2;
			}
			else
			{
				i=0;
				break;
			}
		}
		d[i]='\0';
	}
	static void loadMIMETypes(map<string,string> &mimeTypesMap)
	{
		FILE *file;
		file=fopen("Hawk_Data/mime.types","r");
		if(file==NULL) return;
		char *mimeType;
		char *extension;
		char line[200];
		int x;
		while(true)
		{
			fgets(line,200,file);
			if(feof(file)) break;
			if(line[0]=='#') continue;
			x=strlen(line)-1;
			while(true)
			{
				if(line[x]=='\r' || line[x]=='\n')
				{
					line[x]='\0';
					x--;
				}
				else break;
			}
			//logic to remove \r\r from the end of the string ends here
			mimeType=&line[0];
			for(x=0;line[x]!='\t';x++);
			line[x]='\0';
			x++;
			while(line[x]=='\t')x++;
			while(true)
			{
				extension=&line[x];
				while(line[x]!='\0' && line[x]!=' ')x++;
				if(line[x]=='\0')
				{
					mimeTypesMap.insert(pair<string,string>(string(extension),string(mimeType)));
					break;
				}
				else
				{
					//place \0 on xth index, add entry to map and increment the value of x
					line[x]='\0';
					mimeTypesMap.insert(pair<string,string>(string(extension),string(mimeType)));
					x++;
				}
			}//parsing extensions ends here
		}
		fclose(file);
	}
};
class FileSystemUtility
{
	private :
	FileSystemUtility(){}
	public :
	static int createDirectory(const char *directoryName)
	{
		return (mkdir(directoryName)==0);
	}
	static unsigned int getLastUpdatedTime(const char * fileName)
	{
		struct stat attributes;
		stat(fileName,&attributes);
		return attributes.st_mtime;
	}
	static bool fileExists(const char * path)
	{
		struct stat s;
		int x;
		x=stat(path,&s);
		if(x!=0) return false;
		if(s.st_mode & S_IFDIR) return false;
		return true;
	}
	static bool directoryExists(const char * path)
	{
		struct stat s;
		int x;
		x=stat(path,&s);
		if(x!=0) return false;
		if(s.st_mode & S_IFDIR) return true;
	 	return false;
	}
	static string getFileExtension(const char * path)
	{
		int x;
		x=strlen(path)-1;
		while(x>=0 && path[x]!='.') x--;
		if(x==-1 || path[x]!='.') return string("");
		return string(path+(x+1));
	}

};
class StringUtility
{
	private : 
	StringUtility(); 
	public :
	static void toLowerCase(char *str)
	{
		if(str==NULL) return;
		for(;*str;str++) if(*str>=65 && *str<=90) *str+=32;
	}
};
class HttpErrorStatusUtility
{
	private : 
	HttpErrorStatusUtility(){}
	public :
	static void sendBadRequestError(int clientSocketDescriptor)
	{
		//
	}
	static void sendHttpVersionNotSupportedError(int clientSocketDescriptor,char * httpVersion)
	{
		//
	}
	static void sendNotFoundError(int clientSocketDescriptor,char * requestURI)
	{
		char content[1000];
		char header[200];
		char response[1200];
		sprintf(content,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>404 Error</title></head><body>Requested Resource [%s] Not Found\nContent-Type</body>",requestURI);
		int contentLength=strlen(content);
		sprintf(header,"HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n",contentLength);
		strcpy(response,header);
		strcat(response,content);
		send(clientSocketDescriptor,response,strlen(response),0);
	}
	static void sendMethodNotAllowedError(int clientSocketDescriptor,char * method,char * requestURI)
	{
		//
	}
};	
class validator
{
	private :
	validator(){}
	public : 
	static bool isValidMIMEType(string &mimeType)
	{
		//right now, do nothing
		return true;
	}
	static bool isValidPath(string &path)
	{
		return FileSystemUtility::directoryExists(path.c_str());
	}
	static bool isValidURLFormat(string &url)
	{
		return true;
	}
};
class Error
{
	private :
	string error;
	public : 
	Error(string error)
	{
		this->error=error;
	}
	bool hasError()
	{
		return this->error.length()>0;
	}
	string getError()
	{
		return this->error;
	}
};
class Request
{
	private : 
	string _forwardTo;
	char * method;
	char * requestURI;
	char * httpVersion;
	map<string,string> dataMap;
	Request(char * method, char * requestURI,char *httpVersion,char *dataInRequest)
	{
		this->method=method;
		this->requestURI=requestURI;
		this->httpVersion=httpVersion;
		if(dataInRequest!=NULL && strcmp(this->method,"get")==0)
		{ 
			createDataMap(dataInRequest,dataMap);
		}
	}
	string forwardToWhichResource()
	{
		return this->_forwardTo;
	}
	bool isToBeForwarded()
	{
		return this->_forwardTo.length();
	}
	void createDataMap(char *str,map<string,string> &dataMap)
	{
		char * ptr1, *ptr2;
		char *decoded;
		int keyLength,valueLength;
		ptr1=str;
		ptr2=str;
		while(1)
		{
			while(*ptr2!='\0' && *ptr2!='=') ptr2++;
			if(*ptr2=='\0') return;
			*ptr2='\0';
			keyLength=ptr2-ptr1;
			decoded=new char[keyLength+1];
			HawkUtilities::decode(ptr1,decoded);
			string key=string(ptr1);
			delete [] decoded;
			ptr1=ptr2+1;
			ptr2=ptr1;
			while(*ptr2!='\0' && *ptr2!='&') ptr2++;
			if(*ptr2=='\0')
			{
				valueLength=ptr2-ptr1;
				decoded=new char[valueLength+1];
				HawkUtilities::decode(ptr1,decoded);
				dataMap.insert(pair<string,string>(key,string(decoded)));
				delete [] decoded;
				break;
			}
			else
			{
				*ptr2='\0';
				valueLength=ptr2-ptr1;
				decoded=new char[valueLength+1];
				HawkUtilities::decode(ptr1,decoded);
				dataMap.insert(pair<string,string>(key,string(decoded)));
				delete [] decoded;
				ptr1=ptr2+1;
				ptr2=ptr1;
			}
		}
	}
	map<string,string> varMap;
	public :
	void set(string name,string value)
	{
		varMap.insert({name,value});
	} 
	bool contains(string name)
	{
		return varMap.find(name)!=varMap.end();
	}
	string get(string name)
	{
		auto a=varMap.find(name);
		if(a==varMap.end())return string("");
		return a->second;
	}
	void forwardTo(string _forwardTo)
	{
		this->_forwardTo=_forwardTo;
	}
	string operator[](string key)
	{

		auto iterator=dataMap.find(key);
		if(iterator==dataMap.end())
		{
			return string("");
		}
		return iterator->second;

	}
	friend class Hawk;
};
class Response
{
	private : 
	string contentType;
	forward_list<string> content;
	forward_list<string>::iterator contentIterator;
	unsigned long contentLength;
	public : 
	Response()
	{
		this->contentLength=0;
		this->contentIterator=this->content.before_begin();
	}
	~Response()
	{
 		//not yet decided
	}
	void setContentType(string contentType)
	{
		if(validator::isValidMIMEType(contentType))
		{
			this->contentType=contentType;
		}
	}
	Response & operator<<(string content)
	{
		this->contentLength+=content.length();
		this->contentIterator=this->content.insert_after(this->contentIterator,content);
		return *this;
	}
	friend class HttpResponseUtility;
};	
class HttpResponseUtility
{
	private : 
	HttpResponseUtility(){}
	public :
	static void sendResponse(int clientSocketDescriptor,Response &response)
	{
		char header[200];
		int contentLength=response.contentLength;
		sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n",contentLength);
		send(clientSocketDescriptor,header,strlen(header),0);
		auto contentIterator=response.content.begin();
		while(contentIterator!=response.content.end())
		{
			string str = *contentIterator;
			send(clientSocketDescriptor,str.c_str(),str.length(),0);
			++contentIterator;
		}
	}
};

class Function
{
	public :
	virtual void doService(Request &,Response &)=0;
};
class StartupFunction
{
	public :
	virtual void run()=0;
	virtual int getPriorityNumber()=0;
};

enum __request_method__{__GET__,__POST__,__PUT__,__DELETE__,CONNECT__,TRACE__,HEAD__,__OPTIONS__};
typedef struct __url_mapping
{
	__request_method__ requestMethod;
	Function *function;
	//void (*mappedFunction)(Request &, Response &);
}URLMapping;

class SimpleFunction:public Function
{
	private : 
	void (*mappedFunction)(Request &,Response &);
	public :
	SimpleFunction(void (*mappedFunction)(Request &,Response &))
	{
		this->mappedFunction=mappedFunction;
	}
	void doService(Request &request,Response &response)
	{
		this->mappedFunction(request,response);
	}
};
class ApplicationLevelContainerDependentFunction:public Function
{
	private :
		void (*mappedFunction)(Request &,Response &,ApplicationLevelContainer &);
		ApplicationLevelContainer *p2ApplicationLevelContainer;
	public: 
		ApplicationLevelContainerDependentFunction(void (*mappedFunction)(Request &, Response &, ApplicationLevelContainer &),ApplicationLevelContainer *p2ApplicationLevelContainer)
		{
			this->mappedFunction=mappedFunction;
			this->p2ApplicationLevelContainer=p2ApplicationLevelContainer;
		}
		void doService(Request &request,Response &response)
		{
			this->mappedFunction(request,response,*p2ApplicationLevelContainer);
		}

};
class SimplesStartupFunction: public StartupFunction
{
	private :
	void (*startupFunction)(void);
	int priorityNumber;
	public :
	SimplesStartupFunction(int priorityNumber,void (*startupFunction)(void))
	{
		this->priorityNumber=priorityNumber;
		this->startupFunction=startupFunction;
	}
	int getPriorityNumber()
	{
		return this->priorityNumber;
	}
	void run()
	{
		startupFunction();
	}
};
class ApplicationLevelContainerDependentStartupFunction:public StartupFunction
{
	private :
	int priorityNumber;
	void (*startupFunction)(ApplicationLevelContainer &);
	ApplicationLevelContainer *p2ApplicationLevelContainer;
	public :
	ApplicationLevelContainerDependentStartupFunction(int priorityNumber,void (*startupFunction)(ApplicationLevelContainer &), ApplicationLevelContainer *p2ApplicationLevelContainer)
	{
		this->priorityNumber=priorityNumber;
		this->startupFunction=startupFunction;
		this->p2ApplicationLevelContainer=p2ApplicationLevelContainer;
	}
	int getPriorityNumber()
	{
		return this->priorityNumber;
	}
	void run()
	{
		startupFunction(*p2ApplicationLevelContainer);
	}
};
class StartupFunctionComparator
{
	public :
	int operator()(StartupFunction *e,StartupFunction *f)
	{
		return !(e->getPriorityNumber()<f->getPriorityNumber());
	}
};
class TemplateEngine
{
	private :
	struct rvn
	{
		long start_position;
		long end_position;
		char var_name[256];
	};

	static void createRVNFileName(const char *hawkFileName,char *rvnFileName)
	{
		char *doPtr;
		const char *ptr;
		ptr=hawkFileName;
		while(*ptr!='\0') ptr++;
		while(ptr>=hawkFileName)
		{
			if(*ptr=='/')
			{
				hawkFileName=ptr+1;
				break;
			}
			ptr--;
		}
		for(;*hawkFileName!='\0';hawkFileName++,rvnFileName++)
		{
			if(*hawkFileName=='.') doPtr=rvnFileName;
			*rvnFileName=*hawkFileName;		
		}
		*rvnFileName='\0';
		strcpy(doPtr+1,"rvn");
	}
	static void createRVNFile(const char* hawkFileName,const char *pathToRVNFile)
	{
		char *start,*end,*ptr;
		unsigned int x;
		FILE *f;
		int toRead;
		char buffer[513];
		long fileLength;
		f=fopen(hawkFileName,"rb");
		if(f==NULL)
		{
			//code to send back 404 error
			return;
		}
		FILE *rvnFile;
		rvnFile=fopen(pathToRVNFile,"wb");
		if(rvnFile==NULL)
		{
			//code to send back 500 internal server error page
			fclose(f);
			return;
		}
		fseek(f,0,SEEK_END);
		fileLength=ftell(f);
		rewind(f);
		unsigned int sp,ep;
		int ii;
		struct rvn rvn_record;
		x=0;
		while(1)
		{
			if((fileLength-x)>256) toRead=256;
			else toRead=fileLength-x;
			fread(buffer,toRead,1,f);
			buffer[toRead]='\0';
			x=x+toRead;
			//processsing the contents of the buffer starts here
			start=buffer;
			while(1)
			{
				start=strstr(start,"${");
				if(start==NULL)
				{
					if(x==fileLength)break;
					if(buffer[toRead-1]=='$')
					{
						buffer[0]='$';
						if((fileLength)>255) toRead=255;
						else toRead=fileLength-x;
						fread(buffer+1,toRead,1,f);
						x=x+toRead;
						buffer[1+toRead]='\0';
						start=buffer;
						continue;
					}
					else
					{
						break;
					}
				}
				end=strstr(start+2,"}");
				if(end!=NULL)
				{
					//printf("---------------------------------\n");
					for(ptr=start+1;ptr<end;ptr++)if(*ptr=='$' && *(ptr+1)=='{') break;
					if(*ptr=='$')start=ptr;
					//write rvn record to the file starts here
					for(ptr=start+2,ii=0;ptr<end;ptr++,ii++)
					{
						rvn_record.var_name[ii]=*ptr;
					}
					rvn_record.var_name[ii]='\0';
					sp=(x-toRead)+(start-buffer);
					ep=(x-toRead)+(end-buffer);
					rvn_record.start_position=sp;
					rvn_record.end_position=ep;
					fwrite(&rvn_record,sizeof(struct rvn),1,rvnFile);
					//write rvn record to the file ends here
					start=end+1;
					continue;
				}
				//if } not found, then load the next chunk of 256 bytes
				// and store it in buffer from index 256
				if(x==fileLength)break;
				if((fileLength-x)>256) toRead=256;
				else toRead=fileLength-x;
				fread(buffer+256,toRead,1,f);
				buffer[256+toRead]='\0';
				x=x+toRead;
				end=strstr(buffer+256,"}");
				if(end==NULL) break;
				for(ptr=start+1;ptr<end;ptr++)if(*ptr=='$' && *(ptr+1)=='{')break;
				if(*ptr=='$') start=ptr;
				//write rvn record to the file starts here
				for(ptr=start+2,ii=0;ptr<end;ptr++,ii++) rvn_record.var_name[ii]='\0';
				sp=(x-toRead)+(start-buffer)-256;
				ep=(x-toRead)+(end-buffer)-256;
				rvn_record.start_position=sp;
				rvn_record.end_position=ep;
				fwrite(&rvn_record,sizeof(struct rvn),1,rvnFile);
				strcpy(buffer,buffer+256);
				end=end-256;
				start=end+1;
			}//while(1) ends
			//processing the contents of the buffer ends here
			if(x==fileLength)break;
		}
		fclose(f);
		fclose(rvnFile);
	}
	public :
	//more parameters related to other type of containers will be added later on
	static void processHAWKResource(const char * hawkFileName,Request &request,int clientSocketDescriptor)
	{
		if(!FileSystemUtility::directoryExists("Raven"))
		{
			if(!FileSystemUtility::createDirectory("Raven"))
			{
				//we will implement this part later on
			}
		}
		char rvnFileName[257];
		createRVNFileName(hawkFileName,rvnFileName);
		string folderName=string("Raven");
		string pathToRVNFile=folderName+string(PATH_SEPARATOR)+string(rvnFileName);
		bool generatedRVNFile=false;
		if(FileSystemUtility::fileExists(pathToRVNFile.c_str()))
		{
			if(FileSystemUtility::getLastUpdatedTime(hawkFileName)>FileSystemUtility::getLastUpdatedTime(pathToRVNFile.c_str()))
			{
				generatedRVNFile=true;
			}
		}
		else
		{
			generatedRVNFile=true;
		}
		if(generatedRVNFile)
		{
			createRVNFile(hawkFileName,pathToRVNFile.c_str());
		}
		//process the hawk file (pick up info from rvn file)
		FILE *hawkFile=fopen(hawkFileName,"rb");
		FILE *rvnFile=fopen(pathToRVNFile.c_str(),"rb");
		fseek(hawkFile,0,SEEK_END);
		long fileLength=ftell(hawkFile);
		rewind(hawkFile);
		long responseSize=fileLength;
		string data;
		struct rvn rvnRecord;
		while(1)
		{
			fread(&rvnRecord,sizeof(struct rvn),1,rvnFile);
			if(feof(rvnFile))break;
			responseSize=responseSize-((rvnRecord.end_position-rvnRecord.start_position+1));
			data=request.get(rvnRecord.var_name);
			responseSize=responseSize+data.length();
		}
		cout<<"RESPONSE SIZE : "<<responseSize<<endl;
		//code to calculate response size ends here
		//the code to process hawk file starts here
		string mimeType;
		mimeType=string("text/html");
		char header[200];
		sprintf(header,"HTTP/1.1 200 OK\r\nContent-Type : %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n",mimeType.c_str(),responseSize);
		send(clientSocketDescriptor,header,strlen(header),0);
		long bytesLeftToRead;
		int bytesToRead;
		char buffer[4096];
		bytesLeftToRead=fileLength;
		rewind(rvnFile);
		long tmpBytesLeftToRead;
		long bytesProcessedFromFile=0;
		while(1)
		{
			fread(&rvnRecord,sizeof(struct rvn),1,rvnFile);
			if(feof(rvnFile))break;
			tmpBytesLeftToRead=rvnRecord.start_position-bytesProcessedFromFile;
			bytesToRead=4096;
			while(tmpBytesLeftToRead>0)
			{
				if(tmpBytesLeftToRead<bytesToRead)bytesToRead=tmpBytesLeftToRead;
				fread(buffer,bytesToRead,1,hawkFile);
				if(feof(hawkFile))break;
				bytesProcessedFromFile=bytesProcessedFromFile+bytesToRead;
				send(clientSocketDescriptor,buffer,bytesToRead,0);
				tmpBytesLeftToRead=tmpBytesLeftToRead-bytesToRead;
			}
			fread(buffer,(rvnRecord.end_position-rvnRecord.start_position)+1,1,hawkFile);
			bytesProcessedFromFile=bytesProcessedFromFile+(rvnRecord.end_position-rvnRecord.start_position)+1;
			if(request.contains(rvnRecord.var_name))
			{
				data=request.get(rvnRecord.var_name);
				cout<<"DATA : "<<data<<endl;
				send(clientSocketDescriptor,data.c_str(),data.length(),0);
			}//inner loop ends
		} // outer loop traversing in rvn file ends
		bytesLeftToRead=bytesLeftToRead-bytesProcessedFromFile;
		bytesToRead=4096;
		while(bytesLeftToRead>0)
		{
			if(bytesLeftToRead<bytesToRead)bytesToRead=bytesLeftToRead;
			fread(buffer,bytesToRead,1,hawkFile);
			if(feof(hawkFile))break;
			send(clientSocketDescriptor,buffer,bytesToRead,0);
			bytesToRead=bytesLeftToRead-bytesToRead;
		}// the code to process hawk file ends here
		fclose(hawkFile);
		fclose(rvnFile);
	}
};// class TemplateEngine ends
class Hawk
{
	private : 
	string staticResourceFolder;
	map<string,string> mimeTypes;
	map<string,URLMapping> urlMappings;
	ApplicationLevelContainer applicationLevelContainer;
	priority_queue<StartupFunction *,vector<StartupFunction*>,StartupFunctionComparator> startupFunctions;
	
	bool isHawk(const char *requestURI)
	{
		// a.hawk
		int len=strlen(requestURI);
		if(len<7) return false;
		const char *ptr1=requestURI+len-5;
		char lastPart[7]={".hawk"};
		const char *ptr2=lastPart;
		char a,b;
		while(*ptr1 && *ptr2)
		{
			a=*ptr1;
			b=*ptr2;
			if(a>=97 && a<=122)a-=32;
			if(b>=97 && b<=122)b-=32;
			if(a!=b) return false;
			ptr1++;
			ptr2++;
		}
		return true;
	}
	public : 
	Hawk()
	{
		HawkUtilities::loadMIMETypes(mimeTypes);
		if(mimeTypes.size()==0) throw string ("Hawk_Data folder has been tampered with.");
	}
	void processHAWKResource(int clientSocketDescriptor,const char *requestURI,Request &request)
	{
		if(this->staticResourceFolder.length()==0)
		{
			//send back 404
			return;
		}
		if(!FileSystemUtility::directoryExists(this->staticResourceFolder.c_str()))
		{
			//send back 404
			return;
		}
		string resourcePath=this->staticResourceFolder+string(requestURI);
		if(!FileSystemUtility::fileExists(resourcePath.c_str()))
		{
			//send back 404
			return;
		}
		TemplateEngine::processHAWKResource(resourcePath.c_str(),request,clientSocketDescriptor);
	}
	void setStaticResourceFolder(string staticResourceFolder)
	{
		if(validator::isValidPath(staticResourceFolder)) this->staticResourceFolder=staticResourceFolder;
		else
		{
			string exception=string("Invalid static resource folder path : ")+staticResourceFolder;
			throw exception;
		}
	}
	bool serveStaticResource(int clientSocketDescriptor,const char * requestURI)
	{
		if(this->staticResourceFolder.length()==0) return false;
		if(!FileSystemUtility::directoryExists(this->staticResourceFolder.c_str()))return false;
		string resourcePath=this->staticResourceFolder+string(requestURI);
		if(!FileSystemUtility::fileExists(resourcePath.c_str())) return false;
		FILE *file=fopen(resourcePath.c_str(),"rb");
		if(file==NULL) return false;
		long fileSize;
		fseek(file,0,SEEK_END);
		fileSize=ftell(file);
		if(fileSize==0)
		{
			fclose(file);
			return false;
		}
		rewind(file);// to move the internal file pointer to the start of the file.
		string extension,mimeType;
		extension=FileSystemUtility::getFileExtension(resourcePath.c_str());
		if(extension.length()>0)
		{
			transform(extension.begin(),extension.end(),extension.begin(),::tolower);
			auto mimeTypesIterator=mimeTypes.find(extension);
			if(mimeTypesIterator!=mimeTypes.end())
			{
				mimeType=mimeTypesIterator->second;
			}
			else
			{
				mimeType=string("text/html");
			}
		}
		else
		{
			mimeType=string("text/html");
		}
		char header[200];
		sprintf(header,"HTTP/1.1 200 OK\r\nContent-Type : text/html\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n",fileSize);
		send(clientSocketDescriptor,header,strlen(header),0);
		long bytesLeftToRead;
		int bytesToRead;
		char buffer[4096];
		bytesToRead=4096;
		bytesLeftToRead=fileSize;
		while(bytesLeftToRead>0)
		{
			if(bytesLeftToRead<bytesToRead)bytesToRead=bytesLeftToRead;
			fread(buffer,bytesToRead,1,file);
			if(feof(file))break;// this won't happen in our case.
			send(clientSocketDescriptor,buffer,bytesToRead,0);
			bytesLeftToRead=bytesLeftToRead-bytesToRead;
		}
		fclose(file);
		return true;
	}
	void addStartupService(int priorityNumber,void (*startupFunction)(void))
	{
		StartupFunction *sf;
		sf=new SimplesStartupFunction(priorityNumber,startupFunction);
		this->startupFunctions.push(sf);
	}
	void addStartupService(int priorityNumber,void (*startupFunction)(ApplicationLevelContainer &))
	{
		StartupFunction *sf;
		sf=new ApplicationLevelContainerDependentStartupFunction(priorityNumber,startupFunction,&(this->applicationLevelContainer));
		this->startupFunctions.push(sf);
	}
	void get(string url,void (*callBack)(Request &,Response &))
	{
		if(validator::isValidURLFormat(url))
		{
			Function *function;
			function=new SimpleFunction(callBack);
			urlMappings.insert(pair<string,URLMapping>(url,{__GET__,function}));
		}
	}
	void get(string url,void (*callBack)(Request &,Response &,ApplicationLevelContainer &))
	{
		if(validator::isValidURLFormat(url))
		{
			Function *function;
			function=new ApplicationLevelContainerDependentFunction(callBack,&(this->applicationLevelContainer));
			urlMappings.insert(pair<string,URLMapping>(url,{__GET__,function}));
		}
	}
	void post(string url,void (*callBack)(Request &,Response &))
	{
		if(validator::isValidURLFormat(url))
		{
			Function *function;
			function=new SimpleFunction(callBack);
			urlMappings.insert(pair<string,URLMapping>(url,{__POST__,function}));
		}
	}
	void listen(int portNumber,void (*callBack)(Error &))
	{
		#ifdef _WIN32
		WSADATA wsaData;
		WORD ver;
		ver=MAKEWORD(1,1);
		WSAStartup(ver,&wsaData);
		#endif
		int serverSocketDescriptor;
		char requestBuffer[4097];
		int requestLength;
		int x;
		serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(serverSocketDescriptor<0)
		{
			#ifdef _WIN32
			WSACleanup();
			#endif
			Error error("Unable to create socket.");
			callBack(error);
			return;
		}
		struct sockaddr_in serverSocketInformation;
		serverSocketInformation.sin_family=AF_INET;
		serverSocketInformation.sin_port=htons(portNumber);
		serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
		int successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
		if(successCode<0)
		{
			close(serverSocketDescriptor);
			#ifdef _WIN32
			WSACleanup();
			#endif
			char a[101];
			sprintf(a,"unable to bind socket to port : %d",portNumber);
			Error error(a);
			callBack(error);
			return;
		}
		successCode=::listen(serverSocketDescriptor,10);
		if(successCode<0)
		{
			close(serverSocketDescriptor);
			#ifdef _WIN32
			WSACleanup();
			#endif
			Error error("Unable to accept client connections.");
			callBack(error);
			return;
		}
		// Code to call all startup services starts here
		StartupFunction *startupFunction;
		while(!startupFunctions.empty())
		{
			startupFunction=startupFunctions.top();
			startupFunctions.pop();
			startupFunction->run();
		}
		// Code to call all startup services ends here
		Error error("");
		callBack(error);
		struct sockaddr_in clientSocketInformation;
		#ifdef linux
		socklen_t len=sizeof(clientSocketInformation);
		#endif
		#ifdef _WIN32
		int len=sizeof(clientSocketInformation);
		#endif
		int clientSocketDescriptor;
		while(1)
		{
			int i;
			clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation, &len);
			if(clientSocketDescriptor<0)
			{
				//not yet decided, will write this code later on
			}
			requestLength=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer)-sizeof(char),0);
			if(requestLength==0 || requestLength==-1)
			{
				close(clientSocketDescriptor);
				continue;
			}
			char *method, * requestURI,*httpVersion,*dataInRequest;
			requestBuffer[requestLength]='\0';
			printf("*******************************\n\n");
			printf("%s\n",requestBuffer);
			printf("\n\n***************************\n\n");
			// code to parse the first line of http request starts here
			// first line should be REQUEST_METHOD SPACE URI HTTPVersionCRLF
			method=requestBuffer;
			i=0;
			while(requestBuffer[i] && requestBuffer[i]!=' ')i++;		
			if(requestBuffer[i]=='\0')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			requestBuffer[i]='\0';
			i++;
			if(requestBuffer[i]==' '|| requestBuffer[i]=='\0')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			StringUtility::toLowerCase(method);
			if(!(strcmp(method,"get")==0  || strcmp(method,"post")==0 ||strcmp(method,"put")==0 ||strcmp(method,"delete")==0 ||strcmp(method,"head")==0 ||strcmp(method,"options")==0 ||strcmp(method,"trace")==0 ||strcmp(method,"connect")==0))
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			requestURI=requestBuffer+i;
			while(requestBuffer[i] && requestBuffer[i]!=' ')i++;
			if(requestBuffer[i]=='\0')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			requestBuffer[i]='\0';
			i++;
			if(requestBuffer[i]==' '|| requestBuffer[i]=='\0')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			httpVersion=requestBuffer+i;
			while(requestBuffer[i] && requestBuffer[i]!='\r' && requestBuffer[i]!='\n')i++;
			if(requestBuffer[i]=='\0')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			if(requestBuffer[i]=='\r' && requestBuffer[i+1]!='\n')
			{
				HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
				close(clientSocketDescriptor);
				continue;
			}
			if(requestBuffer[i]=='\r')
			{
				requestBuffer[i]='\0';
				i=i+2;
			}
			else
			{
				requestBuffer[i]='\0';
				i=i+1;
			}
			StringUtility::toLowerCase(httpVersion);
			if(strcmp(httpVersion,"http/1.1")!=0)
			{
				HttpErrorStatusUtility::sendHttpVersionNotSupportedError(clientSocketDescriptor,httpVersion);
				close(clientSocketDescriptor);
				continue;
			}
			dataInRequest=NULL;
			i=0;
			while(requestURI[i]!='\0' && requestURI[i]!='?')i++;
			
			if(requestURI[i]=='?')
			{
				requestURI[i]='\0';
				dataInRequest=requestURI+i+1;
			}
			auto urlMappingsIterator=urlMappings.find(requestURI);
			if(urlMappingsIterator==urlMappings.end())
			{
				if(isHawk(requestURI))
				{
					Request request(method,requestURI,httpVersion,dataInRequest);
					processHAWKResource(clientSocketDescriptor,requestURI,request);
				}
				else if(!serveStaticResource(clientSocketDescriptor,requestURI))
				{
					HttpErrorStatusUtility::sendNotFoundError(clientSocketDescriptor,requestURI);					
				}
				close(clientSocketDescriptor);
				continue;
			}
			URLMapping urlMapping=urlMappingsIterator->second;
			if(urlMapping.requestMethod==__GET__ && strcmp(method,"get")!=0)
			{
				HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor,method,requestURI);
				close(clientSocketDescriptor);
				continue;
			}
			//code to parse the header and then the payload if exists starts here
			//code to parse the header and then the payload if exists ends here
			Request request(method,requestURI,httpVersion,dataInRequest);
			cout<<"---------------------- S T A R T------------------------------------"<<endl;
			while(true)
			{
				Response response;
				urlMapping.function->doService(request,response);
				if(!request.isToBeForwarded())
				{
					HttpResponseUtility::sendResponse(clientSocketDescriptor,response);
					break;
				}
				string  forwardTo=request.forwardToWhichResource();
				urlMappingsIterator=urlMappings.find(forwardTo);
				if(urlMappingsIterator==urlMappings.end())
				{
					if(isHawk(forwardTo.c_str()))
					{
						request.forwardTo(string(""));
						processHAWKResource(clientSocketDescriptor,forwardTo.c_str(),request);
					}
					else if(!serveStaticResource(clientSocketDescriptor,forwardTo.c_str()))
					{
						HttpErrorStatusUtility::sendNotFoundError(clientSocketDescriptor,requestURI);	
					}
					break;
				}
				urlMapping=urlMappingsIterator->second;
				if(urlMapping.requestMethod==__GET__ and strcmp(method,"get")!=0)
				{
					HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor,method,requestURI);
					break;
				}
			}
			cout<<"---------------------- E N D----------------------------------"<<endl;
			close(clientSocketDescriptor);
		}//infinte loop ends here
		#ifdef _WIN32
		WSACleanup();
		#endif
	}
};

// Hawk [The web application developer - user of Hawk web server] 
int main()
{
	typedef struct _student
	{
		int rollNumber;
		char name[21];
		char gender;
	}Student;
	try
	{
		Hawk hawk;
		hawk.setStaticResourceFolder("Raven");
		hawk.addStartupService(2,[](){
			cout<<"-----------------------------------------------------------------------"<<endl;
			cout<<"Some cool function that gets called on startup"<<endl;
			cout<<"The priority number set for this function is 2"<<endl;
			cout<<"-----------------------------------------------------------------------"<<endl;

		});
		hawk.addStartupService(1,[](){
			cout<<"-----------------------------------------------------------------------"<<endl;
			cout<<"Some cool function that gets called on startup"<<endl;
			cout<<"The priority number set for this function is 1"<<endl;
			cout<<"-----------------------------------------------------------------------"<<endl;

		});
		hawk.addStartupService(10,[](ApplicationLevelContainer &applicationLevelContainer){
			cout<<"-----------------------------------------------------------------------"<<endl;
			cout<<"Some too cool function that gets called on startup"<<endl;
			cout<<"The priority number set for this function is 10"<<endl;
			cout<<"-----------------------------------------------------------------------"<<endl;

		});
		hawk.get("/firstCartoonFilm",[](Request &request,Response &response,ApplicationLevelContainer &cc)void {
			string *str;
			str=new string("The Jungle Book");
			cc.set("firstFilm",str,NULL,NULL);
			const char *html =R"""(
			<!DOCTYPE HTML>
			<html lang='en'>
			<head>
			<meta charset='en'>
			<title>hawk Test Cases</title>
			</head>
			<body>
			<h1>First Cartoon Film</h1>
			<h3>The Jungle Book</h3>
			<br/><br/>
			<a href='/secondCartoonFilm'>Watch the next film</a>
			</body>
			</html>
 			)""";
 			response.setContentType("text/html");
 			response<<html;
		});
		hawk.get("/secondCartoonFilm",[](Request &request,Response &response,ApplicationLevelContainer &cc) void 
		{
			string *s;
			cc.get("firstFilm",&s,NULL,NULL);
			response.setContentType("text/html");
			const char *html1=R"""(
				<!DOCTYPE HTML>
				<html lang='en'>
				<head>
				<meta charset='utf-8'>
				<title>hawk Test Cases</title>
				</head>
				<body>
				<h1>First film was
			)""";
			response<<html1;
			response<<*s;
			const char *html2=R"""(
			</h1>
			<br/><br/>
			<h1>Second Cartoon Film</h1>
			<h3>Mulan</h3>
			<br/><br/>
			</body>
			</html>
			)""";
			response<<html2;
		});
// ------------------------------------------------------------------------------------------------------------------------- //
		hawk.get("/coolOne",[](Request &request,Response &response) void {

			cout<<"some processing is being done over here by function mapped to /coolOne"<<endl;
			cout<<"Now forwwarding request to coolTwo"<<endl;
			_forward_(request,string("/coolTwo"));
			cout<<"Some lines that should not get executed as request has been forwarde"<<endl;
		});
		hawk.get("/coolTwo",[](Request &request,Response &response) void {

			cout<<"some processing is being done over here by function mapped to /coolTwo"<<endl;
			cout<<"Now forwwarding request to coolThree"<<endl;
			_forward_(request,string("/coolThree"));
		});
		hawk.get("/coolThree",[](Request &request,Response &response) void {

			cout<<"some processing is being done over here by function mapped to /coolTwo"<<endl;
			cout<<"Now forwwarding request to coolThree"<<endl;
			_forward_(request,string("/SomethingCool.html"));
		});
		hawk.get("/sloganofTheDay",[](Request &request,Response &response) void {
			string slogan;
			ifstream iFile("data/sofd.data");
			string line;
			int i=0;
			while(true)
			{
				if(!getline(iFile,line))break;
				if(slogan.length()>0)slogan+=string("<br>");
				slogan+=line;
				i++;
			}
			iFile.close();
			request.set("sloganofTheDay",slogan);
			request.set("city1","Ujjain");
			request.set("city2","Mumbai");
			_forward_(request,string("/wordsOfWisdom.hawk"));
		});
		hawk.listen(6060,[](Error &error) void {
			if(error.hasError())
			{
				cout<<error.hasError();
				return;
			}
			cout<<"Hawk HTTP Server is ready to accept request on port 6060"<<endl;
		});
	}catch(string exception)
	{
		cout<<exception<<endl;
	}
	return 0;
}