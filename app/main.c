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

  const char *command = argv[1];

  if (strcmp (command, "decode") == 0)
    {
      // You can use print statements as follows for debugging, they'll be
      // visible when running tests. printf("Logs from your program will appear
      // here!\n");

      // Uncomment this block to pass the first stage
      const char *encoded_str = argv[2];

      bencode *decoded = decode_bencode (encoded_str);

      bencode_json (decoded);

      printf ("\n");

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
      const unsigned char *file_name = argv[2];

      unsigned char *buf = write_file_content_to_buffer (file_name);

      bencode *decoded = decode_bencode (buf);
      if (decoded == NULL)
        {
          free (buf);
          return 0;
        }
      bencode_print_dict_from_key (decoded, 1, "announce");
      printf ("hello i am fuckign  ugly man which kill you\n");
      bencode_print_dict_from_key (decoded, 2, "info", "length");
      /* openssl sha hash *\          */
      bencode_dictionary *bdict = bencode_get_dict_from_key (decoded, "info");
      printf ("%d\n", bdict->raw_size);
      unsigned char *encoded_dict = encode_dict_bencode ((bencode *)bdict);

      unsigned char *hash = get_info_hash (encoded_dict, bdict->raw_size);
      print_info_hash (hash);
      free (hash);

      free (encoded_dict);
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
