//
//	ext_func.cpp
//
//		copyright (c) 1996 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

/*forward*/ int list_images( void );

Expression *call_back_result;
unsigned char *call_back_str;
unsigned int call_back_len;

EmacsStringTable EmacsExternImage::name_table;

EmacsExternImage::EmacsExternImage( const EmacsString &image, const EmacsString &filename )
	: ref_count(0)
	, ef_image( image )
	, ef_filename( filename )
	, ef_os_info( make_EmacsExternImageOsInfo() )
	{
	add( image, this );
	}

EmacsExternImage::~EmacsExternImage()
	{
	delete ef_os_info;
	ef_os_info = NULL;
	}

EmacsExternImageOsInfo::EmacsExternImageOsInfo()
	{
	}

EmacsExternImageOsInfo::~EmacsExternImageOsInfo()
	{
	}

int list_images( void )
	{
	EmacsBufferRef old( bf_cur );
	EmacsBuffer::scratch_bfn( "Image list", interactive() );

	int num_images = EmacsExternImage::entries();
	if( num_images == 0 )
		bf_cur->ins_str( u_str("No images in use.\n") );
	else
		{
		bf_cur->ins_str("   Image Name\t\t\t     Context Value  Filename\n"
			"   ----- ----\t\t\t     ------- -----  --------\n" );

		for( int index=0; index<num_images; index++ )
			{
			EmacsExternImage *i = EmacsExternImage::value( index );
			bf_cur->ins_cstr( FormatString("   %-33s 0x%X     %s\n" ) <<
				i->ef_image << int(i->ef_context) << i->ef_filename );
			}
		}
	bf_cur->b_modified = 0;
	set_dot( 1 );
	old.set_bf();
	theActiveView->window_on( bf_cur);

	return 0;
	}

