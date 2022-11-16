

/* ͷ�ļ����� */
#include "oal_ext_if.h"
#include "oal_bus_if.h"
#include "oam_ext_if.h"

/* ʵ��chip���� */
OAL_STATIC uint8_t g_bus_chip_num = 0;

/*
 * �� �� ��  : oal_bus_get_chip_num
 * ��������  : ��ȡӲ�����߽ӿڸ���
 * �� �� ֵ  : chip ����
 */
uint8_t oal_bus_get_chip_num(void)
{
    return g_bus_chip_num;
}

/*
 * �� �� ��  : oal_bus_inc_chip_num
 * ��������  : ��ȡӲ�����߽ӿڸ���
 * �� �� ֵ  : chip ����
 */
uint32_t oal_bus_inc_chip_num(void)
{
    if (g_bus_chip_num < WLAN_CHIP_MAX_NUM_PER_BOARD) {
        g_bus_chip_num++;
    } else {
        oal_io_print("oal_bus_inc_chip_num FAIL: g_bus_chip_num = %d\n", g_bus_chip_num);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
/*
 * �� �� ��  : oal_bus_init_chip_num
 * ��������  : ��ʼ��chip num
 */
void oal_bus_init_chip_num(void)
{
    g_bus_chip_num = 0;

    return;
}

#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
/*
 * �� �� ��  : oal_irq_affinity_init
 * ��������  : �жϺͺ˰󶨳�ʼ��
 */
void oal_bus_irq_affinity_init(uint8_t uc_chip_id, uint8_t uc_device_id, uint32_t ul_core_id)
{
    oal_bus_dev_stru *pst_bus_dev;

    pst_bus_dev = oal_bus_get_dev_instance(uc_chip_id, uc_device_id);
    if (pst_bus_dev == NULL) {
        return;
    }

    oal_irq_set_affinity(pst_bus_dev->st_irq_info.irq, ul_core_id);
}
#endif

/*lint -e19*/
oal_module_symbol(oal_bus_get_chip_num);

#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
oal_module_symbol(oal_bus_irq_affinity_init);
#endif
/*lint +e19*/
