

/* ͷ�ļ����� */
#include "plat_firmware.h"

#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/rtc.h>

#include "plat_debug.h"
#include "plat_uart.h"
#include "plat_cali.h"
#include "platform_common_clk.h"
#include "plat_pm.h"
#include "oal_sdio_host_if.h"
#include "oal_hcc_host_if.h"
#include "oam_ext_if.h"

#include "hw_bfg_ps.h"
#include "plat_efuse.h"
#include "bfgx_exception_rst.h"
#include "board.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_PLAT_FIRMWARE_C

#include "oal_util.h"
#include "securec.h"

/* ȫ�ֱ������� */
/* hi1102 cfg�ļ�·�� */
OAL_STATIC uint8 *g_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_PATH,
    WIFI_CFG_PATH,
    BFGX_CFG_PATH,
    RAM_CHECK_CFG_PATH,
};

/* hi1103 mpw2 cfg�ļ�·�� */
OAL_STATIC uint8 *g_mpw2_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1103_MPW2_PATH,
    WIFI_CFG_HI1103_MPW2_PATH,
    BFGX_CFG_HI1103_MPW2_PATH,
    RAM_CHECK_CFG_HI1103_MPW2_PATH,
};

/* hi1103 pilot cfg�ļ�·�� */
uint8 *g_pilot_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1103_PILOT_PATH,
    WIFI_CFG_HI1103_PILOT_PATH,
    BFGX_CFG_HI1103_PILOT_PATH,
    RAM_CHECK_CFG_HI1103_PILOT_PATH,
};

/* hi1102a cfg�ļ� */
OAL_STATIC uint8 *g_auc_1102a_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1102A_PILOT_PATH,
    WIFI_CFG_HI1102A_PILOT_PATH,
    BFGX_CFG_HI1102A_PILOT_PATH,
    RAM_CHECK_CFG_HI1102A_PILOT_PATH,
    RAM_BCPU_CHECK_CFG_HI1102A_PILOT_PATH,
    BFGX_AND_HITALK_CFG_HI1102A_HITALK_PATH,
    HITALK_CFG_HI1102A_HITALK_PATH,
};

OAL_STATIC uint32 g_asic_type = HI1103_ASIC_MPW2;

uint8 **g_cfg_path = g_cfg_patch_in_vendor;

/* �洢cfg�ļ���Ϣ������cfg�ļ�ʱ��ֵ�����ص�ʱ��ʹ�øñ��� */
firmware_globals_struct g_cfg_info;

/* ����firmware file���ݵ�buffer���Ƚ��ļ��������buffer�У�Ȼ��������device buffer���� */
OAL_STATIC uint8 *g_firmware_down_buf = NULL;

/* g_pucDataBuf�ĳ��� */
OAL_STATIC uint32 g_firmware_down_buf_len = 0;

struct st_wifi_dump_mem_info g_nfc_buffer_data = { 0x30000000 + 0x000f9d00, OMLNFCDATABUFFLEN, "nfc_buffer_data" };

/*
 * �� �� ��  : set_hi1103_asic_type
 * ��������  : ����hi1103 asic����(MPW2/PILOT)
 * �������  : ul_asic_type: 0-MPW2��1-PILOT
 */
void set_hi1103_asic_type(uint32 ul_asic_type)
{
    g_asic_type = ul_asic_type;
}

/*
 * �� �� ��  : get_hi1103_asic_type
 * ��������  : ��ȡhi1103 asic����(MPW2/PILOT)
 * �� �� ֵ  : 0-MPW2��1-PILOT
 */
uint32 get_hi1103_asic_type(void)
{
    return g_asic_type;
}

/*
 * �� �� ��  : read_msg
 * ��������  : host����device��������Ϣ
 * �������  : data: ������Ϣ��buffer
 *             len : ����buffer�ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʽ��յĳ���
 */
int32 read_msg(uint8 *data, int32 len)
{
    int32 length;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((data == NULL))) {
        ps_print_err("data is NULL\n ");
        return -EFAIL;
    }

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    length  = hcc_bus_patch_read(pst_bus, data, len, READ_MEG_TIMEOUT);
    ps_print_dbg("Receive length =[%d]\n", length);

    return length;
}

int32 read_msg_timeout(uint8 *data, int32 len, uint32 timeout)
{
    int32 length;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((data == NULL))) {
        ps_print_err("data is NULL\n ");
        return -EFAIL;
    }

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    length = hcc_bus_patch_read(pst_bus, data, len, timeout);
    ps_print_dbg("Receive length =[%d], data = [%s]\n", length, data);

    return length;
}

/*
 * �� �� ��  : send_msg
 * ��������  : host��device������Ϣ
 * �������  : data: ����buffer
 *             len : �������ݵĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʷ��͵ĳ���
 */
int32 send_msg(uint8 *data, int32 len)
{
    int32 ret;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    ps_print_dbg("len = %d\n", len);
#ifdef HW_DEBUG
    const uint32 ul_max_print_len = 128;
    print_hex_dump_bytes("send_msg :", DUMP_PREFIX_ADDRESS, data,
                         (len < ul_max_print_len ? len : ul_max_print_len));
#endif
    ret = hcc_bus_patch_write(pst_bus, data, len);

    return ret;
}

/*
 * �� �� ��  : recv_expect_result
 * ��������  : ����host����device��ȷ���ص�����
 * �������  : expect: ����device��ȷ���ص�����
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 recv_expect_result(const uint8 *expect)
{
    uint8 auc_buf[RECV_BUF_LEN];
    int32 length;
    int32 i;

    if (!os_str_len(expect)) {
        ps_print_dbg("not wait device to respond!\n");
        return SUCC;
    }

    memset_s(auc_buf, RECV_BUF_LEN, 0, RECV_BUF_LEN);
    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        length = read_msg(auc_buf, RECV_BUF_LEN);
        if (length < 0) {
            ps_print_err("recv result fail\n");
            continue;
        }

        if (!os_mem_cmp(auc_buf, expect, os_str_len(expect))) {
            ps_print_dbg(" send SUCC, expect [%s] ok\n", expect);
            return SUCC;
        } else {
            ps_print_warning(" error result[%s], expect [%s], read result again\n", auc_buf, expect);
        }
    }

    return -EFAIL;
}

int32 recv_expect_result_timeout(const uint8 *expect, uint32 timeout)
{
    uint8 auc_buf[RECV_BUF_LEN];
    int32 length;

    if (!os_str_len(expect)) {
        ps_print_dbg("not wait device to respond!\n");
        return SUCC;
    }

    memset_s(auc_buf, RECV_BUF_LEN, 0, RECV_BUF_LEN);
    length = read_msg_timeout(auc_buf, RECV_BUF_LEN, timeout);
    if (length < 0) {
        ps_print_err("recv result fail\n");
        return -EFAIL;
    }

    if (!os_mem_cmp(auc_buf, expect, os_str_len(expect))) {
        ps_print_dbg(" send SUCC, expect [%s] ok\n", expect);
        return SUCC;
    } else {
        ps_print_warning(" error result[%s], expect [%s], read result again\n", auc_buf, expect);
    }

    return -EFAIL;
}

/*
 * �� �� ��  : msg_send_and_recv_except
 * ��������  : host��device������Ϣ���ȴ�device������Ϣ
 * �������  : data  : ����buffer
 *             len   : �������ݵĳ���
 *             expect: ����device�ظ�������
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 msg_send_and_recv_except(uint8 *data, int32 len, const uint8 *expect)
{
    int32 i;
    int32 ret;

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        ret = send_msg(data, len);
        if (ret < 0) {
            continue;
        }

        ret = recv_expect_result(expect);
        if (ret == 0) {
            return SUCC;
        }
    }

    return -EFAIL;
}

/*
 * �� �� ��  : malloc_cmd_buf
 * ��������  : ����cfg�ļ����������Ľ��������g_st_cfg_infoȫ�ֱ�����
 * �������  : puc_cfg_info_buf: ������cfg�ļ����ݵ�buffer
 *             ul_index        : ��������������������ֵ
 * �� �� ֵ  : NULL��ʾ�����ڴ�ʧ�ܣ����򷵻�ָ�򱣴����cfg�ļ�����������׵�ַ
 */
void *malloc_cmd_buf(uint8 *puc_cfg_info_buf, uint32 ul_index)
{
    int32 length;
    uint8 *flag = NULL;
    uint8 *p_buf = NULL;

    if (puc_cfg_info_buf == NULL) {
        ps_print_err("malloc_cmd_buf: buf is NULL!\n");
        return NULL;
    }

    /* ͳ��������� */
    flag = puc_cfg_info_buf;
    g_cfg_info.count[ul_index] = 0;
    while (flag != NULL) {
        /* һ����ȷ�������н�����Ϊ ; */
        flag = os_str_chr(flag, CMD_LINE_SIGN);
        if (flag == NULL) {
            break;
        }
        g_cfg_info.count[ul_index]++;
        flag++;
    }
    ps_print_dbg("cfg file cmd count: count[%d] = %d\n", ul_index, g_cfg_info.count[ul_index]);

    /* ����洢����ռ� */
    length = ((g_cfg_info.count[ul_index]) + CFG_INFO_RESERVE_LEN) * sizeof(struct cmd_type_st);
    p_buf = os_kmalloc_gfp(length);
    if (p_buf == NULL) {
        ps_print_err("kmalloc cmd_type_st fail\n");
        return NULL;
    }
    memset_s((void *)p_buf, length, 0, length);

    return p_buf;
}

/*
 * �� �� ��  : delete_space
 * ��������  : ɾ���ַ������߶���Ŀո�
 * �������  : string: ԭʼ�ַ���
 *             len   : �ַ����ĳ���
 * �� �� ֵ  : ���󷵻�NULL�����򷵻�ɾ�����߿ո��Ժ��ַ������׵�ַ
 */
uint8 *delete_space(uint8 *string, int32 *len)
{
    int i;

    if ((string == NULL) || (len == NULL)) {
        return NULL;
    }

    /* ɾ��β���Ŀո� */
    for (i = *len - 1; i >= 0; i--) {
        if (string[i] != COMPART_KEYWORD) {
            break;
        }
        string[i] = '\0';
    }
    /* ���� */
    if (i < 0) {
        ps_print_err(" string is Space bar\n");
        return NULL;
    }
    /* ��for����м�ȥ1���������1 */
    *len = i + 1;

    /* ɾ��ͷ���Ŀո� */
    for (i = 0; i < *len; i++) {
        if (string[i] != COMPART_KEYWORD) {
            /* ��ȥ�ո�ĸ��� */
            *len = *len - i;
            return &string[i];
        }
    }

    return NULL;
}

