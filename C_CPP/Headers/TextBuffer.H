
#ifndef __TEXTBUF__
#define __TEXTBUF__

/*******************************************************************************
*
* TextBuffer.H - Manage source text for one or more text areas
*
* The code in this file is based on code that makes part of the NEdit Version
* 5.0.2 from Fermi National Accelator Laboratory, written by Mark Edel. See
* LICENSE.FERMI for details.
*
* Maarten de Boer 1999
* 
*******************************************************************************/

/* Maximum length in characters of a tab or control character expansion
   of a single buffer character */
#define MAX_EXP_CHAR_LEN 20

#define XtFree(var) delete [] var
#define XtMalloc(var) new char[var]


#ifndef True
#define True 1
#define False 0
#endif

class Selection
{
public:
	char mSelected;
	char mRectangular;
	int mStart;
	int mEnd;
	int mRectStart;
	int mRectEnd;

	void SetSelection( int start, int end );
	void SetRectSelect( int start, int end,
	                    int rectStart, int rectEnd );
	void UpdateSelection( int pos, int nDeleted, int nInserted );
	int GetSelectionPos( int *start, int *end );
	int GetSelectionPos( int *start, int *end,
	                     int *isRect, int *rectStart, int *rectEnd );
};

typedef void ( *bufModifyCallbackProc ) ( int pos, int nInserted, int nDeleted,
        int nRestyled, char *deletedText, void *cbArg );

class TextBuffer
{
public:
	int mLength;    	        /* length of the text in the buffer (the length
	            		                                   of the buffer itself must be calculated:
	            		                                   gapEnd - gapStart + length) */
	char *mBuf;                     /* allocated memory where the text is stored */
	int mGapStart;     	        /* points to the first character of the gap */
	int mGapEnd;                    /* points to the first char after the gap */
	Selection mPrimary;   		/* highlighted areas */
	Selection mSecondary;
	Selection mHighlight;
	int mTabDist;   		/* equiv. number of characters in a tab */
	int mUseTabs;   		/* True if buffer routines are allowed to use
	            		    				   tabs for padding in rectangular operations */
	int mNModifyProcs;   		/* number of modify-redisplay procs attached */
	bufModifyCallbackProc	/* procedures to call when buffer is */
	*mNodifyProcs;   	/*    modified to redisplay contents */
	void **mCbArgs;   		/* caller arguments for modifyProcs above */
	int mCursorPosHint;   		/* hint for reasonable cursor position after
	            		    				   a buffer modification operation */
	char mNullSubsChar;   	    	/* NEdit is based on C null-terminated strings,
	            		    	    	    	    	   so ascii-nul characters must be substituted
	            						   with something else.  This is the else, but
	            						   of course, things get quite messy when you
	            						   use it */

	TextBuffer( int requestedSize = 0 );
	~TextBuffer();

	int Length()
	{
		return mLength;
	}

	char * GetAll();
	void SetAll( char *text );
	char * GetRange( int start, int end );
	char GetCharacter( int pos );
	char * GetTextInRect( int start, int end,
	                      int rectStart, int rectEnd );
	void Insert( int pos, const char *text );
	void Remove( int start, int end );
	void Replace( int start, int end, const char *text );
	void CopyFromBuf( TextBuffer *fromBuf, int fromStart,
	                  int fromEnd, int toPos );
	void InsertCol( int column, int startPos, const char *text,
	                int *charsInserted, int *charsDeleted );
	void ReplaceRect( int start, int end, int rectStart,
	                  int rectEnd, const char *text );
	void RemoveRect( int start, int end, int rectStart,
	                 int rectEnd );
	void OverlayRect( int startPos, int rectStart,
	                  int rectEnd, char *text, int *charsInserted, int *charsDeleted );
	void ClearRect( int start, int end, int rectStart,
	                int rectEnd );
	int GetTabDistance();
	void SetTabDistance( int tabDist );
	void Select( int start, int end );
	void Unselect();
	void RectSelect( int start, int end, int rectStart,
	                 int rectEnd );
	int GetSelectionPos( int *start, int *end );
	int GetSelectionPos( int *start, int *end,
	                     int *isRect, int *rectStart, int *rectEnd );
	char * GetSelectionText();
	void RemoveSelected();
	void ReplaceSelected( char *text );
	void SecondarySelect( int start, int end );
	void SecondaryUnselect();
	void SecRectSelect( int start, int end,
	                    int rectStart, int rectEnd );
	int GetSecSelectPos( int *start, int *end,
	                     int *isRect, int *rectStart, int *rectEnd );
	char * GetSecSelectText();
	void RemoveSecSelect();
	void ReplaceSecSelect( char *text );
	void Highlight( int start, int end );
	void Unhighlight();
	void RectHighlight( int start, int end,
	                    int rectStart, int rectEnd );
	int GetHighlightPos( int *start, int *end,
	                     int *isRect, int *rectStart, int *rectEnd );
	char * GetHighlightText();
	void AddModifyCB( bufModifyCallbackProc bufModifiedCB,
	                  void *cbArg );
	void RemoveModifyCB( bufModifyCallbackProc bufModifiedCB,
	                     void *cbArg );
	char * GetLineText( int pos );
	int StartOfLine( int pos );
	int EndOfLine( int pos );
	int GetExpandedChar( int pos, int indent, char *outStr );
	static int ExpandCharacter( char c, int indent, char *outStr, int tabDist,
	                            char nullSubsChar );
	static int CharWidth( char c, int indent, int tabDist, char nullSubsChar );
	int CountDispChars( int lineStartPos, int targetPos );
	int CountForwardDispChars( int lineStartPos, int nChars );
	int CountLines( int startPos, int endPos );
	int CountForwardNLines( int startPos, int nLines );
	int CountBackwardNLines( int startPos, int nLines );
	int SearchForward( int startPos, char *searchChars,
	                   int *foundPos );
	int SearchBackward( int startPos, char *searchChars,
	                    int *foundPos );
	int SubstituteNullChars( char *string, int length );
	void UnsubstituteNullChars( char *string );

private:
	int insert( int pos, const char *text );
	void delet( int start, int end );
	void deleteRect( int start, int end, int rectStart,
	                 int rectEnd, int *replaceLen, int *endPos );
	void insertCol( int column, int startPos, const char *insText,
	                int *nDeleted, int *nInserted, int *endPos );
	void overlayRect( int startPos, int rectStart,
	                  int rectEnd, const char *insText, int *nDeleted, int *nInserted,
	                  int *endPos );
	void callModifyCBs( int pos, int nDeleted,
	                    int nInserted, int nRestyled, char *deletedText );
	void redisplaySelection( Selection *oldSelection,
	                         Selection *newSelection );
	void moveGap( int pos );
	void reallocateBuf( int newGapStart, int newGapLen );
	char * getSelectionText( Selection *sel );
	void removeSelected( Selection *sel );
	void replaceSelected( Selection *sel, char *text );
	int searchForward( int startPos, char searchChar,
	                   int *foundPos );
	int searchBackward( int startPos, char searchChar,
	                    int *foundPos );
	void findRectSelBoundariesForCopy( int lineStartPos,
	                                   int rectStart, int rectEnd, int *selStart, int *selEnd );
	void updateSelections( int pos, int nDeleted,
	                       int nInserted );

};

#endif
