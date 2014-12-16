#include "support.h"
#include "pktbuff.h"

const char* mystr = "Hello world";
#define STR_SIZE 64

struct netfpga_to_driver 
{
 char str[STR_SIZE];
};

#define PKT_SIZE (sizeof(struct ioq_header) + sizeof(struct netfpga_to_driver))

int main() 
{
  // only run this program on thread 0
  if (nf_tid() != 0) 
  {
      while (1) {}
  }

  // initialize
  nf_pktout_init();
  nf_pktin_init();

  // allocate an output buffer
  t_addr *pkt = nf_pktout_alloc(PKT_SIZE);

  // setup the ioq_header
  fill_ioq((struct ioq_header*)pkt, 2, sizeof(struct netfpga_to_driver));

  // get a pointer to the payload struct
  struct netfpga_to_driver* n2d = 
   (struct netfpga_to_driver*) (pkt + sizeof(struct ioq_header));

  // initialize the message
  memset(n2d->str, 0, STR_SIZE); 
  memcpy(n2d->str, mystr, strlen(mystr));
  n2d->str[strlen(mystr)+1] = nf_tid() + 0x30; //thread id in ascii

  // send it
  nf_pktout_send(pkt, pkt + PKT_SIZE); 
  return 0;
}
