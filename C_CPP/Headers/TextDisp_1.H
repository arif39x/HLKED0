/*******************************************************************************
*									       
* TextDisp.H - Display text from a text buffer				       
*									       
*
* The code in this file is based on code that makes part of the NEdit Version
* 5.0.2 from Fermi National Accelator Laboratory, written by Mark Edel. See     * LICENSE.FERMI for details. 
* LICENSE.FERMI for details.
*
* Maarten de Boer 1999
*
*******************************************************************************/

class TextBuffer;
class Selection;

enum cursorStyles {NORMAL_CURSOR, CARET_CURSOR, DIM_CURSOR, BLOCK_CURSOR,
                   HEAVY_CURSOR};

#define NO_HINT -1

#ifndef True
#define True 1
#define False 0
#endif

#include <FL/fl_draw.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Scrollbar.H>

typedef int Position;

typedef struct
{
	Fl_Color color;
	Fl_Font font;
	int	size;
}
styleTableEntry;

typedef void ( *unfinishedStyleCBProc ) ();

int inSelection( Selection *sel, int pos, int lineStartPos,
                 int dispIndex );

class TextDisp: public Fl_Widget
{
public:
	Position mLeft, mTop, mWidth, mHeight;

	int mCursorPos;
	int mCursorOn;
	int mCursorX, mCursorY;   		/* X, Y pos. of cursor for blanking */
	int mCursorToHint;   			/* Tells the buffer modified callback
	            		    					   where to move the cursor, to reduce
	            		    					   the number of redraw calls */
	int mCursorStyle;   			/* One of enum cursorStyles above */
	int mCursorPreferredCol;   		/* Column for vert. cursor movement */
	int mNVisibleLines;   			/* # of visible (displayed) lines */
	int mNBufferLines;   			/* # of newlines in the buffer */
	TextBuffer *mBuffer;        	    	/* Contains text to be displayed */
	TextBuffer *mStyleBuffer;      	    	/* Optional parallel buffer containing
	            		    	    	    	    	    	   color and font information */
	int mFirstChar, mLastChar;   		/* Buffer positions of first and last
	            		    					   displayed character (lastChar points
	            		    					   either to a newline or one character
	            		    					   beyond the end of the buffer) */
	int mContinuousWrap;        	    	/* Wrap long lines when displaying */
	int mWrapMargin;    	    	    	/* Margin in # of char positions for
	            		    	    	    	    	    	   wrapping in continuousWrap mode */
	int *mLineStarts;
	int mTopLineNum;   			/* Line number of top displayed line
	            		    					   of file (first line of file is 1) */
	int mHorizOffset;   			/* Horizontal scroll pos. in pixels */
	int mVisibility;   			/* Window visibility (see XVisibility
	            		    					   event) */
	int mNStyles;   			/* Number of entries in styleTable */
	styleTableEntry *mStyleTable;       	/* Table of fonts and colors for
	            		    	    	    	    	    	   coloring/syntax-highlighting */
	char mUnfinishedStyle;      	    	/* Style buffer entry which triggers
	            		    	    	    	    	    	   on-the-fly reparsing of region */
	unfinishedStyleCBProc		/* Callback to parse "unfinished" */
	mUnfinishedHighlightCB;     	/*     regions */
	void *mHighlightCBArg;      	    	/* Arg to unfinishedHighlightCB */
	Fl_Font mTextfont;   		/* Font */
	int mTextsize;
	Fl_Color mTextcolor;

	int mMaxsize;

	int mFixedFontWidth;   			/* Font width if all current fonts are
	            		    					   fixed and match in width, else -1 */
	Fl_Scrollbar* mHScrollBar;
	Fl_Scrollbar* mVScrollBar;


	Fl_Color mSelect_background;
	Fl_Color mSelect_foreground;

	Fl_Color mHighlight_background;
	Fl_Color mHighlight_foreground;

	Fl_Color mCursor_color;

	//    Pixel bgPixel, selectBGPixel;   	/* Background colors */
	//    Pixel highlightBGPixel;
	//    GC gc, selectGC, highlightGC;	/* GCs for drawing text */
	//    GC selectBGGC, highlightBGGC;	/* GCs for erasing text */
	//    GC cursorFGGC;			/* GC for drawing the cursor */
	//    GC styleGC;     	    	    	/* GC with color and font unspecified
	//    	    	    	    	    	   for drawing colored/styled text */

	/*textDisp *Create(Widget widget, Widget hScrollBar, Widget vScrollBar,
		Position left, Position top, Position width, Position height,
		TextBuffer *buffer, XFontStruct *fontStruct, Pixel bgPixel,
		Pixel fgPixel, Pixel selectFGPixel, Pixel selectBGPixel,
		Pixel highlightFGPixel, Pixel highlightBGPixel, Pixel cursorFGPixel,
		int continuousWrap, int wrapMargin);
	*/

