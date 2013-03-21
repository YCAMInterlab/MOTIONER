//
//  ofBufferExtended
//  MOTIONER
//
//  Created by Onishi Yoshito on 2/12/13.
//
//

#ifndef __MOTIONER__ofBufferExtendedExtended__
#define __MOTIONER__ofBufferExtendedExtended__

#include "ofMain.h"

//----------------------------------------------------------
// ofBufferExtended
//----------------------------------------------------------

class ofBufferExtended{
	
public:
	ofBufferExtended();
	ofBufferExtended(const char * buffer, unsigned int size);
	ofBufferExtended(const string & text);
	ofBufferExtended(istream & stream);
	ofBufferExtended(const ofBufferExtended & buffer_);
    
	~ofBufferExtended();
    
	void set(const char * _buffer, unsigned int _size);
	void set(const string & text);
	bool set(istream & stream);
	void append(const char * _buffer, unsigned int _size);
    void prependData(const char * _buffer, unsigned int _size);
    void appendData(const char * _buffer, unsigned int _size);
    
	bool writeTo(ostream & stream) const;
    
	void clear();
    
	void allocate(long _size);
    
	char * getBinaryBuffer();
	const char * getBinaryBuffer() const;
    
	string getText() const;
	operator string() const;  // cast to string, to use a buffer as a string
	ofBufferExtended & operator=(const string & text);
    
	long size() const;
	string getNextLine();
    string getFirstLine();
	bool isLastLine();
    void resetLineReader();
    
	friend ostream & operator<<(ostream & ostr, const ofBufferExtended & buf);
	friend istream & operator>>(istream & istr, ofBufferExtended & buf);
    
private:
	vector<char> 	buffer;
	long 			nextLinePos;
};

ofBufferExtended ofBufferExtendedFromFile(const string & path, bool binary);
bool ofBufferExtendedToFile(const string & path, ofBufferExtended & buffer, bool binary);



#endif /* defined(__MOTIONER__ofBufferExtendedExtended__) */
