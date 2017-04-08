#ifndef __PROTOCOL_CORE_H__
#define __PROTOCOL_CORE_H__

/* protocol data unit */
typedef struct __PDU
{
#define PROTO_DATA_LEN_MAX     4096

	unsigned char  cmd;
	unsigned char  cmd_ex;
	unsigned char *data; //[PROTO_DATA_LEN_MAX];
	int            data_len;
	unsigned char  checksum1;
	unsigned char  checksum2;
	int            b_enable;    /* 这个包是否有效 */
}PDU;


extern int parse_packet2(const unsigned char *in_buffer, int data_len, PDU *lpPdu);

extern unsigned create_protocol_packet(unsigned char cmd, unsigned char cmd_ex,
		                               const unsigned char *data, unsigned int data_len,
                                       unsigned char *out_buffer, unsigned int buffer_size);

extern void assemble_protocol_packet(const PDU *lpPdu, unsigned char *out_buffer, int *size);

#endif  /* __PROTOCOL_CORE_H__ */
