/*
 * protocol_define.h
 *
 *  Created on: 2012-10-26
 *      Author: flx
 */

#ifndef __PROTOCOL_DEFINE_H__
#define __PROTOCOL_DEFINE_H__

/* protocol command set */
#define PCS_DISCOVER_DEVICE             0xE0
#define PCS_CLIENT_LOGIN                0xE1
#define PCS_UPLOAD_FILE                 0xE2
#define PCS_UPGRADE                     0xE3

/* protocol command set extend */
#define PCS_EX_OK 		                0xFE
#define PCS_EX_ERR		                0xFF
#define PCS_EX_01		                0x01
#define PCS_EX_02		                0x02
#define PCS_EX_03		                0x03
#define PCS_EX_04		                0x04
#define PCS_EX_05		                0x05


#define PROTOCOL_FILE_TYPE_ZIP          0X01
#define PROTOCOL_FILE_TYPE_SO           0X02
#define PROTOCOL_FILE_TYPE_LUA          0X03
#define PROTOCOL_FILE_TYPE_IRDA         0x04
#define PROTOCOL_FILE_TYPE_ZIP_IOS      0x05

#define PROTOCOL_DATA_SIZE_MAX          4096           /* 集控协议包的最大长度 */
#define PROTOCOL_DATA_LEN_MAX           (PROTOCOL_DATA_SIZE_MAX - 16)


#endif /* __PROTOCOL_DEFINE_H__ */
