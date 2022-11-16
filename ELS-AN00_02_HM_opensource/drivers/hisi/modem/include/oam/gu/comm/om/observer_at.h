

#ifndef __OBSERVER_AT_H__
#define __OBSERVER_AT_H__

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)

/*
 * ����˵��: SPY��AT��Ϣ�ӿ�ö��
 */
enum TEMPPRT_AT_MsgId {
    /* SPY -> AT */
    ID_TEMPPRT_AT_EVENT_IND = 0,    /* _H2ASN_MsgChoice TEMP_PROTECT_EventAtInd */

    ID_TEMPPRT_STATUS_AT_EVENT_IND, /* _H2ASN_MsgChoice TEMP_PROTECT_EventAtInd */

    ID_TEMPPRT_AT_MSG_ID_BUTT

};
typedef VOS_UINT32 TEMPPRT_AT_MsgIdUint32;
/*
 * ����˵��: SPY��AT�ϱ�����Ϣ�ṹ
 */
typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32                  msgId;
    VOS_UINT32                  tempProtectEvent;
    VOS_UINT32                  tempProtectParam;
} TEMP_PROTECT_EventAtInd;

typedef struct {
    VOS_UINT32 isEnable;           /* �Ƿ���Ҫ������������, 0Ϊ�رգ�1Ϊ���� */
    VOS_INT32  closeAdcThreshold;  /* ����͹���ģʽ���¶����� */
    VOS_INT32  alarmAdcThreshold;  /* ��Ҫ���и����������¶����� */
    VOS_INT32  resumeAdcThreshold; /* �ָ�������ģʽ���¶����� */
} SPY_TempThresholdPara;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
