

#ifndef __HMAC_TEST_MAIN_H__
#define __HMAC_TEST_MAIN_H__

/* 1 ����ͷ�ļ����� */
#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "hal_ext_if.h"
#ifdef _PRE_WLAN_ALG_ENABLE
#include "alg_ext_if.h"
#endif
#include "hmac_ext_if.h"
#include "hmac_vap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_TEST_MAIN_H

/* 2 �궨�� */
/* ����оƬ��֤����ǰ����鿪�� */
#define hmac_chip_test_call(func_call)

/* 3 ö�ٶ��� */
/* 4 ȫ�ֱ������� */
/* 5 ��Ϣͷ���� */
/* 6 ��Ϣ���� */
/* 7 STRUCT���� */
/*
 * �ṹ˵��:����Ϊ���Ե��������ṹ������ͳһ�������в������õ��ĸ��ֿ��ƿ��أ���������������
 * ������Ҫ�����������ӽṹ��Ա�����Ǳ���
 */
typedef struct dmac_test_mn {
    oal_uint8 uc_hmac_chip_test;
    oal_uint8 auc_pad[3];
} hmac_test_mng_stru;
/* 8 UNION���� */
/* 9 OTHERS���� */
extern hmac_test_mng_stru g_st_hmac_test_mng;

/* 10 �������� */
extern oal_void hmac_test_init(oal_void);
extern oal_void hmac_test_set_chip_test(oal_uint8 uc_chip_test_open);
extern oal_uint32 hmac_test_get_chip_test(oal_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of __HMAC_TEST_MAIN_H__ */
