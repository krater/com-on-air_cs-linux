CFLAGS=-Wall -O2 -I..
PROGS=coa_syncsniff pcap2cchan
PCAP_PROGS=pcapstein 
all:$(PROGS) $(PCAP_PROGS) dect_cli

dect_cli: 
	$(CC) $(CFLAGS) -lpcap dect_cli.c audioDecode.c codec/g721.c codec/g72x.c codec/g711.c -o dect_cli
$(PCAP_PROGS): $(foreach p,$(PCAP_PROGS), $p.c)
	$(CC) $(CFLAGS) -lpcap $@.c -o $@
clean:
	rm -f $(PROGS) $(PCAP_PROGS) dect_cli
