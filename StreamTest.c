#include  "Stream.h"
#include <assert.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

void PrintLine(const char* source, int pos, int col)
{
	const char* pHead = source + pos;
	while (pHead > source)
	{
		if (*pHead == '\n')
		{
			pHead++;
			break;
		}
		pHead--;
	}

	const char* pTail = source + pos;
	while (*pTail)
	{
		if (*pTail == '\n')
			break;
		pTail++;
	}
	//for (const char* psz = pHead; psz != pTail; psz++)
	//{
	   UINT oldcp = GetConsoleOutputCP();
	    SetConsoleOutputCP(CP_UTF8);
		printf("%.*s\n", pTail - pHead, pHead);
		for (int i = 1; i < pTail - pHead; i++)
		{
			if (i < col)
				printf(" ");			
			else
			{
				printf("^");
				break;
			}
		}
		
		SetConsoleOutputCP(oldcp);

	//}
}


void Test3()
{
    struct Stream stream = STREAM_INIT;

    assert(stream.CurrentCol == 0);
    assert(stream.CurrentLine == 0);
    assert(stream.CurrentChar == 0);
    assert(stream.NextBytePos == 0);
    assert(stream.data == NULL);

    //Stream_Set(&stream, u8"maçã");
    if (Stream_Set(&stream, u8"ábç\n dêf\n"))
    {
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 0);
        assert(stream.CurrentChar == 0);
        assert(stream.NextBytePos == 0);
        assert(stream.data != NULL);


        Stream_Match(&stream);

        assert(stream.CurrentCol == 1);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'á');
        assert(stream.NextBytePos == 2);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 2);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'b');
        assert(stream.NextBytePos == 3);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 3);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'ç');
        assert(stream.NextBytePos == 5);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 2);
        assert(stream.CurrentChar == L'\n');
        assert(stream.NextBytePos == 6);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 1);
        assert(stream.CurrentLine == 2);
        assert(stream.CurrentChar == L' ');
        assert(stream.NextBytePos == 7);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 2);
        assert(stream.CurrentLine == 2);
        assert(stream.CurrentChar == L'd');
        assert(stream.NextBytePos == 8);

		PrintLine(stream.data, stream.CurrentBytePos, stream.CurrentCol);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 3);
        assert(stream.CurrentLine == 2);
        assert(stream.CurrentChar == L'ê');
        assert(stream.NextBytePos == 10);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 2);
        assert(stream.CurrentChar == L'f');
        assert(stream.NextBytePos == 11);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 3);
        assert(stream.CurrentChar == L'\n');
        assert(stream.NextBytePos == 12);

    }

    Stream_Close(&stream);
}



void Test1()
{
    struct Stream stream = STREAM_INIT;

    //Stream_Set(&stream, u8"maçã");
    if (Stream_Open(&stream, "utf8maca.txt"))
    {
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 0);
        assert(stream.CurrentChar == 0);
        assert(stream.NextBytePos == 0);
        assert(stream.data != NULL);


        Stream_Match(&stream);

        assert(stream.CurrentCol == 1);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == 'm');
        assert(stream.CurrentBytePos == 0);
        assert(stream.NextBytePos == 1);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 2);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == 'a');        
        assert(stream.CurrentBytePos == 1);
        assert(stream.NextBytePos == 2);

        assert(Stream_LookAhead(&stream) == L'ç');

        Stream_Match(&stream);
        assert(stream.CurrentCol == 3);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'ç');
        assert(stream.CurrentBytePos == 2);
        assert(stream.NextBytePos == 4);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'ã');

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == (wchar_t)EOF);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == (wchar_t)EOF);
    }

    Stream_Close(&stream);
}


void Test1L()
{
    struct Stream stream = STREAM_INIT;

    assert(stream.CurrentCol == 0);
    assert(stream.CurrentLine == 0);
    assert(stream.CurrentChar == 0);
    assert(stream.NextBytePos == 0);
    assert(stream.data == NULL);


    if (Stream_Set(&stream, u8"maçã"))
    {
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 0);
        assert(stream.CurrentChar == 0);
        assert(stream.NextBytePos == 0);
        assert(stream.data != NULL);


        Stream_Match(&stream);

        assert(stream.CurrentCol == 1);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == 'm');
        assert(stream.NextBytePos == 1);

        Stream_Match(&stream);
        assert(stream.CurrentCol == 2);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == 'a');

        assert(Stream_LookAhead(&stream) == L'ç');

        Stream_Match(&stream);
        assert(stream.CurrentCol == 3);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'ç');

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == L'ã');

        Stream_Match(&stream);
        assert(stream.CurrentCol == 4);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == (wchar_t)EOF);
    }

    Stream_Close(&stream);
}

void Test2()
{
    struct Stream stream = STREAM_INIT;
    
    if (Stream_Open(&stream, "bomutf8empty.txt"))
    {
        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 0);
        assert(stream.CurrentChar == 0);
        assert(stream.NextBytePos== 0);

        Stream_Match(&stream);

        assert(stream.CurrentCol == 0);
        assert(stream.CurrentLine == 1);
        assert(stream.CurrentChar == WEOF);
        assert(stream.NextBytePos == 0);
    }

    Stream_Close(&stream);
}

void Test0()
{
    struct Stream stream = STREAM_INIT;

    assert(stream.CurrentCol == 0);
    assert(stream.CurrentLine == 0);
    assert(stream.CurrentChar == 0);

    Stream_Match(&stream);

    Stream_Close(&stream);
}



int main()
{
    //_setmode(_fileno(stdout), _O_U16TEXT);
    
    Test0();
    Test1();
    Test1L();
    Test2();
    Test3();


   //wprintf(L"maçã");
}