/*
 * �� �� ��  : string_to_num
 * ��������  : ���ַ���ת����������
 * �������  : string:������ַ���
 * �������  : number:�ַ���ת���Ժ��������
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 string_to_num(uint8 *string, int32 *number)
{
    int32 i;
    int32 l_num;

    if (string == NULL) {
        ps_print_err("string is NULL!\n");
        return -EFAIL;
    }

    l_num = 0;
    for (i = 0; (string[i] >= '0') && (string[i] <= '9'); i++) {
        l_num = (l_num * 10) + (string[i] - '0');   /* �ַ���ת���ֵ��߼���Ҫ */
    }

    *number = l_num;

    return SUCC;
}

/*
 * �� �� ��  : num_to_string
 * ��������  : ��������ת�����ַ���
 * �������  : string:�����������
 * �������  : number:������ת���Ժ���ַ���
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 num_to_string(uint8 *string, uint32 number)
{
    int32 i = 0;
    int32 j = 0;
    int32 tmp[INT32_STR_LEN];
    uint32 num = number;

    if (string == NULL) {
        ps_print_err("string is NULL!\n");
        return -EFAIL;
    }

    do {
        tmp[i] = num % 10;
        num = num / 10;   /* ��������ת�ַ����߼���Ҫ */
        i++;
    } while (num != 0);

    do {
        string[j] = tmp[i - 1 - j] + '0';
        j++;
    } while (j != i);

    string[j] = '\0';

    return SUCC;
}

/*
 * �� �� ��  : open_file_to_readm
 * ��������  : ���ļ�������read mem������������
 * �� �� ֵ  : ���ش��ļ���������
 */
os_kernel_file_stru *open_file_to_readm(uint8 *name)
{
    mm_segment_t fs;
    os_kernel_file_stru *fp = NULL;
    uint8 *file_name = NULL;

    if (name == NULL) {
        file_name = WIFI_DUMP_PATH "/readm_wifi";
    } else {
        file_name = name;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(file_name, O_RDWR | O_CREAT, 0664);
    set_fs(fs);

    return fp;
}

/*
 * �� �� ��  : recv_device_mem
 * ��������  : ����device�����������ڴ棬���浽ָ�����ļ���
 * �������  : fp : �����ڴ���ļ�ָ��
 *             len: ��Ҫ������ڴ�ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʱ�����ڴ�ĳ���
 */
int32 recv_device_mem(os_kernel_file_stru *fp, uint8 *tmp_data_buf, int32 len)
{
    int32 ret = -EFAIL;
    mm_segment_t fs;
    uint8 retry = 3;
    int32 lenbuf = 0;

    if (OAL_IS_ERR_OR_NULL(fp)) {
        ps_print_err("fp is error,fp = 0x%p\n", fp);
        return -EFAIL;
    }

    if (tmp_data_buf == NULL) {
        ps_print_err("tmp_data_buf is NULL\n");
        return -EFAIL;
    }

    ps_print_dbg("expect recv len is [%d]\n", len);

    fs = get_fs();
    set_fs(KERNEL_DS);
    ps_print_dbg("pos = %d\n", (int)fp->f_pos);
    while (len > lenbuf) {
        ret = read_msg(tmp_data_buf + lenbuf, len - lenbuf);
        if (ret > 0) {
            lenbuf += ret;
        } else {
            retry--;
            lenbuf = 0;
            if (retry == 0) {
                ret = -EFAIL;
                ps_print_err("time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        vfs_write(fp, tmp_data_buf, len, &fp->f_pos);
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
    vfs_fsync(fp, 0);
#else
    vfs_fsync(fp, fp->f_path.dentry, 0);
#endif
    set_fs(fs);

    return ret;
}

/*
 * �� �� ��  : check_version
 * ��������  : ���������device�汾�ţ������device�ϱ��İ汾�ź�host�İ汾���Ƿ�ƥ��
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 check_version(void)
{
    int32 ret;
    int32 length;
    int32 i;
    uint8 rec_buf[VERSION_LEN];

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        memset_s(rec_buf, VERSION_LEN, 0, VERSION_LEN);

        ret = memcpy_s(rec_buf, sizeof(rec_buf), (uint8 *)VER_CMD_KEYWORD, os_str_len(VER_CMD_KEYWORD));
        if (ret != EOK) {
            ps_print_err("rec_buf not enough\n");
            return -EFAIL;
        }
        length = os_str_len(VER_CMD_KEYWORD);

        rec_buf[length] = COMPART_KEYWORD;
        length++;

        ret = send_msg(rec_buf, length);
        if (ret < 0) {
            ps_print_err("send version fail![%d]\n", i);
            continue;
        }

        memset_s(g_cfg_info.dev_version, VERSION_LEN, 0, VERSION_LEN);
        memset_s(rec_buf, VERSION_LEN, 0, VERSION_LEN);
        msleep(1);

        ret = read_msg(rec_buf, VERSION_LEN);
        if (ret < 0) {
            ps_print_err("read version fail![%d]\n", i);
            continue;
        }

        memcpy_s(g_cfg_info.dev_version, VERSION_LEN, rec_buf, VERSION_LEN);

        if (!os_mem_cmp((int8 *)g_cfg_info.dev_version,
                        (int8 *)g_cfg_info.cfg_version,
                        os_str_len(g_cfg_info.cfg_version))) {
            ps_print_info("Device Version = [%s], CfgVersion = [%s].\n",
                          g_cfg_info.dev_version, g_cfg_info.cfg_version);
            return SUCC;
        } else {
            ps_print_err("ERROR version,Device Version = [%s], CfgVersion = [%s].\n",
                         g_cfg_info.dev_version, g_cfg_info.cfg_version);
        }
    }

    return -EFAIL;
}

/*
 * �� �� ��  : number_type_cmd_send
 * ��������  : ����number���͵���������͵�device
 * �������  : Key  : ����Ĺؼ���
 *             value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ������ʾ�ɹ�
 */
int32 number_type_cmd_send(uint8 *key, const char *value)
{
    int32 ret;
    int32 data_len;
    int32 value_len;
    int32 i;
    int32 n;
    uint8 auc_num[INT32_STR_LEN];
    uint8 buff_tx[SEND_BUF_LEN];

    value_len = os_str_len((int8 *)value);

    memset_s(auc_num, INT32_STR_LEN, 0, INT32_STR_LEN);
    memset_s(buff_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

    data_len = 0;
    data_len = os_str_len(key);
    ret = memcpy_s(buff_tx, sizeof(buff_tx), key, data_len);
    if (ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }

    buff_tx[data_len] = COMPART_KEYWORD;
    data_len = data_len + 1;

    for (i = 0, n = 0; (i <= value_len) && (n < INT32_STR_LEN); i++) {
        if ((value[i] == ',') || (value_len == i)) {
            ps_print_dbg("auc_num = %s, i = %d, n = %d\n", auc_num, i, n);
            if (n == 0) {
                continue;
            }
            ret = memcpy_s((uint8 *)&buff_tx[data_len], sizeof(buff_tx) - data_len, auc_num, n);
            if (ret != EOK) {
                ps_print_err("buff_tx not enough\n");
                return -EFAIL;
            }
            data_len = data_len + n;

            buff_tx[data_len] = COMPART_KEYWORD;
            data_len = data_len + 1;

            memset_s(auc_num, INT32_STR_LEN, 0, INT32_STR_LEN);
            n = 0;
        } else if (value[i] == COMPART_KEYWORD) {
            continue;
        } else {
            auc_num[n] = value[i];
            n++;
        }
    }

    ret = send_msg(buff_tx, data_len);

    return ret;
}

/*
 * �� �� ��  : update_device_cali_count
 * ��������  : ʹ��WRITEM�������device��У׼�������״��ϵ�ʱΪȫ0
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 update_device_cali_count(uint8 *key, uint8 *value)
{
    int32 ret;
    uint32 len, value_len;
    uint32 number = 0;
    uint8 *addr = NULL;
    uint8 buff_tx[SEND_BUF_LEN];

    /* �������value�ַ��������valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
    /* ����Ժ����ʽΪ"���ݿ��,Ҫд�ĵ�ַ,Ҫд��ֵ"---"4,0xXXXX,value" */
    len = 0;
    memset_s(buff_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

    /* buff_tx="" */
    buff_tx[len] = '4';
    len++;
    buff_tx[len] = ',';
    len++;

    /* buff_tx="4," */
    value_len = os_str_len(value);
    addr = delete_space(value, &value_len);
    if (addr == NULL) {
        ps_print_err("addr is NULL, value[%s] value_len[%d]", value, value_len);
        return -EFAIL;
    }
    ret = memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len);
    if (ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len] = ',';
    len++;

    /* buff_tx="4,0xXXX," */
    ret = get_cali_count(&number);
    ret += num_to_string(&buff_tx[len], number);

    /* ��ʱbuff_tx="4,0xXXX,value" */
    /* ʹ��WMEM_CMD_KEYWORD������device����У׼���� */
    ret += number_type_cmd_send(WMEM_CMD_KEYWORD, buff_tx);
    if (ret < 0) {
        ps_print_err("send key=[%s],value=[%s] fail\n", key, buff_tx);
        return ret;
    }

    ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
    if (ret < 0) {
        ps_print_err("recv expect result fail!\n");
        return ret;
    }

    return SUCC;
}

/*
 * �� �� ��  : download_bfgx_cali_data
 * ��������  : ʹ��files�������bfgx��У׼����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 download_bfgx_cali_data(uint8 *key, uint8 *value)
{
    int32  ret;
    uint32 len;
    uint32 value_len;
    uint8 *addr = NULL;
    uint8 buff_tx[SEND_BUF_LEN];

    /* �������value�ַ��������valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
    /* ����Ժ����ʽΪ"FILES �ļ����� Ҫд�ĵ�ַ"---"FILES 1 0xXXXX " */
    memset_s(buff_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

    /* buff_tx="" */
    len = os_str_len(key);
    ret = memcpy_s(buff_tx, sizeof(buff_tx), key, len);
    if (ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES " */
    buff_tx[len] = '1';
    len++;
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES 1 " */
    value_len = os_str_len(value);
    addr = delete_space(value, &value_len);
    if (addr == NULL) {
        ps_print_err("addr is NULL, value[%s] value_len[%d]", value, value_len);
        return -EFAIL;
    }
    ret = memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len);
    if (ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES 1 0xXXXX " */
    /* ���͵�ַ */
    ret = msg_send_and_recv_except(buff_tx, len, MSG_FROM_DEV_READY_OK);
    if (ret < 0) {
        ps_print_err("SEND [%s] addr error\n", key);
        return -EFAIL;
    }

    /* ��ȡbfgxУ׼���� */
    ret = get_bfgx_cali_data(g_firmware_down_buf, &len, g_firmware_down_buf_len);
    if (ret < 0) {
        ps_print_err("get bfgx cali data failed, len=%d\n", len);
        return -EFAIL;
    }

    /* Wait at least 5 ms */
    oal_usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);

    /* ����bfgxУ׼���� */
    ret = msg_send_and_recv_except(g_firmware_down_buf, len, MSG_FROM_DEV_FILES_OK);
    if (ret < 0) {
        ps_print_err("send bfgx cali data fail\n");
        return -EFAIL;
    }

    return SUCC;
}

/*
 * �� �� ��  : download_dcxo_cali_data
 * ��������  : ʹ��files�������dcxo��У׼����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 download_dcxo_cali_data(uint8 *key, uint8 *value)
{
    int32 ret;
    uint32 len;
    uint32 value_len;
    uint8 *addr = NULL;
    uint8 buff_tx[SEND_BUF_LEN];

    /* �������value�ַ��������valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
    /* ����Ժ����ʽΪ"FILES �ļ����� Ҫд�ĵ�ַ"---"FILES 1 0xXXXX " */
    memset_s(buff_tx, sizeof(buff_tx), 0, SEND_BUF_LEN);

    /* buff_tx="" */
    len = os_str_len(key);
    ret = memcpy_s(buff_tx, sizeof(buff_tx), key, len);
    if (ret != EOK) {
        ps_print_err("memcpy_s faild, ret = %d", ret);
        return -EFAIL;
    }
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES " */
    buff_tx[len] = '1';
    len++;
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES 1 " */
    value_len = os_str_len(value);
    addr = delete_space(value, &value_len);
    if (addr == NULL) {
        ps_print_err("addr is NULL, value[%s] value_len[%d]", value, value_len);
        return -EFAIL;
    }
    ret = memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len);
    if (ret != EOK) {
        ps_print_err("memcpy_s faild, ret = %d", ret);
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len] = COMPART_KEYWORD;
    len++;

    /* buff_tx="FILES 1 0xXXXX " */
    /* ���͵�ַ */
    ret = msg_send_and_recv_except(buff_tx, len, MSG_FROM_DEV_READY_OK);
    if (ret < 0) {
        ps_print_err("SEND [%s] addr error\n", key);
        return -EFAIL;
    }

    oal_usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);

    /* ����dcxoУ׼���� */
    ret = msg_send_and_recv_except(g_dcxo_data_buf, DCXO_CALI_DATA_BUF_LEN, MSG_FROM_DEV_FILES_OK);
    if (ret < 0) {
        ps_print_err("send bfgx cali data fail\n");
        return -EFAIL;
    }

    return SUCC;
}

/*
 * �� �� ��  : parse_file_cmd
 * ��������  : ����file�������
 * �������  : string   : file����Ĳ���
 *             addr     : ���͵����ݵ�ַ
 *             file_path: �����ļ���·��
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 parse_file_cmd(uint8 *string, unsigned long *addr, int8 **file_path)
{
    uint8 *tmp = NULL;
    int32 count = 0;
    int8 *after = NULL;

    if (string == NULL || addr == NULL || file_path == NULL) {
        ps_print_err("param is error!\n");
        return -EFAIL;
    }

    /* ��÷��͵��ļ��ĸ������˴�����Ϊ1��string�ַ����ĸ�ʽ������"1,0xXXXXX,file_path" */
    tmp = string;
    while (*tmp == COMPART_KEYWORD) {
        tmp++;
    }
    string_to_num(tmp, &count);
    if (count != FILE_COUNT_PER_SEND) {
        ps_print_err("the count of send file must be 1, count = [%d]\n", count);
        return -EFAIL;
    }

    /* ��tmpָ���ַ������ĸ */
    tmp = os_str_chr(string, ',');
    if (tmp == NULL) {
        ps_print_err("param string is err!\n");
        return -EFAIL;
    } else {
        tmp++;
        while (*tmp == COMPART_KEYWORD) {
            tmp++;
        }
    }

    *addr = simple_strtoul(tmp, &after, 16); /* ���ַ���ת����16������ */

    ps_print_dbg("file to send addr:[0x%lx]\n", *addr);

    /* "1,0xXXXX,file_path" */
    /*         ^          */
    /*       after        */
    while (*after == COMPART_KEYWORD) {
        after++;
    }
    /* ����','�ַ� */
    after++;
    while (*after == COMPART_KEYWORD) {
        after++;
    }

    ps_print_dbg("after:[%s]\n", after);

    *file_path = after;

    return SUCC;
}

void oal_print_wcpu_reg(oal_uint32 *pst_buf, oal_uint32 ul_size)
{
    oal_int32 i;
    const oal_uint32 ul_print_reg_num = 4; /* ÿ�δ�ӡ4�ֽڼĴ����ĸ��� */

    ul_size = OAL_ROUND_UP(ul_size, sizeof(oal_uint32)); /* ����4�ֽڶ����ĳ��ȣ�Ĭ�Ͻ�λ */
    if (ul_size) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "print wcpu registers:");
    }

    for (i = 0; i < ul_size; i += ul_print_reg_num) {
        oam_error_log4(0, OAM_SF_ANY, "wcpu_reg: %x %x %x %x",
                       *(pst_buf + i + 0), *(pst_buf + i + 1),
                       *(pst_buf + i + 2), *(pst_buf + i + 3)); /* wcpu 4�ֽڼĴ����еĵ�2 3�ֽڵ�ֵ */
    }
}

