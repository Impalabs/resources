/*
 * ��Ȩ���� (c) ��Ϊ�������޹�˾ 2020-2020
 * ����˵�� : host/device У׼����ʹ��ͷ�ļ�
 */

#ifndef __WLAN_CALI_H__
#define __WLAN_CALI_H__

#if defined (_PRE_PRODUCT_ID_HI110X_HOST) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_DEV)
#include "1103/wlan_cali_1103.h"
#endif
#if defined (_PRE_PRODUCT_ID_HI110X_HOST) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1105_DEV)
#include "1105/wlan_cali_1105.h"
#endif
#if defined (_PRE_PRODUCT_ID_HI110X_HOST) || (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1106_DEV) || \
    (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_BISHENG_DEV)
#include "1106/wlan_cali_1106.h" /* 1106/bisheng У׼���ݸ�ʽ��ͬ��bishengʹ��1106У׼���ݽṹ */
#include "1106/wlan_private_1106.h"
#endif

#endif /* end of wlan_cali.h */
