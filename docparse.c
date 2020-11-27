/// \section{Description} 
/// This is parser for inline documentation, primary for C an C{}\verb!++! documentation.
/// Unlike i.e. Doxygene, this is not for code documenatation but user documentation.

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "type.h"

#define	MAX_LINE_LEN      512
#define	MAX_REFS          128
#define	MAX_FILENAME_LEN   64
#define	DEFAULT_FILE       ""

#define	USE_SNUGSHADE (0)


#define	DOC_MARK  "///" /// \\ Basic format mark is tripple-slash (~w2 = "~q")
/// and rest of line will be used for generating \LaTeX\ documentation.
/// \\ Basic function is to copy text after mark into latex document.
/// Additional text filtering is used.

#define	err_printf(...) {printf("\r\nERROR: "); printf (__VA_ARGS__); printf("\r\n");}

static bool verbatim = false; // 
static bool cite = false;     //

typedef struct {
	ascii file[MAX_FILENAME_LEN];
} cfg_t;

static ascii *refs[MAX_REFS];

void usage()
{
	printf(	"\nusage: docparse [-f file]\n");
	printf(	"\t -h - print this help text\n");
	printf(	"\t -f file - file to read as references list\n");
	printf(	"\n");
}

void configure( cfg_t *cfg, int argc, char *argv[] )
{
	static struct option opt[] = {
		{ "file",    1, NULL, 'f' },
		{ "help",    0, NULL, 'h' },
		{ NULL,      0, NULL, 0 }
	};
	int	o;

	strncpy(cfg->file, DEFAULT_FILE, MAX_FILENAME_LEN);

	while ( (o = getopt_long( argc, argv, "f:h", opt, NULL )) != -1 )
	{
		switch ( o ) 
		{
		case 'h':
			usage ();
			exit( 0 );
			break;
		case 'f':
			strncpy(cfg->file, optarg, MAX_FILENAME_LEN);
			break;
		}
	}

}

/// \section{Text filtering}

/// \subsection{Replaced characters} 
void print_char(const ascii ch)
{	// {{{
	if (cite)
	{
		printf ("%c", ch);
		return; 
	}
	switch (ch)
	{
	/// Some characters must be replacet by mathematics syntax, so 
	case '<': /// ~s, 
	case '>': /// ~s
	case '=': /// and ~s 
		printf ("$%c$", ch); /// will be bounded by \$.
		break;
	/// Some other characters will be complemented by "backslash". They are: 
	case '_': /// ~s, 
	case '%': /// ~s,
	case '#': /// ~s
	case '&': /// and ~s
		printf("\\%c", ch);
		break;
		
	// anything else will be just copied
	default:
		printf ("%c", ch);
	}
}	// }}}

int print_ref (ascii *ref)
{	// {{{
	int ref_len=0;
	int i;

	for (i=0; i<32; i++)
	{
		if ((ref[i] == ' ') || (ref[i] == '\0'))
			break;
		ref_len++;
	}

	if (ref_len == 0)
		return (0); 
	
	for (i=0; i<MAX_REFS; i++)
	{
		if (refs[i] == NULL)
			return (ref_len);
		
		if (strncmp(refs[i]+4, ref, ref_len) == 0)
		{	// begin is the same
			if (*(refs[i]+4+ref_len) == '*')
			{	// and length is also the same, this is the one
				printf ("%s", refs[i]+4+ref_len+2);
				return (ref_len);
			}
		}
	}
	return (ref_len);
}	// }}}

void print_case_value (ascii *line)
{	// {{{
	int i;
	bool copy_ena = false;

	for (i=0; (i<MAX_LINE_LEN) && (line[i]!='\0'); i++)
	{
		if (! copy_ena)
		{
			if (strncmp(line+i, "case", 4) == 0)
			{
				copy_ena = true;
				i+=3;
			}
			continue;
		}
		if ((line[i] == ' ') || (line[i] == '\t'))
			continue;

		if (line[i] == ':')
			break;

		print_char(line[i]);
	}
}	// }}}