#define READ_DEVICE_MAX_BUF_SIZE 128
/* read device reg by bootloader */
int32 read_device_reg16(uint32 address, uint16 *value)
{
    int32 ret, buf_len;
    const uint32 ul_read_msg_len = 4;
    const uint32 ul_dump_len = 8;
    uint8 buf_tx[READ_DEVICE_MAX_BUF_SIZE];
    uint8 buf_result[READ_DEVICE_MAX_BUF_SIZE];
    void *addr = (void *)buf_result;

    memset_s(buf_tx, READ_DEVICE_MAX_BUF_SIZE, 0, READ_DEVICE_MAX_BUF_SIZE);
    memset_s(buf_result, READ_DEVICE_MAX_BUF_SIZE, 0, READ_DEVICE_MAX_BUF_SIZE);

    buf_len = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c0x%x%c%d%c",
                         RMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         address,
                         COMPART_KEYWORD,
                         4, /* 4��ʾ����� */
                         COMPART_KEYWORD); /* ��� READM 0x... 4 ���������� */
    if (buf_len < 0) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "log str format err line[%d]\n", __LINE__);
        return buf_len;
    }

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "%s", buf_tx);

    ret = send_msg(buf_tx, buf_len);
    if (ret < 0) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "send msg [%s] failed, ret=%d", buf_tx, ret);
        return ret;
    }

    ret = read_msg(buf_result, ul_read_msg_len);
    if (ret > 0) {
        /* �����ض����ڴ�,����С��ֱ��ת�� */
        *value = (uint16)oal_readl(addr);
        oal_print_hex_dump(buf_result, ul_dump_len, HEX_DUMP_GROUP_SIZE, "reg16: ");
        return 0;
    }

    oal_print_hi11xx_log(HI11XX_LOG_ERR, "read_device_reg16 failed, ret=%d", ret);

    return -1;
}

