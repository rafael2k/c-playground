#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nanorq.h>

void dump_esi(nanorq *rq, struct ioctx *myio, FILE *oh, uint8_t sbn,
              uint32_t esi) {
  uint32_t tag = nanorq_tag(sbn, esi);
  size_t packet_size = nanorq_symbol_size(rq);
  uint8_t data[packet_size];
  memset(data, 0, packet_size);
  uint64_t written = nanorq_encode(rq, (void *)data, esi, sbn, myio);

  if (written != packet_size) {
    fprintf(stdout, "failed to encode packet data for sbn %d esi %d.", sbn,
            esi);
    abort();
  } else {
    fwrite(&tag, 1, sizeof(tag), oh);
    fwrite(data, 1, packet_size, oh);
  }
}

void dump_block(nanorq *rq, struct ioctx *myio, FILE *oh, uint8_t sbn) {
  float expected_loss = 6.0;
  int overhead = 5;

  uint32_t num_esi = nanorq_block_symbols(rq, sbn);
  int num_dropped = 0, num_rep = 0;
  for (uint32_t esi = 0; esi < num_esi; esi++) {
    float dropped = ((float)(rand()) / (float)RAND_MAX) * (float)100.0;
    float drop_prob = expected_loss;
    if (dropped < drop_prob) {
      num_dropped++;
    } else {
      dump_esi(rq, myio, oh, sbn, esi);
    }
  }
  for (uint32_t esi = num_esi; esi < num_esi + num_dropped + overhead; esi++) {
    dump_esi(rq, myio, oh, sbn, esi);
    num_rep++;
  }
  nanorq_encoder_cleanup(rq, sbn);
  fprintf(stdout, "block %d is %d packets, dropped %d, created %d repair\n",
          sbn, num_esi, num_dropped, num_rep);
}

void usage(char *prog) {
  fprintf(stdout, "usage:\n%s <filename> <packet_size>\n", prog);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    usage(argv[0]);

  char *infile = argv[1];
  struct ioctx *myio = ioctx_from_file(infile, 1);
  if (!myio) {
    fprintf(stdout, "couldnt access file %s\n", infile);
    return -1;
  }

  size_t filesize = myio->size(myio);

  // determine chunks, symbol size, memory usage from size
  size_t packet_size = strtol(argv[2], NULL, 10); // T
  uint8_t align = 1;

  srand((unsigned int)time(0));

  nanorq *rq = nanorq_encoder_new(filesize, packet_size, align);

  if (rq == NULL) {
    fprintf(stdout, "Could not initialize encoder.\n");
    return -1;
  }

  int num_sbn = nanorq_blocks(rq);
  printf("sbn = %d\n", num_sbn);
  for (int b = 0; b < num_sbn; b++) {
    nanorq_generate_symbols(rq, b, myio);
  }

  uint64_t oti_common = nanorq_oti_common(rq);
  uint32_t oti_scheme = nanorq_oti_scheme_specific(rq);
  FILE *oh = fopen("data.rq", "w+");
  fwrite(&oti_common, 1, sizeof(oti_common), oh);
  fwrite(&oti_scheme, 1, sizeof(oti_scheme), oh);


  printf("size oti_common: %lu %lu\n", sizeof(oti_common), oti_common);
  printf("size oti_scheme: %lu %u\n", sizeof(oti_scheme), oti_scheme);

  for (int sbn = 0; sbn < num_sbn; sbn++) {
    dump_block(rq, myio, oh, sbn);
  }
  fclose(oh);

  nanorq_free(rq);
  myio->destroy(myio);

  return 0;
}
