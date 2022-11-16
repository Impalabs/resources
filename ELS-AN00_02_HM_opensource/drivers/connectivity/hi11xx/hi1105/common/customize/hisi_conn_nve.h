
#ifdef HISI_CONN_NVE_SUPPORT
#ifndef __HISI_CONN_NVE_H__
#define __HISI_CONN_NVE_H__

/* ����ͷ�ļ����� */
#include "plat_type.h"
#include "oam_wdk.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HISI_CONN_NVE_H

/* conn_nve������ض��� */
#define CONN_NVE_DEVICE_NAME        "/dev/block/by-name/hisi_conn_nve"
#define CRC32C_REV_SEED             0
/* hisi_conn_nve��С1MB */
#define CONN_NVE_SIZE               0x100000
/* ����ֵ���� */
typedef enum {
    CONN_NVE_RET_OK,
    CONN_NVE_ERROR_INIT,
    CONN_NVE_ERROR_WRITE,
    CONN_NVE_ERROR_READ,
    CONN_NVE_ERROR_SET_SIZE,
    CONN_NVE_ERROR_OVERSIZE,
    CONN_NVE_ERROR_NO_PART,
    CONN_NVE_ERROR_BUT,
} HISI_CONN_RET;

typedef enum {
    HISI_CONN_WIFI_DATA,
    HISI_CONN_BFGX_DATA,
    // ���������������
    HISI_CONN_PTABLE_BUT,
} HISI_CONN_PTABLE_ID;

typedef struct {
    uint32_t id;
    uint32_t start;
    uint32_t len;
} hisi_conn_particion_stru;
/* �������� */
/* conn_nve ���������ӿ� */
uint8_t hisi_conn_nve_setup_done(void);
int32_t hisi_conn_nve_alloc_ramdisk(void);
void hisi_conn_nve_free_ramdisk(void);
uint8_t* hisi_conn_nve_get_ramdisk(uint32_t particion_id);
uint32_t crc32c_conn_nve(uint32_t crc, const uint8_t *buf, unsigned long long len);
int32_t hisi_conn_nve_set_particion_size(uint32_t particion_id, uint32_t len);
int32_t hisi_conn_nve_read(uint32_t particion_id, size_t offset, size_t len, uint8_t *buf);
int32_t hisi_conn_nve_write(uint32_t particion_id, size_t offset, size_t len, uint8_t *buf);
#endif
#endif