/* write device regs by bootloader */
int32 write_device_reg16(uint32 address, uint16 value)
{
    int32 ret, buf_len;
    uint8 buf_tx[READ_DEVICE_MAX_BUF_SIZE];

    memset_s(buf_tx, sizeof(buf_tx), 0, sizeof(buf_tx));

    buf_len = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c2%c0x%x%c%d%c",
                         WMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         COMPART_KEYWORD,
                         address,
                         COMPART_KEYWORD,
                         value,
                         COMPART_KEYWORD);
    if (buf_len < 0) {
        oal_print_hi11xx_log(HI11XX_LOG_INFO, "log str format err line[%d]\n", __LINE__);
        return buf_len;
    }

    oal_print_hi11xx_log(HI11XX_LOG_INFO, "%s", buf_tx);

    ret = send_msg(buf_tx, buf_len);
    if (ret < 0) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "send msg [%s] failed, ret=%d", buf_tx, ret);
        return ret;
    }

    ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
    if (ret < 0) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "send msg [%s] recv failed, ret=%d", buf_tx, ret);
        return ret;
    }

    return 0;
}
#ifdef HI110X_HAL_MEMDUMP_ENABLE
int32 recv_device_memdump(uint8 *data_buf, int32 len)
{
    int32 ret = -EFAIL;
    uint8 retry = 3;
    int32 lenbuf = 0;

    if (data_buf == NULL) {
        ps_print_err("data_buf is NULL\n");
        return -EFAIL;
    }

    ps_print_dbg("expect recv len is [%d]\n", len);

    while (len > lenbuf) {
        ret = read_msg(data_buf + lenbuf, len - lenbuf);
        if (ret > 0) {
            lenbuf += ret;
        } else {
            retry--;
            lenbuf = 0;
            if (retry == 0) {
                ret = -EFAIL;
                ps_print_err("time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        wifi_memdump_enquenue(data_buf, len);
    }

    return ret;
}
int32 sdio_read_device_mem(struct st_wifi_dump_mem_info *pst_mem_dump_info,
                           uint8 *data_buf,
                           uint32 ul_data_buf_len)
{
    uint8 buf_tx[SEND_BUF_LEN];
    int32 ret = 0;
    uint32 size = 0;
    uint32 offset;
    uint32 remainder = pst_mem_dump_info->size;

    offset = 0;
    while (remainder > 0) {
        memset_s(buf_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

        size = min(remainder, ul_data_buf_len);
        ret = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c0x%lx%c%d%c",
                         RMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         pst_mem_dump_info->mem_addr + offset,
                         COMPART_KEYWORD,
                         size,
                         COMPART_KEYWORD);
        if (ret < 0) {
            oal_print_hi11xx_log(HI11XX_LOG_INFO, "log str format err line[%d]\n", __LINE__);
            break;
        }
        ps_print_dbg("read mem cmd:[%s]\n", buf_tx);
        send_msg(buf_tx, os_str_len(buf_tx));

        ret = recv_device_memdump(data_buf, size);
        if (ret < 0) {
            ps_print_err("wifi mem dump fail, filename is [%s],ret=%d\n", pst_mem_dump_info->file_name, ret);
            break;
        }

#ifdef CONFIG_PRINTK
        if (offset == 0) {
            oal_int8 *pst_file_name = (pst_mem_dump_info->file_name ?
                                       ((oal_int8 *)pst_mem_dump_info->file_name) : (oal_int8 *)"default: ");
            if (!oal_strcmp("wifi_device_panic_mem", pst_file_name)) {
                /* dump the device cpu reg mem when panic,24B mem header + 24*4 reg info */
                if (size > CPU_PANIC_MEMDUMP_SIZE) {
                    oal_print_hex_dump(data_buf + CPU_PANIC_MEMDUMP_HEAD_SIZE, CPU_PANIC_MEMDUMP_INFO_SIZE,
                                       HEX_DUMP_GROUP_SIZE, pst_file_name);
                    /* print sdt log */
#ifdef CONFIG_MMC
                    /* dump 24*4B */
                    oal_print_wcpu_reg ((oal_uint32 *)(data_buf + CPU_PANIC_MEMDUMP_HEAD_SIZE),
                                        CPU_PANIC_MEMDUMP_INFO_SIZE / 4); /* ��Ĵ��������4�ֽڣ������������Ҫ��4 */
#endif
                }
            }
        }
#endif

        offset += size;

        remainder -= size;
    }

    return ret;
}

/*
 * �� �� ��  : wifi_device_mem_dump
 * ��������  : firmware����ʱ��ȡwifi���ڴ�
 * �������  : pst_mem_dump_info  : ��Ҫ��ȡ���ڴ���Ϣ
 *             count              : ��Ҫ��ȡ���ڴ�����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 wifi_device_mem_dump(struct st_wifi_dump_mem_info *pst_mem_dump_info, uint32 count)
{
    int32 ret = -EFAIL;
    uint32 i;
    uint8 *data_buf = NULL;
    const uint32 ul_buff_size = 100;
    uint8 buff[ul_buff_size];
    uint32 *pcount = (uint32 *)&buff[0];
    uint32 sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());

    if (!ps_is_device_log_enable()) {
        return 0;
    }

    /* ��??����??��????3��1|?��,��3�䨮D?????��??����?��Y��������??3��1|?�� */
    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);

    if (pst_mem_dump_info == NULL) {
        ps_print_err("pst_wifi_dump_info is NULL\n");
        return -EFAIL;
    }

    do {
        ps_print_info("try to malloc mem dump buf len is [%d]\n", sdio_transfer_limit);
        data_buf = (uint8 *)os_kmalloc_gfp(sdio_transfer_limit);
        if (data_buf == NULL) {
            ps_print_warning("malloc mem  len [%d] fail, continue to try in a smaller size\n", sdio_transfer_limit);
            sdio_transfer_limit = sdio_transfer_limit >> 1;
        }
    } while ((data_buf == NULL) && (sdio_transfer_limit >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (data_buf == NULL) {
        ps_print_err("data_buf KMALLOC failed\n");
        return -EFAIL;
    }

    ps_print_info("mem dump data buf len is [%d]\n", sdio_transfer_limit);

    wifi_notice_hal_memdump();

    for (i = 0; i < count; i++) {
        *pcount = pst_mem_dump_info[i].size;
        ps_print_info("mem dump data size [%d]==> [%d]\n", *pcount, pst_mem_dump_info[i].size);
        wifi_memdump_enquenue(buff, 4);  /* ��������sk_buff�Ĵ�С */
        ret = sdio_read_device_mem(&pst_mem_dump_info[i], data_buf, sdio_transfer_limit);
        if (ret < 0) {
            break;
        }
    }
    wifi_memdump_finish();

    os_mem_kfree(data_buf);

    return ret;
}

#else

/*
 * �� �� ��  : sdio_read_device_mem
 * ��������  : ��device����bootloaderʱ��DEVICE��ȡ�ڴ�
 * �� �� ֵ  : С��0��ʾʧ��
 */
int32 sdio_read_device_mem(struct st_wifi_dump_mem_info *pst_mem_dump_info,
                           os_kernel_file_stru *fp,
                           uint8 *tmp_data_buf,
                           uint32 tmp_ul_data_buf_len)
{
    uint8 buf_tx[SEND_BUF_LEN];
    int32 ret = 0;
    uint32 size = 0;
    uint32 offset;
    uint32 remainder = pst_mem_dump_info->size;

    offset = 0;
    while (remainder > 0) {
        memset_s(buf_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

        size = min(remainder, tmp_ul_data_buf_len);
        ret = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c0x%lx%c%d%c",
                         RMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         pst_mem_dump_info->mem_addr + offset,
                         COMPART_KEYWORD,
                         size,
                         COMPART_KEYWORD);
        if (ret < 0) {
            oal_print_hi11xx_log(HI11XX_LOG_INFO, "log str format err line[%d]\n", __LINE__);
            break;
        }
        ps_print_dbg("read mem cmd:[%s]\n", buf_tx);
        send_msg(buf_tx, os_str_len(buf_tx));

        ret = recv_device_mem(fp, tmp_data_buf, size);
        if (ret < 0) {
            ps_print_err("wifi mem dump fail, filename is [%s],ret=%d\n", pst_mem_dump_info->file_name, ret);
            break;
        }

#ifdef CONFIG_PRINTK
        if (offset == 0) {
            oal_int8 *pst_file_name = (pst_mem_dump_info->file_name ?
                                       ((oal_int8 *)pst_mem_dump_info->file_name) : (oal_int8 *)"default: ");
            if (!oal_strcmp("wifi_device_panic_mem", pst_file_name)) {
                /* dump the device cpu reg mem when panic,
                  24B mem header + 24*4 reg info */
                if (size > CPU_PANIC_MEMDUMP_SIZE) {
                    oal_print_hex_dump(tmp_data_buf + CPU_PANIC_MEMDUMP_HEAD_SIZE, CPU_PANIC_MEMDUMP_INFO_SIZE,
                                       HEX_DUMP_GROUP_SIZE, pst_file_name);
                    /* print sdt log */
#ifdef CONFIG_MMC
                    /* dump 24*4B */
                    oal_print_wcpu_reg ((oal_uint32 *)(tmp_data_buf + CPU_PANIC_MEMDUMP_HEAD_SIZE),
                                        CPU_PANIC_MEMDUMP_INFO_SIZE / 4); /* ��Ĵ��������4�ֽڣ������������Ҫ��4 */
#endif
                }
            }
        }
#endif

        offset += size;

        remainder -= size;
    }

    return ret;
}

/*
 * �� �� ��  : wifi_device_mem_dump
 * ��������  : firmware����ʱ��ȡwifi���ڴ�
 * �������  : pst_mem_dump_info  : ��Ҫ��ȡ���ڴ���Ϣ
 *             count              : ��Ҫ��ȡ���ڴ�����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 wifi_device_mem_dump(struct st_wifi_dump_mem_info *pst_mem_dump_info, uint32 count)
{
    os_kernel_file_stru *fp = NULL;
    int32 ret = -EFAIL;
    uint32 i;
    const uint32 ul_filename_len = 100;
    char filename[ul_filename_len];

    ktime_t time_start, time_stop;
    oal_uint64 trans_us;
    uint8 *tmp_data_buf = NULL;
    uint32 sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());

    if (!ps_is_device_log_enable()) {
        return 0;
    }

    /* ���ڴ��ȿ��ǳɹ���,ҳ��С������ڴ���������ɹ��� */
    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);

    if (pst_mem_dump_info == NULL) {
        ps_print_err("pst_wifi_dump_info is NULL\n");
        return -EFAIL;
    }

    do {
        ps_print_info("try to malloc mem dump buf len is [%d]\n", sdio_transfer_limit);
        tmp_data_buf = (uint8 *)os_kmalloc_gfp(sdio_transfer_limit);
        if (tmp_data_buf == NULL) {
            ps_print_warning("malloc mem  len [%d] fail, continue to try in a smaller size\n", sdio_transfer_limit);
            sdio_transfer_limit = sdio_transfer_limit >> 1;
        }
    } while ((tmp_data_buf == NULL) && (sdio_transfer_limit >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (tmp_data_buf == NULL) {
        ps_print_err("tmp_data_buf KMALLOC failed\n");
        return -EFAIL;
    }

    ps_print_info("mem dump data buf len is [%d]\n", sdio_transfer_limit);

    plat_wait_last_rotate_finish();

    for (i = 0; i < count; i++) {
        time_start = ktime_get();
        /* ���ļ���׼������wifi mem dump */
        memset_s(filename, sizeof(filename), 0, sizeof(filename));
        ret = snprintf_s(filename, sizeof(filename), sizeof(filename) - 1, WIFI_DUMP_PATH "/%s_%s.bin",
                         SDIO_STORE_WIFI_MEM, pst_mem_dump_info[i].file_name);
        if (ret < 0) {
            ps_print_err("filename format str err\n");
            break;
        }
        ps_print_info("readm %s\n", filename);

        fp = open_file_to_readm(filename);
        if (OAL_IS_ERR_OR_NULL(fp)) {
            ps_print_err("create file error,fp = 0x%p, filename is [%s]\n", fp, pst_mem_dump_info[i].file_name);
            break;
        }

        ret = sdio_read_device_mem(&pst_mem_dump_info[i], fp, tmp_data_buf, sdio_transfer_limit);
        if (ret < 0) {
            filp_close(fp, NULL);
            break;
        }
        filp_close(fp, NULL);
        time_stop = ktime_get();
        trans_us = (oal_uint64)ktime_to_us(ktime_sub(time_stop, time_start));
        OAL_IO_PRINT("device get mem %s cost %llu us\n", filename, trans_us);
    }

    /* send cmd to oam_hisi to rotate file */
    plat_send_rotate_cmd_2_app(CMD_READM_WIFI_SDIO);

    os_mem_kfree(tmp_data_buf);

    return ret;
}

#endif

/* ����ָ��readm�洢���ļ�·����file_name����ǰ���п� */
int32 sdio_read_path_mem(uint8 *key, uint8 *value, uint8 *file_name)
{
    int32 ret;
    uint32 size, readlen;
    int32 retry = 3;
    uint8 *flag;
    os_kernel_file_stru *fp = NULL;
    uint8 *tmp_data_buf = NULL;
    uint32 sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());

    /* ���ڴ��ȿ��ǳɹ���,ҳ��С������ڴ���������ɹ��� */
    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);

    flag = os_str_chr(value, ',');
    if (flag == NULL) {
        ps_print_err("RECV LEN ERROR..\n");
        return -EFAIL;
    }
    flag++;
    ps_print_dbg("recv len [%s]\n", flag);
    while (*flag == COMPART_KEYWORD) {
        flag++;
    }

    string_to_num(flag, &size);

    do {
        ps_print_info("try to malloc sdio mem read buf len is [%d]\n", sdio_transfer_limit);
        tmp_data_buf = (uint8 *)os_kmalloc_gfp(sdio_transfer_limit);
        if (tmp_data_buf == NULL) {
            ps_print_warning("malloc mem len [%d] fail, continue to try in a smaller size\n", sdio_transfer_limit);
            sdio_transfer_limit = sdio_transfer_limit >> 1;
        }
    } while ((tmp_data_buf == NULL) && (sdio_transfer_limit >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (tmp_data_buf == NULL) {
        ps_print_err("tmp_data_buf KMALLOC failed\n");
        return -EFAIL;
    }

    fp = open_file_to_readm(file_name);
    if (IS_ERR(fp)) {
        ps_print_err("create file error,fp = 0x%p\n", fp);
        os_mem_kfree(tmp_data_buf);
        return SUCC;
    }

    ret = number_type_cmd_send(key, value);
    if (ret < 0) {
        ps_print_err("send %s,%s fail \n", key, value);
        filp_close(fp, NULL);
        os_mem_kfree(tmp_data_buf);
        return ret;
    }

    ps_print_dbg("recv len [%d]\n", size);
    while (size > 0) {
        readlen = min(size, sdio_transfer_limit);
        ret = recv_device_mem(fp, tmp_data_buf, readlen);
        if (ret > 0) {
            size -= ret;
        } else {
            ps_print_err("read error retry:%d\n", retry);
            --retry;
            if (!retry) {
                ps_print_err("retry fail\n");
                break;
            }
        }
    }

    filp_close(fp, NULL);
    os_mem_kfree(tmp_data_buf);

    return ret;
}

int32 sdio_read_mem(uint8 *key, uint8 *value)
{
    int32 ret;
    uint32 size, readlen;
    int32 retry = 3;
    uint8 *flag;
    os_kernel_file_stru *fp = NULL;
    uint8 *tmp_data_buf = NULL;
    uint32 sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());

    /* ���ڴ��ȿ��ǳɹ���,ҳ��С������ڴ���������ɹ��� */
    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);

    flag = os_str_chr(value, ',');
    if (flag == NULL) {
        ps_print_err("RECV LEN ERROR..\n");
        return -EFAIL;
    }
    flag++;
    ps_print_dbg("recv len [%s]\n", flag);
    while (*flag == COMPART_KEYWORD) {
        flag++;
    }

    string_to_num(flag, &size);

    do {
        ps_print_info("try to malloc sdio mem read buf len is [%d]\n", sdio_transfer_limit);
        tmp_data_buf = (uint8 *)os_kmalloc_gfp(sdio_transfer_limit);
        if (tmp_data_buf == NULL) {
            ps_print_warning("malloc mem len [%d] fail, continue to try in a smaller size\n", sdio_transfer_limit);
            sdio_transfer_limit = sdio_transfer_limit >> 1;
        }
    } while ((tmp_data_buf == NULL) && (sdio_transfer_limit >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (tmp_data_buf == NULL) {
        ps_print_err("tmp_data_buf KMALLOC failed\n");
        return -EFAIL;
    }

    fp = open_file_to_readm(NULL);
    if (IS_ERR(fp)) {
        ps_print_err("create file error,fp = 0x%p\n", fp);
        os_mem_kfree(tmp_data_buf);
        return SUCC;
    }

    ret = number_type_cmd_send(key, value);
    if (ret < 0) {
        ps_print_err("send %s,%s fail \n", key, value);
        filp_close(fp, NULL);
        os_mem_kfree(tmp_data_buf);
        return ret;
    }

    ps_print_dbg("recv len [%d]\n", size);
    while (size > 0) {
        readlen = min(size, sdio_transfer_limit);
        ret = recv_device_mem(fp, tmp_data_buf, size);
        if (ret > 0) {
            size -= ret;
        } else {
            ps_print_err("read error retry:%d\n", retry);
            --retry;
            if (!retry) {
                ps_print_err("retry fail\n");
                break;
            }
        }
    }

    filp_close(fp, NULL);
    os_mem_kfree(tmp_data_buf);

    return ret;
}

/*
 * �� �� ��  : exec_number_type_cmd
 * ��������  : ִ��number���͵�����
 * �������  : key  : ����Ĺؼ���
 *             value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 exec_number_type_cmd(uint8 *key, uint8 *value)
{
    int32 ret = -EFAIL;
    board_info *tmp_board_info = NULL;

    tmp_board_info = get_hi110x_board_info();
    if (tmp_board_info == NULL) {
        ps_print_err("tmp_board_info is null!\n");
        return -EFAIL;
    }

    if (!os_mem_cmp(key, VER_CMD_KEYWORD, os_str_len(VER_CMD_KEYWORD))) {
        ret = check_version();
        if (ret < 0) {
            ps_print_err("check version FAIL [%d]\n", ret);
            return -EFAIL;
        }
    }

    if (!os_str_cmp((int8 *)key, WMEM_CMD_KEYWORD)) {
        if (os_str_str((int8 *)value, (int8 *)STR_REG_NFC_EN_KEEP) != NULL) {
            if (get_ec_version() == V100) {
                ps_print_info("hi110x V100\n");
            } else {
                ps_print_info("hi110x V120\n");
                return SUCC;
            }
        }

        ret = number_type_cmd_send(key, value);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", key, value);
            return ret;
        }

        ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
        if (ret < 0) {
            ps_print_err("recv expect result fail!\n");
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, CALI_COUNT_CMD_KEYWORD)) {
        /* ����У׼������device */
        ret = update_device_cali_count(key, value);
        if (ret < 0) {
            ps_print_err("update device cali count fail\n");
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, CALI_BFGX_DATA_CMD_KEYWORD)) {
        if (oal_atomic_read(&g_ir_only_mode) != 0) {
            ps_print_info("ir only pass the download cali data cmd\n");
            return SUCC;
        }

        /* ����BFGX��У׼���� */
        ret = download_bfgx_cali_data(FILES_CMD_KEYWORD, value);
        if (ret < 0) {
            ps_print_err("download bfgx cali data fail\n");
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, CALI_DCXO_DATA_CMD_KEYWORD)) {
        ps_print_dbg("download dcxo cali data begin\n");
        /* ����TCXO ��У׼������devcie */
        ret = download_dcxo_cali_data(FILES_CMD_KEYWORD, value);
        if (ret < 0) {
            ps_print_err("download dcxo cali data fail\n");
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, JUMP_CMD_KEYWORD)) {
        ret = number_type_cmd_send(key, value);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", key, value);
            return ret;
        }

        /* 100000ms timeout */
        ret = recv_expect_result_timeout(MSG_FROM_DEV_JUMP_OK, READ_MEG_JUMP_TIMEOUT);
        if (ret >= 0) {
            ps_print_info("JUMP success!\n");
            return ret;
        } else {
            ps_print_err("CMD JUMP timeout! ret=%d\n", ret);
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, SETPM_CMD_KEYWORD) || !os_str_cmp((int8 *)key, SETBUCK_CMD_KEYWORD) ||
               !os_str_cmp((int8 *)key, SETSYSLDO_CMD_KEYWORD) || !os_str_cmp((int8 *)key, SETNFCRETLDO_CMD_KEYWORD) ||
               !os_str_cmp((int8 *)key, SETPD_CMD_KEYWORD) || !os_str_cmp((int8 *)key, SETNFCCRG_CMD_KEYWORD) ||
               !os_str_cmp((int8 *)key, SETABB_CMD_KEYWORD) || !os_str_cmp((int8 *)key, SETTCXODIV_CMD_KEYWORD)) {
        ret = number_type_cmd_send(key, value);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", key, value);
            return ret;
        }

        ret = recv_expect_result(MSG_FROM_DEV_SET_OK);
        if (ret < 0) {
            ps_print_err("recv expect result fail!\n");
            return ret;
        }
    } else if (!os_str_cmp((int8 *)key, RMEM_CMD_KEYWORD)) {
        ret = sdio_read_mem(key, value);
    }

    return ret;
}

