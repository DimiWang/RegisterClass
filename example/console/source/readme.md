 bitfield input_json format [[output_file_name] [options]]

>input_json is a file with register structure

> format 
	* to_string
	  - default format @name=@value
	* to_string(format)
	  - available tags (name,purename, offset, value, hex, descr, extras , readonly, <cr>, <lf>, <tab>)
	* to_hex  
	  - is a hex format
	* to_u32
	  - unsigned 32 hex format 
	* to_u8
	  - unsigned 8 hex format
	* to_bits
	  - bits stream format

> options
	-t, --file-trim  maximum output lines -where the rest is chopped
	-e, --line-end  line ending '<cr>', '<lf>' or '<cr><lf>'
	


