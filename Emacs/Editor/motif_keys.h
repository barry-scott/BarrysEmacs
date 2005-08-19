// keyboard conversion table
class EmacsMotifKeySymMap
	{
public:
	EmacsMotifKeySymMap();
	~EmacsMotifKeySymMap();

	bool init();

	const unsigned char *lookup( KeySym k, int modifiers ) const;

private:
	int getentry();

	// functions
	int kf_getch();
	int kf_getns();
	void kf_ungetch();
	int gettoken( int &integer_result, EmacsString &string_result );


	// variables
	unsigned char *buffer;
	unsigned char *get_ptr;
	unsigned char *buffer_end;

	int linenr;
	unsigned char *line_start;

	enum { max_map_entries = 256 };

	struct keysym_map
		{
		enum { max_keymap_entry_size = 8 };
		// variables
		KeySym key_code;
		unsigned char translation[max_keymap_entry_size];
		unsigned char enh_translation[max_keymap_entry_size];
		unsigned char shift_translation[max_keymap_entry_size];
		unsigned char ctrl_translation[max_keymap_entry_size];
		unsigned char ctrl_shift_translation[max_keymap_entry_size];
		};
	int map_entries;
	struct keysym_map map[max_map_entries];
	};