/*
 * �� �� ��  : exec_quit_type_cmd
 * ��������  : ִ��quit���͵�����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 exec_quit_type_cmd(void)
{
    int32 ret;
    int32 length;
    const uint32 ul_buf_len = 8;
    uint8 buf[ul_buf_len];
    board_info *tmp_board_info = NULL;

    tmp_board_info = get_hi110x_board_info();
    if (tmp_board_info == NULL) {
        ps_print_err("tmp_board_info is null!\n");
        return -EFAIL;
    }

    memset_s(buf, sizeof(buf), 0, sizeof(buf));

    ret = memcpy_s(buf, sizeof(buf), (uint8 *)QUIT_CMD_KEYWORD, os_str_len(QUIT_CMD_KEYWORD));
    if (ret != EOK) {
        ps_print_err("memcpy_s faild, ret = %d", ret);
        return -EFAIL;
    }
    length = os_str_len(QUIT_CMD_KEYWORD);

    buf[length] = COMPART_KEYWORD;
    length++;

    ret = msg_send_and_recv_except(buf, length, MSG_FROM_DEV_QUIT_OK);

    return ret;
}

STATIC int32_t firmware_file_send(os_kernel_file_stru *fp, uint32_t file_len, unsigned long addr)
{
    unsigned long addr_send;
    uint32_t per_send_len;
    uint32_t send_count;
    int32_t rdlen;
    int32_t ret;
    uint32_t i;
    uint32_t offset = 0;
    uint8_t buff_tx[SEND_BUF_LEN] = {0};

    per_send_len = (g_firmware_down_buf_len > file_len) ? file_len : g_firmware_down_buf_len;
    send_count = (file_len + per_send_len - 1) / per_send_len;

    for (i = 0; i < send_count; i++) {
        rdlen = oal_file_read_ext(fp, fp->f_pos, g_firmware_down_buf, per_send_len);
        if (rdlen > 0) {
            ps_print_dbg("len of kernel_read is [%d], i=%d\n", rdlen, i);
            fp->f_pos += rdlen;
        } else {
            ps_print_err("len of kernel_read is error! ret=[%d], i=%d\n", rdlen, i);
            return (rdlen < 0) ? rdlen : -EFAIL;
        }

        addr_send = addr + offset;
        ps_print_dbg("send addr is [0x%lx], i=%d\n", addr_send, i);
        ret = snprintf_s(buff_tx, sizeof(buff_tx), sizeof(buff_tx) - 1, "%s%c%d%c0x%lx%c",
                         FILES_CMD_KEYWORD, COMPART_KEYWORD, FILE_COUNT_PER_SEND,
                         COMPART_KEYWORD, addr_send, COMPART_KEYWORD);
        if (ret < 0) {
            ps_print_err("send file addr cmd failed\n");
            return -EFAIL;
        }
        /* ���͵�ַ */
        ps_print_dbg("send file addr cmd is [%s]\n", buff_tx);
        ret = msg_send_and_recv_except(buff_tx, os_str_len(buff_tx), MSG_FROM_DEV_READY_OK);
        if (ret < 0) {
            ps_print_err("SEND [%s] error\n", buff_tx);
            return -EFAIL;
        }

        /* Wait at least 5 ms */
        oal_usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);

        /* �����ļ����� */
        ret = msg_send_and_recv_except(g_firmware_down_buf, rdlen, MSG_FROM_DEV_FILES_OK);
        if (ret < 0) {
            ps_print_err(" sdio send data fail\n");
            return -EFAIL;
        }
        offset += rdlen;
    }

    /* ���͵ĳ���Ҫ���ļ��ĳ���һ�� */
    if (offset != file_len) {
        ps_print_err("file send len is err! send len is [%d], file len is [%d]\n", offset, file_len);
        return -EFAIL;
    }

    return SUCC;
}


