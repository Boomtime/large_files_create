/*
 * create large empty files in Windows
 * only works on NTFS, probably
 */

#include <Windows.h>
#include <tchar.h>

#define DPRINT(x,...) _tprintf(_T(x) _T("\n"),__VA_ARGS__)


/* MongoDB maximum extents file size */
#define MONGODB_MAX_FILE_SIZE  2146435072

/* create a file handle for use as a large file read/write test */
static HANDLE createFile( const TCHAR* p_fn )
{
	return CreateFile(
		p_fn,
		FILE_READ_ACCESS | FILE_WRITE_ACCESS,
		0, /* share mode */
		NULL, /* security descriptor */
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
		0 /* template */
	);
}

/* set file size to p_size bytes beyond current file pointer position */
static void fileInflate( HANDLE p_file, LONGLONG p_size )
{
	LARGE_INTEGER lisize;

	lisize.QuadPart = p_size;
	SetFilePointerEx( p_file, lisize, 0, FILE_CURRENT );
    SetEndOfFile( p_file );
}

int _tmain( int argc, _TCHAR* argv[] )
{
	HANDLE fd;
	ULONGLONG fsize;
	TCHAR* end;
	int apos;

	DPRINT( "large_files_create:" );

	if( argc <= 1 )
	{
		DPRINT( " nothing to do... specify some file names perhaps" );
		return 1;
	}

	end = 0;
	fsize = _tcstoui64( argv[1], &end, 0 );

	/* check if no conversion */
	if( end == argv[1] )
	{
		fsize = MONGODB_MAX_FILE_SIZE;
		apos = 1;
		DPRINT( " size defaulting to [%i]", fsize );
	}
	else
	{
		apos = 2;
		DPRINT( " size parsed as [%i]", fsize );
	}

	DPRINT( " file count is [%i]", argc - apos );

	while( apos < argc )
	{
		if( _tcslen( argv[apos] ) > 0 )
		{
			DPRINT( " try create [%s]", argv[apos] );

			fd = createFile( argv[apos] );

			if( 0 == fd )
			{
				DPRINT( " FAILED [%i]", GetLastError() );
				return 2;
			}
			else
			{
				DPRINT( " file opened [%08X]", fd );

				fileInflate( fd, ULONGLONG(fsize) );
				DPRINT( " file inflated", fd );

				CloseHandle( fd );
			}
		}

		apos++;
	}

	return 0;
}

