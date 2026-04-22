/*******************************************************************************
*
* TextEdit.H - Edit text from a text buffer
*
* This code is distributed under the LGPL
* 
* Maarten de Boer 1999
*
*******************************************************************************/

#ifndef __TEXTEDIT__
#define __TEXTEDIT__

#include "TextDisp.H"
#include "TextBuffer.H"

class TextEdit: public TextDisp
{
public:
	int handle( int e );
	int handle_key();

	TextEdit( Position left, Position top, Position width, Position height,
	          Fl_Scrollbar* hScrollbar, Fl_Scrollbar* vScrollbar,
	          TextBuffer *buffer,
	          int continuousWrap, int wrapMargin );
private:
	char* mCopy;

	void WordForward( void );
	void WordBackward( void );

	int StartOfWord( int pos );
	int EndOfWord( int pos );

	static void s_ScrollTimeout( void* ptr );
	void ScrollTimeout( void );
};

#endif