STATIC int32 file_open_get_len(int8 *path, os_kernel_file_stru **fp, uint32 *file_len)
{
    mm_segment_t fs;

    fs = get_fs();
    set_fs(KERNEL_DS);
    *fp = filp_open(path, O_RDONLY, 0);
    if (IS_ERR_OR_NULL(*fp)) {
        ps_print_err("filp_open [%s] fail!!, fp=%pK, errno:%ld\n", path, *fp, PTR_ERR(*fp));
        set_fs(fs);
        *fp = NULL;
        return -EFAIL;
    }

    /* ��ȡfile�ļ���С */
    *file_len = vfs_llseek(*fp, 0, SEEK_END);
    if (*file_len <= 0) {
        ps_print_err("file size of %s is 0!!\n", path);
        filp_close(*fp, NULL);
        set_fs(fs);
        return -EFAIL;
    }

    /* �ָ�fp->f_pos���ļ���ͷ */
    vfs_llseek(*fp, 0, SEEK_SET);
    set_fs(fs);

    return SUCC;
}

/*
 * �� �� ��  : exec_file_type_cmd
 * ��������  : ִ��file���͵�����
 * �������  : key  : ����Ĺؼ���
 *             value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
int32 exec_file_type_cmd(uint8 *key, uint8 *value)
{
    unsigned long addr;
    int8 *path = NULL;
    int32_t ret;
    uint32_t file_len;
    os_kernel_file_stru *fp = NULL;
    board_info *tmp_board_info = NULL;

    tmp_board_info = get_hi110x_board_info();
    if (tmp_board_info == NULL) {
        ps_print_err("tmp_board_info is null!\n");
        return -EFAIL;
    }

    if (oal_atomic_read(&g_ir_only_mode) != 0) {
        ps_print_info("ir only pass the download file cmd\n");
        return SUCC;
    }

    ret = parse_file_cmd(value, &addr, &path);
    if (ret < 0) {
        ps_print_err("parse file cmd fail!\n");
        return ret;
    }

    ps_print_info("download firmware:%s addr:0x%x\n", path, (uint32)addr);

    ret = file_open_get_len(path, &fp, &file_len);
    if (ret < 0) {
        return ret;
    }

    ps_print_dbg("file len is [%d]\n", file_len);

    ret = firmware_file_send(fp, file_len, addr);
    if (ret < 0) {
        ps_print_err("firmware file send fail!\n");
    }

    oal_file_close(fp);

    return ret;
}

/*
 * �� �� ��  : exec_shutdown_type_cmd
 * ��������  : ִ��shutdown cpu type������
 * �������  : which_cpu: Ҫ�رյ�cpu
 * �� �� ֵ  : -1��ʾʧ�ܣ��Ǹ�����ʾ�ɹ�
 */
int32 exec_shutdown_type_cmd(uint32 which_cpu)
{
    int32 ret = -EFAIL;
    uint8 value_shutdown[SHUTDOWN_TX_CMD_LEN];

    if (which_cpu == DEV_WCPU) {
        ret = snprintf_s(value_shutdown, sizeof(value_shutdown), sizeof(value_shutdown) - 1, "%d,%s,%d",
                         HOST_TO_DEVICE_CMD_HEAD, SOFT_WCPU_EN_ADDR, 0);
        if (ret < 0) {
            ps_print_err("SOFT_WCPU_EN_ADDR cmd format failed\n");
            return ret;
        }

        ret = number_type_cmd_send(WMEM_CMD_KEYWORD, value_shutdown);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", SHUTDOWN_WIFI_CMD_KEYWORD, value_shutdown);
            return ret;
        }
    } else if (which_cpu == DEV_BCPU) {
        ret = snprintf_s(value_shutdown, sizeof(value_shutdown), sizeof(value_shutdown) - 1, "%d,%s,%d",
                         HOST_TO_DEVICE_CMD_HEAD, SOFT_BCPU_EN_ADDR, 0);
        if (ret < 0) {
            ps_print_err("SOFT_BCPU_EN_ADDR cmd format failed\n");
            return ret;
        }

        ret = number_type_cmd_send(WMEM_CMD_KEYWORD, value_shutdown);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", SHUTDOWN_BFGX_CMD_KEYWORD, value_shutdown);
            return ret;
        }

        ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
        if (ret < 0) {
            ps_print_err("recv expect result fail!\n");
            return ret;
        }

        ret = snprintf_s(value_shutdown, sizeof(value_shutdown), sizeof(value_shutdown) - 1, "%d,%s,%d",
                         HOST_TO_DEVICE_CMD_HEAD, BCPU_DE_RESET_ADDR, 1);
        if (ret < 0) {
            ps_print_err("BCPU_DE_RESET_ADDR cmd format failed\n");
            return ret;
        }

        ret = number_type_cmd_send(WMEM_CMD_KEYWORD, value_shutdown);
        if (ret < 0) {
            ps_print_err("send key=[%s],value=[%s] fail\n", SHUTDOWN_BFGX_CMD_KEYWORD, value_shutdown);
            return ret;
        }

        ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
        if (ret < 0) {
            ps_print_err("recv expect result fail!\n");
            return ret;
        }
    } else {
        ps_print_err("para is error, which_cpu=[%d]\n", which_cpu);
        return -EFAIL;
    }

    return SUCC;
}

/*
 * �� �� ��  : execute_download_cmd
 * ��������  : ִ��firmware download������
 * �������  : cmd_type: �������������
 *             cmd_name: ����Ĺؼ���
 *             cmd_para: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ��Ǹ�����ʾ�ɹ�
 */
int32 execute_download_cmd(int32 cmd_type, uint8 *cmd_name, uint8 *cmd_para)
{
    int32 ret;

    switch (cmd_type) {
        case FILE_TYPE_CMD:
            ps_print_dbg(" command type FILE_TYPE_CMD\n");
            ret = exec_file_type_cmd(cmd_name, cmd_para);
            break;
        case NUM_TYPE_CMD:
            ps_print_dbg(" command type NUM_TYPE_CMD\n");
            ret = exec_number_type_cmd(cmd_name, cmd_para);
            break;
        case QUIT_TYPE_CMD:
            ps_print_dbg(" command type QUIT_TYPE_CMD\n");
            ret = exec_quit_type_cmd();
            break;
        case SHUTDOWN_WIFI_TYPE_CMD:
            ps_print_dbg(" command type SHUTDOWN_WIFI_TYPE_CMD\n");
            ret = exec_shutdown_type_cmd(DEV_WCPU);
            break;
        case SHUTDOWN_BFGX_TYPE_CMD:
            ps_print_dbg(" command type SHUTDOWN_BFGX_TYPE_CMD\n");
            ret = exec_shutdown_type_cmd(DEV_BCPU);
            break;

        default:
            ps_print_err("command type error[%d]\n", cmd_type);
            ret = -EFAIL;
            break;
    }

    return ret;
}

/*
 * �� �� ��  : firmware_read_cfg
 * ��������  : ��ȡcfg�ļ������ݣ��ŵ�������̬�����buffer��
 * �������  : puc_CfgPatch    : cfg�ļ���·��
 *             puc_read_buffer : ����cfg�ļ����ݵ�buffer
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_read_cfg(uint8 *cfg_patch, uint8 *puc_read_buffer)
{
    mm_segment_t fs;
    int32 ret;
    os_kernel_file_stru *fp = NULL;

    if ((cfg_patch == NULL) || (puc_read_buffer == NULL)) {
        ps_print_err("para is NULL\n");
        return -EFAIL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(cfg_patch, O_RDONLY, 0);
    if (OAL_IS_ERR_OR_NULL(fp)) {
        set_fs(fs);
        ps_print_err("open file %s fail, fp=%p\n", cfg_patch, fp);
        fp = NULL;
        return -EFAIL;
    }

    memset_s(puc_read_buffer, READ_CFG_BUF_LEN, 0, READ_CFG_BUF_LEN);

    ret = oal_file_read_ext(fp, fp->f_pos, puc_read_buffer, READ_CFG_BUF_LEN);

    filp_close(fp, NULL);
    set_fs(fs);
    fp = NULL;

    return ret;
}

/*
 * �� �� ��  : firmware_parse_cmd
 * ��������  : ����cfg�ļ��е�����
 * �������  : puc_cfg_buffer: ����cfg�ļ����ݵ�buffer
 *             puc_cmd_name  : ��������Ժ�����ؼ��ֵ�buffer
 *             puc_cmd_para  : ��������Ժ����������buffer
 * �� �� ֵ  : �������������
 */
