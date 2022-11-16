

#ifndef __OAL_GPIO_H__
#define __OAL_GPIO_H__

#ifdef _PRE_BOARD_SD5115
#include "hi_drv_common.h"
#include "hi_drv_gpio.h"

#endif
#include "arch/oal_gpio.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_OAL_GPIO_H

/* �궨�� */
#ifdef _PRE_BOARD_SD5115
#define OAL_GPIO_PORT_E      HI_GPIO_PORT_E
#define OAL_GPIO_BIT_E       HI_GPIO_BIT_E
#define OAL_GPIO_BIT_ATTR_S  hi_gpio_bit_attr_s
#define OAL_GPIO_PORT_ATTR_S hi_gpio_port_attr_s
#define OAL_LEVEL_E          HI_LEVEL_E
#else

#endif

/* 10 �������� */
#ifdef _PRE_BOARD_SD5115
/* GPIOλ�������Բ��� */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_bit_attr_get(OAL_GPIO_PORT_E em_port, OAL_GPIO_BIT_E em_bit,
                                                     OAL_GPIO_BIT_ATTR_S *pst_attr)
{
    return hi_kernel_gpio_bit_attr_get(em_port, em_bit, pst_attr);
}

OAL_STATIC OAL_INLINE oal_uint oal_gpio_bit_attr_set(OAL_GPIO_PORT_E em_port, OAL_GPIO_BIT_E em_bit,
                                                     OAL_GPIO_BIT_ATTR_S *pst_attr)
{
    return hi_kernel_gpio_bit_attr_set(em_port, em_bit, pst_attr);
}

/* GPIO�˿���������,��������Ҫ��Ҫ�ر�GPIO�˿�ʱ�ӣ��ᵼ�¸���GPIO��ص�λ���ܹ��� */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_port_attr_get(OAL_GPIO_PORT_E em_port, OAL_GPIO_PORT_ATTR_S *pst_attr)
{
    return hi_kernel_gpio_port_attr_get(em_port, pst_attr);
}

OAL_STATIC OAL_INLINE oal_uint oal_gpio_port_attr_set(OAL_GPIO_PORT_E em_port, OAL_GPIO_PORT_ATTR_S *pst_attr)
{
    return hi_kernel_gpio_port_attr_set(em_port, pst_attr);
}

/* GPIO�˿���� ����GPIO�˿�(0~7bit)ȫ������Ϊ���ģʽʱ, ͨ���˽ӿ������������ */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_port_gpio_write(OAL_GPIO_PORT_E em_port, uint uc_data)
{
    return hi_kernel_gpio_write(em_port, uc_data);
}

/* GPIO�˿����� ����GPIO�˿�(0~7bit)ȫ������Ϊ����ģʽʱ, ͨ���˽ӿڵõ��������� */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_port_gpio_read(OAL_GPIO_PORT_E em_port, uint *puc_data)
{
    return hi_kernel_gpio_read(em_port, puc_data);
}

/* GPIO��ƽ����, ����GPIO����Ϊ���ģʽʱ,ͨ���˽ӿ����������ƽ */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_bit_write(OAL_GPIO_PORT_E em_port, OAL_GPIO_BIT_E em_bit, OAL_LEVEL_E em_level)
{
    return hi_kernel_gpio_bit_write(em_port, em_bit, em_level);
}

/* GPIO�����ƽ��ȡ������GPIO����Ϊ����ʱ,��ͨ���˽ӿڵõ������ƽ */
OAL_STATIC OAL_INLINE oal_uint oal_gpio_bit_read(OAL_GPIO_PORT_E em_port, OAL_GPIO_BIT_E em_bit, OAL_LEVEL_E *pem_level)
{
    return hi_kernel_gpio_bit_read(em_port, em_bit, pem_level);
}
#endif /* _PRE_BOARD_SD5115 */

#endif /* end of oal_ext_if.h */