void print_quoted_value (ascii *line)
{	// {{{
	int i;
	bool copy_ena = false;

	for (i=0; (i<MAX_LINE_LEN) && (line[i] != '\0'); i++)
	{
		if (! copy_ena)
		{
			if (line[i] == '"')
			{
				copy_ena = true;
			}
			continue;
		}
		if (line[i] == '"')
			break;
		print_char(line[i]);
	}
}	// }}}

int print_word (ascii *line, int offset)
{	// {{{
	ascii *ptext;
	int i, result, n=0;
	bool copy_ena   = false;
	bool prev_space = true;

	// on the "offset" is number which one word to print
	ptext = line + offset;
	for (i=0; i<5; i++)
	{
		if ((*ptext < '0') || (*ptext > '9'))
			break;
		n=n*10;
		n+=(*ptext-'0');
		ptext++;
	}
	// now "n" is the order number of word we want
	if (n == 0)
		return (0);

	result = i;

	for (i=0; i<MAX_LINE_LEN; i++)
	{
		if ( // detect word separating chracter
			(line[i] == ' ')
		 || (line[i] == '\t')
		 || (line[i] == ',')
		)
		{
			if (n==0)
				break;

			prev_space = true;
			continue;
		}
		if (prev_space)
		{	// next word begin
			prev_space = false;
			if (--n == 0)
			{	// 
				copy_ena = true;
			}
		}

		if (copy_ena)
		{
			switch (line[i])
			{	// skip some characters which are unusual
			case '{':
			case '}':
			case '(':
			case ')':
				continue;
			}
			print_char(line[i]);
		}
	}
	return (result);
}	// }}}

void process_line (ascii *line)
{
	int i, tmp;
	int out=0;
	bool copy_ena=false;
	bool quote = false;
	#if USE_SNUGSHADE == 1
	bool shade_mode=false;
	#endif // USE_SNUGSHADE == 1

	for (i=0; i<MAX_LINE_LEN; i++)
	{
		if (! copy_ena)
		{
			if (line[i] == '\0')
			{
				if (cite)
					printf ("\r\n");
				return;
			}
			if (line[i] == '"')
			{
				quote = quote ? false : true;
			}

			if ((! quote) && (strncmp(line+i, DOC_MARK, 3) == 0))
			{
				copy_ena=true;
				cite=false;
				i+=2;
			}
			else if (cite)
			{	//
				switch (line[i])
				{
				case '\t':
					printf("    ");
					break;
				default:
					printf ("%c", line[i]);
				}
			}
			continue;
		}

		if (line[i] == '\0')
		{
			#if USE_SNUGSHADE == 1
			if (shade_mode)
				printf ("\\end{snugshade} ");
			#endif // USE_SNUGSHADE == 1
			printf ("\r\n");
			return;
		}
		/// \subsection{Special characters} 

		if (++out==1)
		{	/// First character after mark "///" could invoke replace by macro:
			///>
			switch (line[i])
			{
			case '*': ///* ~s =>
				printf ("\\");
				printf ("item "); /// \verb!\~q!
				continue;
			case '>': ///* ">" =>
				if (line[i+1] == '>')
					i++;
				printf ("\\");
				printf ("begin{list}{}{} "); /// \verb!\~q!
				continue;
			case '<': ///* "<" => 
				if (line[i+1] == '<')
					i++;
				printf ("\\");
				printf ("end{list} "); /// \verb!\~q!
				continue;
			case '}': ///* "\}" =>
				printf ("\\");
				printf ("begin{itemize} "); /// \verb!\~q!
				continue;
			case '{': ///* "\{" => 
				printf ("\\");
				printf ("end{itemize} "); /// \verb!\~q!
				continue;
			default:
				break;
			}
			///< 
		}
		if (line[i] == '\\')
		{	// some keyword detection
			if (strncmp("\\begin{verbatim}", line+i, 16) == 0)
			{
				verbatim = true;
			}
			else if (strncmp("\\end{verbatim}", line+i, 14) == 0)
			{
				verbatim = false;
			}
			else if (strncmp("\\section", line+i, 8) == 0)
			{
			#if USE_SNUGSHADE == 1
				printf ("\\begin{snugshade} ");
				shade_mode = true;
			#endif // USE_SNUGSHADE == 1
			}
		}

		if (line[i] == '`') /// Character '``' (back apostrophe)
		{	/// will start/stop 'verb!' environment:
			if (line[i+1] == '`')
			{
				printf ("`");
				i++;
				continue;
			}
			if (verbatim)
			{
				printf ("!");
				verbatim = false;
			}
			else
			{
				printf ("\\verb!");
				verbatim = true;
			}
			continue;
		}
		if (verbatim)
		{
			printf ("%c", line[i]);
			continue;
		}

		/// \subsection{References to code} 

		if (line[i] == '~') /// Special mark '$\sim$' (tilde) 
		{	/// will be replaced according to next character:
			///>> 
			i++;
			switch (line[i])
			{
			case '~':
				printf ("$\\sim$");
				break;

			case 'c': ///* "~~c" insert all next text up to next DOC_MARK (multiple lines)
				cite=true;
				break;

			case 'q': ///* "~~q" insert text which is in quotes on this line (before DOC_MARK)
				print_quoted_value(line);
				break;

			case 'r': ///* "~~r<REF>" = insert reference from references file (see \ref{REFFILE})
				i+=print_ref(line+i+1);
				break;

			case 's': ///* "~~s" insert value <val> from "case <val>:" from current line
				print_case_value(line);
				break;
			

			case 'w': ///* "~~w<N> insert <N>-th word from begin of line
				i+=print_word(line, i+1);
				break;
			}
			///<< 
			continue;
		}

		print_char(line[i]);
	}
}