int32 firmware_parse_cmd(uint8 *puc_cfg_buffer, uint8 *puc_cmd_name, uint32 cmd_name_len,
                         uint8 *puc_cmd_para, uint32 cmd_para_len)
{
    int32 ret;
    int32 cmd_type;
    int32 l_cmdlen;
    int32 l_paralen;
    uint8 *begin = NULL;
    uint8 *end = NULL;
    uint8 *link = NULL;
    uint8 *handle = NULL;
    uint8 *handle_temp = NULL;

    begin = puc_cfg_buffer;
    if ((puc_cfg_buffer == NULL) || (puc_cmd_name == NULL) || (puc_cmd_para == NULL)) {
        ps_print_err("para is NULL\n");
        return ERROR_TYPE_CMD;
    }

    /* ע���� */
    if (puc_cfg_buffer[0] == '@') {
        return ERROR_TYPE_CMD;
    }

    /* �����У������˳������� */
    link = os_str_chr((int8 *)begin, '=');
    if (link == NULL) {
        /* �˳������� */
        if (os_str_str((int8 *)puc_cfg_buffer, QUIT_CMD_KEYWORD) != NULL) {
            return QUIT_TYPE_CMD;
        } else if (os_str_str((int8 *)puc_cfg_buffer, SHUTDOWN_WIFI_CMD_KEYWORD) != NULL) {
            return SHUTDOWN_WIFI_TYPE_CMD;
        } else if (os_str_str((int8 *)puc_cfg_buffer, SHUTDOWN_BFGX_CMD_KEYWORD) != NULL) {
            return SHUTDOWN_BFGX_TYPE_CMD;
        }

        return ERROR_TYPE_CMD;
    }

    /* �����У�û�н����� */
    end = os_str_chr(link, ';');
    if (end == NULL) {
        return ERROR_TYPE_CMD;
    }

    l_cmdlen = link - begin;

    /* ɾ���ؼ��ֵ����߿ո� */
    handle = delete_space((uint8 *)begin, &l_cmdlen);
    if (handle == NULL) {
        return ERROR_TYPE_CMD;
    }

    /* �ж��������� */
    if (!os_mem_cmp(handle, (uint8 *)FILE_TYPE_CMD_KEY, os_str_len((uint8 *)FILE_TYPE_CMD_KEY))) {
        handle_temp = os_str_str(handle, (uint8 *)FILE_TYPE_CMD_KEY);
        if (handle_temp == NULL) {
            ps_print_err("'ADDR_FILE_'is not handle child string, handle=%s", handle);
            return ERROR_TYPE_CMD;
        }
        handle = handle_temp + os_str_len(FILE_TYPE_CMD_KEY);
        l_cmdlen = l_cmdlen - os_str_len(FILE_TYPE_CMD_KEY);
        cmd_type = FILE_TYPE_CMD;
    } else if (!os_mem_cmp(handle, (uint8 *)NUM_TYPE_CMD_KEY, os_str_len(NUM_TYPE_CMD_KEY))) {
        handle_temp = os_str_str(handle, (uint8 *)NUM_TYPE_CMD_KEY);
        if (handle_temp == NULL) {
            ps_print_err("'PARA_' is not handle child string, handle=%s", handle);
            return ERROR_TYPE_CMD;
        }
        handle = handle_temp + os_str_len(NUM_TYPE_CMD_KEY);
        l_cmdlen = l_cmdlen - os_str_len(NUM_TYPE_CMD_KEY);
        cmd_type = NUM_TYPE_CMD;
    } else {
        return ERROR_TYPE_CMD;
    }

    ret = memcpy_s(puc_cmd_name, cmd_name_len, handle, l_cmdlen);
    if (ret != EOK) {
        ps_print_err("cmd len out of range! ret = %d\n", ret);
        return ERROR_TYPE_CMD;
    }

    /* ɾ��ֵ���߿ո� */
    begin = link + 1;
    l_paralen = end - begin;

    handle = delete_space((uint8 *)begin, &l_paralen);
    if (handle == NULL) {
        return ERROR_TYPE_CMD;
    }
    ret = memcpy_s(puc_cmd_para, cmd_para_len, handle, l_paralen);
    if (ret != EOK) {
        ps_print_err("para len out of range!ret = %d\n", ret);
        return ERROR_TYPE_CMD;
    }

    return cmd_type;
}

/*
 * �� �� ��  : firmware_parse_cfg
 * ��������  : ����cfg�ļ����������Ľ��������g_st_cfg_infoȫ�ֱ�����
 * �������  : puc_cfg_info_buf: ������cfg�ļ����ݵ�buffer
 *             l_buf_len       : puc_cfg_info_buf�ĳ���
 *             ul_index        : ��������������������ֵ
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_parse_cfg(uint8 *puc_cfg_info_buf, int32 l_buf_len, uint32 ul_index)
{
    int32 i;
    int32 length;
    int32 ret;
    uint8 *flag = NULL;
    uint8 *begin = NULL;
    uint8 *end = NULL;
    int32 cmd_type;
    uint8 cmd_name[DOWNLOAD_CMD_LEN];
    uint8 cmd_para[DOWNLOAD_CMD_PARA_LEN];
    uint32 cmd_para_len = 0;
    if (puc_cfg_info_buf == NULL) {
        ps_print_err("puc_cfg_info_buf is NULL!\n");
        return -EFAIL;
    }

    g_cfg_info.apst_cmd[ul_index] = (struct cmd_type_st *)malloc_cmd_buf(puc_cfg_info_buf, ul_index);
    if (g_cfg_info.apst_cmd[ul_index] == NULL) {
        ps_print_err(" malloc_cmd_buf fail!\n");
        return -EFAIL;
    }

    /* ����CMD BUF */
    flag = puc_cfg_info_buf;
    length = l_buf_len;
    i = 0;
    while ((i < g_cfg_info.count[ul_index]) && (flag < &puc_cfg_info_buf[length])) {
        /*
         * ��ȡ�����ļ��е�һ��,�����ļ�������unix��ʽ.
         * �����ļ��е�ĳһ�к����ַ� @ ����Ϊ����Ϊע����
         */
        begin = flag;
        end = os_str_chr(flag, '\n');
        if (end == NULL) { /* �ļ������һ�У�û�л��з� */
            ps_print_dbg("lost of new line!\n");
            end = &puc_cfg_info_buf[length];
        } else if (end == begin) { /* ����ֻ��һ�����з� */
            ps_print_dbg("blank line\n");
            flag = end + 1;
            continue;
        }
        *end = '\0';

        ps_print_dbg("operation string is [%s]\n", begin);

        memset_s(cmd_name, sizeof(cmd_name), 0, DOWNLOAD_CMD_LEN);
        memset_s(cmd_para, sizeof(cmd_para), 0, DOWNLOAD_CMD_PARA_LEN);

        cmd_type = firmware_parse_cmd(begin, cmd_name, sizeof(cmd_name), cmd_para, sizeof(cmd_para));

        ps_print_dbg("cmd type=[%d],cmd_name=[%s],cmd_para=[%s]\n", cmd_type, cmd_name, cmd_para);

        if (cmd_type != ERROR_TYPE_CMD) { /* ��ȷ���������ͣ����� */
            g_cfg_info.apst_cmd[ul_index][i].cmd_type = cmd_type;
            memcpy_s(g_cfg_info.apst_cmd[ul_index][i].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN);
            memcpy_s(g_cfg_info.apst_cmd[ul_index][i].cmd_para, DOWNLOAD_CMD_PARA_LEN,
                     cmd_para, DOWNLOAD_CMD_PARA_LEN);
            /* ��ȡ���ð汾�� */
            if (!os_mem_cmp(g_cfg_info.apst_cmd[ul_index][i].cmd_name,
                            VER_CMD_KEYWORD,
                            os_str_len(VER_CMD_KEYWORD))) {
                cmd_para_len = os_str_len(g_cfg_info.apst_cmd[ul_index][i].cmd_para);

                ret = memcpy_s(g_cfg_info.cfg_version, sizeof(g_cfg_info.cfg_version),
                               g_cfg_info.apst_cmd[ul_index][i].cmd_para, cmd_para_len);
                if (ret != EOK) {
                    ps_print_err("cmd_para_len = %d over cfg_version length", cmd_para_len);
                    return -EFAIL;
                }
                ps_print_dbg("g_CfgVersion = [%s].\n", g_cfg_info.cfg_version);
            } else if (!os_mem_cmp(g_cfg_info.apst_cmd[ul_index][i].cmd_name,
                                   CALI_DCXO_DATA_CMD_KEYWORD,
                                   os_str_len(CALI_DCXO_DATA_CMD_KEYWORD))) {
                if (!test_bit(DCXO_PARA_READ_OK, &g_dcxo_info.nv_init_flag)) {
                    ret = read_dcxo_cali_data();
                    if (ret < 0) {
                        ps_print_err("read dcxo para from nv failed !\n");
                        return -EFAIL;
                    }
                    set_bit(DCXO_PARA_READ_OK, &g_dcxo_info.nv_init_flag);
                } else {
                    ps_print_dbg("dcxo para has already initialized, skip...\n");
                }
            }
            i++;
        }
        flag = end + 1;
    }

    /* ����ʵ������������޸����յ�������� */
    g_cfg_info.count[ul_index] = i;
    ps_print_info("effective cmd count: count[%d] = %d\n", ul_index, g_cfg_info.count[ul_index]);

    return SUCC;
}

/*
 * �� �� ��  : firmware_get_cfg
 * ��������  : ��ȡcfg�ļ����������������Ľ��������g_st_cfg_infoȫ�ֱ�����
 * �������  : puc_CfgPatch: cfg�ļ���·��
 *             ul_index     : ��������������������ֵ
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_get_cfg(uint8 *cfg_patch, uint32 ul_index)
{
    uint8 *puc_read_cfg_buf = NULL;
    int32 l_readlen;
    int32 ret;

    if (cfg_patch == NULL) {
        ps_print_err("cfg file path is null!\n");
        return -EFAIL;
    }

    /* cfg�ļ��޶���С��2048,���cfg�ļ��Ĵ�Сȷʵ����2048�������޸�READ_CFG_BUF_LEN��ֵ */
    puc_read_cfg_buf = os_kmalloc_gfp(READ_CFG_BUF_LEN);
    if (puc_read_cfg_buf == NULL) {
        ps_print_err("kmalloc READ_CFG_BUF fail!\n");
        return -EFAIL;
    }

    l_readlen = firmware_read_cfg(cfg_patch, puc_read_cfg_buf);
    if (l_readlen < 0) {
        ps_print_err("read cfg error!\n");
        os_mem_kfree(puc_read_cfg_buf);
        puc_read_cfg_buf = NULL;
        return -EFAIL;
    /* ��1��Ϊ��ȷ��cfg�ļ��ĳ��Ȳ�����READ_CFG_BUF_LEN����Ϊfirmware_read_cfg���ֻ���ȡREAD_CFG_BUF_LEN���ȵ����� */
    } else if (l_readlen > READ_CFG_BUF_LEN - 1) {
        ps_print_err("cfg file [%s] larger than %d\n", cfg_patch, READ_CFG_BUF_LEN);
        os_mem_kfree(puc_read_cfg_buf);
        puc_read_cfg_buf = NULL;
        return -EFAIL;
    } else {
        ps_print_dbg("read cfg file [%s] ok, size is [%d]\n", cfg_patch, l_readlen);
    }

    ret = firmware_parse_cfg(puc_read_cfg_buf, l_readlen, ul_index);
    if (ret < 0) {
        ps_print_err("parse cfg error!\n");
    }

    os_mem_kfree(puc_read_cfg_buf);
    puc_read_cfg_buf = NULL;

    return ret;
}

