/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 */
#ifndef __MDRV_CHR_H__
#define __MDRV_CHR_H__


#define CHR_ALARM_ID_ERR_LOG_IND   (0x0A)
#define CHR_MOUDLE_ID_DRV          (0x020067)
#define CHR_MOUDLE_ID_DRV_NR       (0x020085)

typedef unsigned int (*chr_drv_ind_func_t)(void *pData, unsigned int ulLen);

typedef enum {
    CHR_DRV_WAKE_LOCK_ALARMID = 0, /* ����͹����ϱ� */
    CHR_DRV_SOCP_DST_FULL_ALARMID = 1, /* socp dest buffer full,report event */
} chr_om_drv_alarmid_e;

typedef struct {
    unsigned int moduleid;
    unsigned short faultid; /* �����ϱ�ʱ������ע */
    unsigned short alarmid;
    unsigned short alarm_type;
    unsigned short resv;
    unsigned int msg_type;
    unsigned int msgsn;
    unsigned int msglen;
} chr_drv_ind_s;

/*****************************************************************************
 * �� �� ��  : mdrv_chr_register_callback
 *
 * ��������  : ���ӿ�����ע��CHR�Ļص�����
 *
 * �������  : omChrFuncPtr
 * �������  : ��
 *
 * �� �� ֵ  : NA
 *
 * �޸ļ�¼  :
 *
 *****************************************************************************/
int  mdrv_chr_drv_register_callback(chr_drv_ind_func_t omChrFuncPtr);


#endif