/// \section{Reference file} \label{REFFILE} 
/// Used for references to other part of documentation.
void read_refs(ascii *filename)
{	// {{{
	FILE *fp;
	char *line = NULL;
	size_t buf_size = 0;
	u16 num_refs=0;
	int i, l;

	// vycistim prostor pro reference
	for (i=0; i<MAX_REFS; i++)
	{	
		refs[i]=NULL;
	}

	if (strlen(filename) < 1)
		return;

    if ((fp = fopen(filename, "r")) == NULL)
	{
    	err_printf("opening \"%s\" file.", filename);
    	exit(-1);
	}
	while (getline(&line, &buf_size, fp) >= 0)
	{	/// \\ Refernce file could be crearted using: 
		/// \begin{verbatim} cat <file>| grep -o "/\*~r.*\*~\*/" \end{verbatim}
		/// And each line will be:
		/// \begin{verbatim} *~r<N>*/"some text"/*~*/ \end{verbatim}
		l = strlen(line);
		if (l<12)
		{	// nesmyslna delka
			free(line);
			line=NULL;
			continue;
		}
		if (num_refs>=MAX_REFS)
			break;
		line[l-6] = '\0';
		refs[num_refs] = line;
		// printf ("\r\nREF%d,l=%d,\"%s\"", num_refs, l, refs[num_refs]);
		num_refs++;
		line=NULL;
	}
	fclose(fp);
}	// }}}

int main(int argc, char *argv[])
{
	ascii line[MAX_LINE_LEN];
	cfg_t cfg;
	int   ch;
 	int   s = 0;

	configure (&cfg, argc, argv );
	read_refs(cfg.file);

	while ((ch = getchar()) != EOF)
	{	// cteme stdin
		if ((ch=='\r') || (ch=='\n'))
			ch='\0'; // jakykoli konec radku prevadim na '\0'

		if (s>=MAX_LINE_LEN)
		{	// neni misto v bufferu
			// nebudu ten radek zpracovavat
			if (ch == '\0')
				s=0; // mame konec radku, zaciname znovu
			continue;
		}
		line[s] = ch;
		if (ch == '\0')
		{
			if (s)
			{
				process_line(line);
			}
			s=0;
			continue;
		}
 		s++;
	}

	return (0);
}