/*
 * �� �� ��  : firmware_download
 * ��������  : firmware����
 * �������  : ul_index: ��Ч�����������������
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_download(uint32 ul_index)
{
    int32 ret;
    int32 i;
    int32 l_cmd_type;
    uint8 *puc_cmd_name = NULL;
    uint8 *puc_cmd_para = NULL;
    hcc_bus *pst_bus = NULL;

    if (ul_index >= CFG_FILE_TOTAL) {
        ps_print_err("ul_index [%d] is error!\n", ul_index);
        return -EFAIL;
    }

    store_efuse_info();

    ps_print_info("start download firmware, ul_index = [%d]\n", ul_index);

    if (g_cfg_info.count[ul_index] == 0) {
        ps_print_err("firmware download cmd count is 0, ul_index = [%d]\n", ul_index);
        return -EFAIL;
    }

    pst_bus = hcc_get_current_110x_bus();
    if (pst_bus == NULL) {
        ps_print_err("firmware curr bus is null, ul_index = [%d]\n", ul_index);
        return -EFAIL;
    }

    g_firmware_down_buf = (uint8 *)oal_memtry_alloc(oal_min(pst_bus->cap.max_trans_size, MAX_FIRMWARE_FILE_TX_BUF_LEN),
                                                    MIN_FIRMWARE_FILE_TX_BUF_LEN, &g_firmware_down_buf_len);
    g_firmware_down_buf_len = OAL_ROUND_DOWN(g_firmware_down_buf_len, 8);  /* �����3bit����֤8�ֽڶ��� */
    if (g_firmware_down_buf == NULL || (g_firmware_down_buf_len == 0)) {
        ps_print_err("g_firmware_down_buf KMALLOC failed, min request:%u\n", MIN_FIRMWARE_FILE_TX_BUF_LEN);
        return -EFAIL;
    }

    ps_print_info("download firmware file buf len is [%d]\n", g_firmware_down_buf_len);

    for (i = 0; i < g_cfg_info.count[ul_index]; i++) {
        l_cmd_type = g_cfg_info.apst_cmd[ul_index][i].cmd_type;
        puc_cmd_name = g_cfg_info.apst_cmd[ul_index][i].cmd_name;
        puc_cmd_para = g_cfg_info.apst_cmd[ul_index][i].cmd_para;

        ps_print_dbg("cmd[%d]:type[%d], name[%s], para[%s]\n", i, l_cmd_type, puc_cmd_name, puc_cmd_para);

        ps_print_dbg("firmware down start cmd[%d]:type[%d], name[%s]\n", i, l_cmd_type, puc_cmd_name);

        ret = execute_download_cmd(l_cmd_type, puc_cmd_name, puc_cmd_para);
        if (ret < 0) {
            os_mem_kfree(g_firmware_down_buf);
            g_firmware_down_buf = NULL;
            ps_print_err("download firmware fail\n");

            return ret;
        }

        ps_print_dbg("firmware down finish cmd[%d]:type[%d], name[%s]\n", i, l_cmd_type, puc_cmd_name);
    }

    os_mem_kfree(g_firmware_down_buf);
    g_firmware_down_buf = NULL;

    ps_print_info("finish download firmware\n");

    return SUCC;
}

int32 print_firmware_download_cmd(uint32 ul_index)
{
    int32 i;
    int32 l_cmd_type;
    uint8 *puc_cmd_name = NULL;
    uint8 *puc_cmd_para = NULL;
    uint32 count;

    count = g_cfg_info.count[ul_index];
    ps_print_info("[%s] download cmd, total count is [%d]\n", g_cfg_path[ul_index], count);

    for (i = 0; i < count; i++) {
        l_cmd_type = g_cfg_info.apst_cmd[ul_index][i].cmd_type;
        puc_cmd_name = g_cfg_info.apst_cmd[ul_index][i].cmd_name;
        puc_cmd_para = g_cfg_info.apst_cmd[ul_index][i].cmd_para;

        ps_print_info("cmd[%d]:type[%d], name[%s], para[%s]\n", i, l_cmd_type, puc_cmd_name, puc_cmd_para);
    }

    return 0;
}

int32 print_cfg_file_cmd(void)
{
    int32 i;

    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        print_firmware_download_cmd(i);
    }

    return 0;
}

/*
 * �� �� ��  : firmware_cfg_path_init
 * ��������  : ��ȡfirmware��cfg�ļ�·��
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_cfg_path_init(void)
{
    int32 ret;
    int32 length;
    uint8 rec_buf[VERSION_LEN];

    if (get_hi110x_subchip_type() == BOARD_VERSION_HI1103) {
        memset_s(rec_buf, sizeof(rec_buf), 0, VERSION_LEN);

        ret = memcpy_s(rec_buf, sizeof(rec_buf), (uint8 *)VER_CMD_KEYWORD, os_str_len(VER_CMD_KEYWORD));
        if (ret != EOK) {
            ps_print_err("memcpy_s failed,ret = %d", ret);
            return -EFAIL;
        }

        length = os_str_len(VER_CMD_KEYWORD);

        rec_buf[length] = COMPART_KEYWORD;
        length++;

        ret = send_msg(rec_buf, length);
        if (ret < 0) {
            ps_print_err("Hi1103 send version cmd fail!\n");
            return -EFAIL;
        }

        msleep(1);

        ret = read_msg(rec_buf, VERSION_LEN);
        if (ret < 0) {
            ps_print_err("Hi1103 read version fail!\n");
            return -EFAIL;
        }

        ps_print_info("Hi1103 Device Version=[%s].\n", rec_buf);

        if (!os_mem_cmp((int8 *)rec_buf, (int8 *)HI1103_MPW2_BOOTLOADER_VERSION,
                        os_str_len(HI1103_MPW2_BOOTLOADER_VERSION))) {
            g_cfg_path = g_mpw2_cfg_patch_in_vendor;
            set_hi1103_asic_type(HI1103_ASIC_MPW2);
            return SUCC;
        } else if (!os_mem_cmp((int8 *)rec_buf, (int8 *)HI1103_PILOT_BOOTLOADER_VERSION,
                               os_str_len(HI1103_PILOT_BOOTLOADER_VERSION))) {
            g_cfg_path = g_pilot_cfg_patch_in_vendor;
            set_hi1103_asic_type(HI1103_ASIC_PILOT);
            return SUCC;
        } else {
            ps_print_warning("Hi1103 Device Version Error!\n");
            g_cfg_path = g_pilot_cfg_patch_in_vendor;
            set_hi1103_asic_type(HI1103_ASIC_PILOT);
            return SUCC;
        }
    } else if (get_hi110x_subchip_type() == BOARD_VERSION_HI1102A) {
        ps_print_info("subchip type is hi1102a.\n");
        g_cfg_path = g_auc_1102a_cfg_patch_in_vendor;
        set_hi1103_asic_type(HI1103_ASIC_PILOT);
        return SUCC;
    }

    return SUCC;
}

/*
 * �� �� ��  : firmware_cfg_init
 * ��������  : firmware���ص�cfg�ļ���ʼ������ȡ������cfg�ļ����������Ľ��������
 *             g_st_cfg_infoȫ�ֱ�����
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32 firmware_cfg_init(void)
{
    int32 ret;
    uint32 i;

    ret = firmware_cfg_path_init();
    if (ret != SUCC) {
        ps_print_err("firmware cfg path init fail!");
        return -EFAIL;
    }

    /* �������ڱ��湲ʱ��У׼���ݵ�buffer */
    ret = dcxo_data_buf_malloc();
    if (ret < 0) {
        ps_print_err("alloc dcxo data buf fail\n");
        goto alloc_dcxo_data_buf_fail;
    }

    /* ����cfg�ļ� */
    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        ret = firmware_get_cfg(g_cfg_path[i], i);
        if (ret < 0) {
            if ((i == RAM_REG_TEST_CFG) || (i == RAM_BCPU_REG_TEST_CFG) ||
                (i == BFGX_AND_HITALK_CFG) || (i == HITALK_CFG)) {
                ps_print_warning("ram_reg_test_cfg or ram_bcpu_reg_test_cfg or hitalk maybe not exist, please check\n");
                continue;
            }

            ps_print_err("get cfg file [%s] fail\n", g_cfg_path[i]);
            goto cfg_file_init_fail;
        }
    }

    return SUCC;

cfg_file_init_fail:
alloc_dcxo_data_buf_fail:

    firmware_cfg_clear();

    return -EFAIL;
}

/*
 * �� �� ��  : firmware_cfg_clear
 * ��������  : �ͷ�firmware_cfg_initʱ������ڴ�
 * �� �� ֵ  : ��
 */
void firmware_cfg_clear(void)
{
    int32 i;

    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        g_cfg_info.count[i] = 0;
        if (g_cfg_info.apst_cmd[i] != NULL) {
            os_mem_kfree(g_cfg_info.apst_cmd[i]);
            g_cfg_info.apst_cmd[i] = NULL;
        }
    }

    dcxo_data_buf_free();
}

/*
 * �� �� ��  : nfc_buffer_data_recv
 * ��������  : ����nfc buffer����
 */
int32 nfc_buffer_data_recv(uint8 *tmp_data_buf, int32 len)
{
    uint32 ret = 0;
    int32 lenbuf = 0;
    int32 retry = 3;

    if (tmp_data_buf == NULL) {
        ps_print_err("tmp_data_buf is NULL\n");
        return -EFAIL;
    }

    // ��������
    while (len > lenbuf) {
        ret = read_msg(tmp_data_buf + lenbuf, len - lenbuf);
        if (ret > 0) {
            lenbuf += ret;
        } else {
            retry--;
            lenbuf = 0;
            if (retry == 0) {
                ps_print_err("time out\n");
                return -EFAIL;
            }
        }
    }
    return SUCC;
}

#define DEVICE_MEM_CHECK_SUCC 0x000f
#define GET_MEM_CHECK_FLAG    "0x50000018,4"
int32 is_device_mem_test_succ(void)
{
    int32 ret;
    int32 test_flag = 0;

    ret = number_type_cmd_send(RMEM_CMD_KEYWORD, GET_MEM_CHECK_FLAG);
    if (ret < 0) {
        ps_print_warning("send cmd %s:%s fail,ret = %d\n", RMEM_CMD_KEYWORD, GET_MEM_CHECK_FLAG, ret);
        return -1;
    }

    ret = read_msg((uint8 *)&test_flag, sizeof(test_flag));
    if (ret < 0) {
        ps_print_warning("read device test flag fail, read_len = %d, return = %d\n", (int32)sizeof(test_flag), ret);
        return -1;
    }
    ps_print_warning("get device test flag:0x%x\n", test_flag);
    if (test_flag == DEVICE_MEM_CHECK_SUCC) {
        return 0;
    }
    return -1;
}
int32 get_device_test_mem(uint8 *file_name)
{
    wlan_memdump_t *wlan_memdump_s = NULL;
    const uint32 ul_buff_len = 100;
    uint8 buff[ul_buff_len];
    int32 ret;

    if (file_name == NULL) {
        ps_print_err("reg mem test file_name is NULL!\n");
        return -EINVAL;
    }

    wlan_memdump_s = get_wlan_memdump_cfg();
    if (wlan_memdump_s == NULL) {
        ps_print_err("memdump cfg is NULL!\n");
        return -FAILURE;
    }
    ret = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "0x%x,%d", wlan_memdump_s->addr, wlan_memdump_s->len);
    if (ret < 0) {
        ps_print_warning("RMEM_CMD_KEYWORD format failed\n");
        return -FAILURE;
    }
    if (sdio_read_path_mem(RMEM_CMD_KEYWORD, buff, file_name) >= 0) {
        ps_print_warning("read device mem succ\n");
    } else {
        ps_print_warning("read device mem fail\n");
        return -FAILURE;
    }
    return 0;
}

