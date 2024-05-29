#include "bencode.h"
#include "utils.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  if (argc < 3)
    {
      fprintf (stderr, "Usage: your_bittorrent.sh <command> <args>\n");
      return 1;
    }

  setbuf (stderr, NULL);
  setbuf (stdout, NULL);

  const char *command = argv[1];

  if (strcmp (command, "decode") == 0)
    {

      const char *encoded_str = argv[2];

      bencode *decoded = decode_bencode (encoded_str);

      bencode_json (decoded);

      printf ("\n");

      encoded_str = encode_dict_bencode (decoded);

      printf ("encoded_str is %s\n", encoded_str);

      free ((void *)encoded_str);

      bencode_free (decoded);

      free (decoded);
    }
  else if (strcmp (command, "info") == 0)
    {

      /**
       * parse torrent file
       *
       * writing TRACKER URL key and value
       *
       * writing Length key and hist value
       */

      printf ("[your_program] starting\n");
      const char *file_name = argv[2];

      char *buf = write_file_content_to_buffer (file_name);
      printf ("[your_program] buf is writed\n");
      bencode *decoded = decode_bencode (buf);
      printf ("[your_program] decoded successfully and %p\n", decoded);

      printf ("[your program] decoded type is %d and %d size\n", decoded->type,
              decoded->raw_size);
      // bencode_json (decoded);
      bencode_print_dict_from_key (decoded, 1, "announce");
      bencode_print_dict_from_key (decoded, 2, "info", "length");
      bencode_free (decoded);

      free (decoded);

      free (buf);
    }
  else
    {
      fprintf (stderr, "Unknown command: %s\n", command);
      return 1;
    }

  return 0;
}