	TextDisp( Position left, Position top, Position width, Position height,
	          Fl_Scrollbar* hScrollbar, Fl_Scrollbar* vScrollbar,
	          TextBuffer *buffer,
	          int continuousWrap, int wrapMargin );

	~TextDisp();

	int damage_range_start, damage_range_end;
	int damage_cursor_x, damage_cursor_y;

	void draw( void );


	virtual int handle( int )
	{
		return 0;
	}

	void resize( int X, int Y, int W, int H );

	void SetBuffer( TextBuffer *buffer );
	TextBuffer *GetBuffer();
	void AttachHighlightData( TextBuffer *styleBuffer,
	                          styleTableEntry *styleTable, int nStyles, char unfinishedStyle,
	                          unfinishedStyleCBProc unfinishedHighlightCB, void *cbArg );
	//void SetFont( XFontStruct *fontStruct);

	void SetFont( Fl_Font font, int size );

	void Resize( int width, int height )
	{
		size( width, height );
	}

	void RedisplayRect( int left, int top, int width,
	                    int height );
	void RedisplayRect_( int left, int top, int width,
	                     int height );
	void RedisplayRange( int start, int end );
	void RedisplayRange_( int start, int end );
	void SetScroll( int topLineNum, int horizOffset );
	void GetScroll( int *topLineNum, int *horizOffset );
	void Insert( const char *text );
	void Overstrike( const char *text );
	void SetInsertPosition( int newPos );
	int GetInsertPosition();
	int XYToPosition( int x, int y );
	void XYToUnconstrainedPosition( int x, int y, int *row,
	                                int *column );
	int OffsetWrappedColumn( int row, int column );
	int OffsetWrappedRow( int row );
	int PositionToXY( int pos, int *x, int *y );
	int PosToLineAndCol( int pos, int *lineNum, int *column );
	int InSelection( int x, int y );
	void MakeInsertPosVisible();
	int MoveRight();
	int MoveLeft();
	int MoveUp();
	int MoveDown();
	void BlankCursor();
	void UnblankCursor();
	void SetCursorStyle( int style );
	void SetWrapMode( int wrap, int wrapMargin );
	int EndOfLine( int pos, int startPosIsLineStart );
	int StartOfLine( int pos );
	int CountForwardNLines( int startPos, int nLines,
	                        int startPosIsLineStart );
	int CountBackwardNLines( int startPos, int nLines );
	int CountLines( int startPos, int endPos,
	                int startPosIsLineStart );

private:

	void updateLineStarts( int pos, int charsInserted,
	                       int charsDeleted, int linesInserted, int linesDeleted, int *scrolled );
	void offsetLineStarts( int newTopLineNum );
	void calcLineStarts( int startLine, int endLine );
	void calcLastChar();
	int posToVisibleLineNum( int pos, int *lineNum );
	void redisplayLine( int visLineNum, int leftClip,
	                    int rightClip, int leftCharIndex, int rightCharIndex );
	void drawString( int style, int x, int y, int toX,
	                 char *string, int nChars );
	void clearRect( int style, int x, int y,
	                int width, int height );
	void drawCursor( int x, int y );
	void drawCursor_( int x, int y );
	int styleOfPos( int lineStartPos,
	                int lineLen, int lineIndex, int dispIndex );
	int stringWidth( char *string, int length, int style );
	int xyToPos( int x, int y, int posType );
	void xyToUnconstrainedPos( int x, int y, int *row,
	                           int *column, int posType );
	static void bufModifiedCB( int pos, int nInserted, int nDeleted,
	                           int nRestyled, char *deletedText, void *cbArg );
	void setScroll( int topLineNum, int horizOffset,
	                int updateVScrollBar, int updateHScrollBar );
	//void hScrollCB(Widget w, XtPointer clientData, XtPointer callData);
	//void vScrollCB(Widget w, XtPointer clientData, XtPointer callData);

	static void hScrollCB( Fl_Scrollbar* w, TextDisp* d );
	static void vScrollCB( Fl_Scrollbar* w, TextDisp* d );

	void updateVScrollBarRange();
	int updateHScrollBarRange();
	int measureVisLine( int visLineNum );
	int emptyLinesVisible();
	void blankCursorProtrusions();
	void blankCursorProtrusions_();
	void resetClipRectangles();
	int visLineLength( int visLineNum );
	void findWrapRange( char *deletedText, int pos,
	                    int nInserted, int nDeleted, int *modRangeStart, int *modRangeEnd,
	                    int *linesInserted, int *linesDeleted );
	void wrappedLineCounter( TextBuffer *buf, int startPos,
	                         int maxPos, int maxLines, int startPosIsLineStart, int *retPos,
	                         int *retLines, int *retLineStart, int *retLineEnd );
	void findLineEnd( int startPos, int startPosIsLineStart,
	                  int *lineEnd, int *nextLineStart );
	int wrapUsesCharacter( int lineEndPos );
	void hideOrShowHScrollBar();
	void extendRangeForStyleMods( int *start, int *end );

	void Resize_( int width, int height );

};
