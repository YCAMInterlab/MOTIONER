//
//  ofBufferExtendedExtended.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/12/13.
//
//

#include "ofBufferExtended.h"

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofBufferExtended
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
ofBufferExtended::ofBufferExtended(){
	nextLinePos = 0;
	buffer.resize(1);
}

//--------------------------------------------------
ofBufferExtended::ofBufferExtended(const char * buffer, unsigned int size){
	set(buffer, size);
}

//--------------------------------------------------
ofBufferExtended::ofBufferExtended(const string & text){
	set(text);
}

//--------------------------------------------------
ofBufferExtended::ofBufferExtended(istream & stream){
	set(stream);
}

//--------------------------------------------------
ofBufferExtended::ofBufferExtended(const ofBufferExtended & buffer_){
	buffer = buffer_.buffer;
	nextLinePos = buffer_.nextLinePos;
}

//--------------------------------------------------
ofBufferExtended::~ofBufferExtended(){
	clear();
}

//--------------------------------------------------
bool ofBufferExtended::set(istream & stream){
	clear();
	if(stream.bad()){
		return false;
	}
    
	char aux_buffer[1024];
	std::streamsize size = 0;
	stream.read(aux_buffer, 1024);
	std::streamsize n = stream.gcount();
	while(n > 0){
		// we resize to size+1 initialized to 0 to have a 0 at the end for strings
		buffer.resize(size + n + 1, 0);
		memcpy(&(buffer[0]) + size, aux_buffer, n);
		size += n;
		if(stream){
			stream.read(aux_buffer, 1024);
			n = stream.gcount();
		}
		else{n = 0;
		}
	}
	return true;
}

//--------------------------------------------------
bool ofBufferExtended::writeTo(ostream & stream) const {
	if(stream.bad()){
		return false;
	}
	stream.write(&(buffer[0]), buffer.size() - 1);
	return true;
}

//--------------------------------------------------
void ofBufferExtended::set(const char * _buffer, unsigned int _size){
	buffer.assign(_buffer,_buffer+_size);
	buffer.resize(buffer.size()+1);
	buffer.back() = 0;
}

//--------------------------------------------------
void ofBufferExtended::set(const string & text){
	set(text.c_str(),text.size());
}

//--------------------------------------------------
void ofBufferExtended::append(const char * _buffer, unsigned int _size){
	buffer.insert(buffer.end()-1,_buffer,_buffer+_size);
	buffer.back() = 0;
}

//--------------------------------------------------
void ofBufferExtended::appendData(const char * _buffer, unsigned int _size){
	buffer.insert(buffer.end()-1,_buffer,_buffer+_size);
}

//--------------------------------------------------
void ofBufferExtended::prependData(const char * _buffer, unsigned int _size){
	buffer.insert(buffer.begin(),_buffer,_buffer+_size);
}

//--------------------------------------------------
void ofBufferExtended::clear(){
	buffer.resize(1);
	nextLinePos = 0;
}

//--------------------------------------------------
void ofBufferExtended::allocate(long _size){
	clear();
	buffer.resize(_size);
}

//--------------------------------------------------
char *ofBufferExtended::getBinaryBuffer(){
	if(buffer.empty()){
		return NULL;
	}
	return &buffer[0];
}

//--------------------------------------------------
const char *ofBufferExtended::getBinaryBuffer() const {
	if(buffer.empty()){
		return "";
	}
	return &buffer[0];
}

//--------------------------------------------------
string ofBufferExtended::getText() const {
	if(buffer.empty()){
		return "";
	}
	return &buffer[0];
}

//--------------------------------------------------
ofBufferExtended::operator string() const {
	return getText();
}

//--------------------------------------------------
ofBufferExtended & ofBufferExtended::operator=(const string & text){
	set(text);
	return *this;
}

//--------------------------------------------------
long ofBufferExtended::size() const {
	if(buffer.empty()){
		return 0;
	}
	//we always add a 0 at the end to avoid problems with strings
	return buffer.size() - 1;
}

//--------------------------------------------------
string ofBufferExtended::getNextLine(){
	if(buffer.empty() || (int)(buffer.size() - 1) == nextLinePos){
		return "";
	}
	long currentLinePos = nextLinePos;
	bool lineEndWasCR = false;
	while(nextLinePos < (int)buffer.size() - 1 && buffer[nextLinePos] != '\n'){
		if(buffer[nextLinePos] != '\r'){
			nextLinePos++;
		}
		else{
			lineEndWasCR = true;
			break;
		}
	}
	string line(getBinaryBuffer() + currentLinePos, nextLinePos - currentLinePos);
	if(nextLinePos < (int)(buffer.size() - 1)){
		nextLinePos++;
	}
	// if lineEndWasCR check for CRLF
	if(lineEndWasCR && nextLinePos < (int)(buffer.size() - 1) && buffer[nextLinePos] == '\n'){
		nextLinePos++;
	}
	return line;
}

//--------------------------------------------------
string ofBufferExtended::getFirstLine(){
	resetLineReader();
	return getNextLine();
}

//--------------------------------------------------
bool ofBufferExtended::isLastLine(){
	return (int)(buffer.size() - 1) == nextLinePos;
}

//--------------------------------------------------
void ofBufferExtended::resetLineReader(){
	nextLinePos = 0;
}

//--------------------------------------------------
ostream & operator<<(ostream & ostr, const ofBufferExtended & buf){
	buf.writeTo(ostr);
	return ostr;
}

//--------------------------------------------------
istream & operator>>(istream & istr, ofBufferExtended & buf){
	buf.set(istr);
	return istr;
}

//--------------------------------------------------
ofBufferExtended ofBufferExtendedFromFile(const string & path, bool binary){
	ios_base::openmode mode = binary ? ifstream::binary : ios_base::in;
	ifstream istr(ofToDataPath(path, true).c_str(), mode);
	ofBufferExtended buffer(istr);
	istr.close();
	return buffer;
}

//--------------------------------------------------
bool ofBufferExtendedToFile(const string & path, ofBufferExtended & buffer, bool binary){
	ios_base::openmode mode = binary ? ofstream::binary : ios_base::out;
	ofstream ostr(ofToDataPath(path, true).c_str(), mode);
	bool ret = buffer.writeTo(ostr);
	ostr.close();
	return ret;
}
