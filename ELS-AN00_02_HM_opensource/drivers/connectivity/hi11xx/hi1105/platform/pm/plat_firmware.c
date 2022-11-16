

/* ͷ�ļ����� */
#include "plat_firmware.h"
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/rtc.h>
#include "plat_debug.h"
#include "platform_common_clk.h"
#include "plat_uart.h"
#include "plat_cali.h"
#include "plat_pm.h"
#include "oal_sdio_host_if.h"
#include "oal_hcc_host_if.h"
#include "oal_kernel_file.h"
#include "bfgx_dev.h"
#include "bfgx_exception_rst.h"
#include "oal_util.h"
#include "securec.h"
#include "oam_ext_if.h"

/* ȫ�ֱ������� */
/* hi1103 pilot cfg�ļ�·�� */
uint8_t *g_hi1103_pilot_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1103_PILOT_PATH,
    WIFI_CFG_HI1103_PILOT_PATH,
    BFGX_CFG_HI1103_PILOT_PATH,
    RAM_CHECK_CFG_HI1103_PILOT_PATH,
};

/* hi1105 fpga cfg�ļ�·�� */
STATIC uint8_t *g_hi1105_fpga_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1105_FPGA_PATH,
    WIFI_CFG_HI1105_FPGA_PATH,
    BFGX_CFG_HI1105_FPGA_PATH,
    RAM_CHECK_CFG_HI1105_FPGA_PATH,
};

/* hi1105 asic cfg�ļ�·�� */
uint8_t *g_hi1105_asic_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1105_ASIC_PATH,
    WIFI_CFG_HI1105_ASIC_PATH,
    BFGX_CFG_HI1105_ASIC_PATH,
    RAM_CHECK_CFG_HI1105_ASIC_PATH,
};

/* hi1106 fpga cfg�ļ�·�� */
uint8_t *g_hi1106_fpga_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1106_FPGA_PATH,
    WIFI_CFG_HI1106_FPGA_PATH,
    BFGX_CFG_HI1106_FPGA_PATH,
    RAM_WIFI_CHECK_CFG_HI1106_FPGA_PATH,
};

/* hi1106 pilot cfg�ļ�·�� */
uint8_t *g_hi1106_asic_cfg_patch_in_vendor[CFG_FILE_TOTAL] = {
    BFGX_AND_WIFI_CFG_HI1106_ASIC_PATH,
    WIFI_CFG_HI1106_ASIC_PATH,
    BFGX_CFG_HI1106_ASIC_PATH,
    RAM_WIFI_CHECK_CFG_HI1106_ASIC_PATH,
};

STATIC uint32_t g_asic_type = HI1103_ASIC_PILOT;

uint8_t **g_cfg_path = g_hi1103_pilot_cfg_patch_in_vendor;

/* �洢cfg�ļ���Ϣ������cfg�ļ�ʱ��ֵ�����ص�ʱ��ʹ�øñ��� */
firmware_globals g_cfg_info;

/* ����firmware file���ݵ�buffer���Ƚ��ļ��������buffer�У�Ȼ��������device buffer���� */
STATIC uint8_t *g_firmware_down_buf = NULL;

/* DataBuf�ĳ��� */
STATIC uint32_t g_firmware_down_buf_len = 0;

#define DEVICE_MEM_DUMP_MAX_BUFF_SIZE 100
#define QUIT_TYPE_CMD_MAX_BUFF_SIZE 8

STATIC int32_t exec_number_wmem_cmd(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_sleep_cmd(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_cali_count_cmd(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_cali_bfgx_data(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_cali_dcxo_data(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_jump_cmd(uint8_t *key, uint8_t *value);
STATIC int32_t exec_number_rmem_cmd(uint8_t *key, uint8_t *value);

/* ����������� */
STATIC firmware_number g_number_type[] = {
    {WMEM_CMD_KEYWORD,  exec_number_wmem_cmd},
    {SLEEP_CMD_KEYWORD, exec_number_sleep_cmd},
    {CALI_COUNT_CMD_KEYWORD, exec_number_cali_count_cmd},
    {CALI_BFGX_DATA_CMD_KEYWORD, exec_number_cali_bfgx_data},
    {CALI_DCXO_DATA_CMD_KEYWORD, exec_number_cali_dcxo_data},
    {JUMP_CMD_KEYWORD, exec_number_jump_cmd},
    {RMEM_CMD_KEYWORD, exec_number_rmem_cmd}
};

/*
 * �� �� ��  : set_hi110x_asic_type
 * ��������  : ����hi110x asic����(FPGA/PILOT)
 * �������  : ż��-FPGA������-PILOT(�� 0-HI1103_ASIC_MPW2,1- HI1103_ASIC_PILOT)
 */
STATIC void set_hi110x_asic_type(uint32_t asic_type)
{
    g_asic_type = asic_type;
}

/*
 * �� �� ��  : get_hi110x_asic_type
 * ��������  : ��ȡhi110x asic����(FPGA/PILOT)
 * �� �� ֵ  : ż��-FPGA������-PILOT(�� 0-HI1103_ASIC_MPW2,1- HI1103_ASIC_PILOT)
 */
uint32_t get_hi110x_asic_type(void)
{
    return g_asic_type;
}

/*
 * �� �� ��  : read_msg
 * ��������  : host����device��������Ϣ
 * �������  : data: ������Ϣ��buffer
 *             data_len : ����buffer�ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʽ��յĳ���
 */
int32_t read_msg(uint8_t *data, int32_t data_len)
{
    int32_t len;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((data == NULL))) {
        ps_print_err("data is NULL\n ");
        return -EFAIL;
    }

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    len = hcc_bus_patch_read(pst_bus, data, data_len, READ_MEG_TIMEOUT);
    ps_print_dbg("Receive len=[%d] \n", len);

    return len;
}

STATIC int32_t read_msg_timeout(uint8_t *data, int32_t data_len, uint32_t timeout)
{
    int32_t len;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((data == NULL))) {
        ps_print_err("data is NULL\n ");
        return -EFAIL;
    }

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    len = hcc_bus_patch_read(pst_bus, data, data_len, timeout);
    ps_print_dbg("Receive len=[%d], data = [%s]\n", len, data);

    return len;
}

/*
 * �� �� ��  : send_msg
 * ��������  : host��device������Ϣ
 * �������  : data: ����buffer
 *             len : �������ݵĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʷ��͵ĳ���
 */
int32_t send_msg(uint8_t *data, int32_t len)
{
    int32_t l_ret;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();

    if (unlikely((pst_bus == NULL))) {
        ps_print_err("pst_bus is NULL\n ");
        return -EFAIL;
    }

    ps_print_dbg("len = %d\n", len);
#ifdef HW_DEBUG
    const uint32_t ul_max_print_len = 128;
    print_hex_dump_bytes("send_msg :", DUMP_PREFIX_ADDRESS, data,
                         (len < ul_max_print_len ? len : ul_max_print_len));
#endif
    l_ret = hcc_bus_patch_write(pst_bus, data, len);

    return l_ret;
}

/*
 * �� �� ��  : recv_expect_result
 * ��������  : ����host����device��ȷ���ص�����
 * �������  : expect: ����device��ȷ���ص�����
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32_t recv_expect_result(const uint8_t *expect)
{
    uint8_t buf[RECV_BUF_LEN];
    int32_t len;
    int32_t i;

    if (!os_str_len(expect)) {
        ps_print_dbg("not wait device to respond!\n");
        return SUCC;
    }

    memset_s(buf, RECV_BUF_LEN, 0, RECV_BUF_LEN);
    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        len = read_msg(buf, RECV_BUF_LEN);
        if (len < 0) {
            ps_print_err("recv result fail\n");
            continue;
        }

        if (!os_mem_cmp(buf, expect, os_str_len(expect))) {
            ps_print_dbg(" send SUCC, expect [%s] ok\n", expect);
            return SUCC;
        } else {
            ps_print_warning(" error result[%s], expect [%s], read result again\n", buf, expect);
        }
    }

    return -EFAIL;
}

STATIC int32_t recv_expect_result_timeout(const uint8_t *expect, uint32_t timeout)
{
    uint8_t buf[RECV_BUF_LEN];
    int32_t len;

    if (!os_str_len(expect)) {
        ps_print_dbg("not wait device to respond!\n");
        return SUCC;
    }

    memset_s(buf, RECV_BUF_LEN, 0, RECV_BUF_LEN);
    len = read_msg_timeout(buf, RECV_BUF_LEN, timeout);
    if (len < 0) {
        ps_print_err("recv result fail\n");
        return -EFAIL;
    }

    if (!os_mem_cmp(buf, expect, os_str_len(expect))) {
        ps_print_dbg(" send SUCC, expect [%s] ok\n", expect);
        return SUCC;
    } else {
        ps_print_warning(" error result[%s], expect [%s], read result again\n", buf, expect);
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
STATIC int32_t msg_send_and_recv_except(uint8_t *data, int32_t len, const uint8_t *expect)
{
    int32_t i;
    int32_t ret;

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
STATIC void *malloc_cmd_buf(uint8_t *puc_cfg_info_buf, uint32_t idx)
{
    int32_t l_len;
    uint8_t *flag = NULL;
    uint8_t *p_buf = NULL;

    if (puc_cfg_info_buf == NULL) {
        ps_print_err("malloc_cmd_buf: buf is NULL!\n");
        return NULL;
    }

    /* ͳ��������� */
    flag = puc_cfg_info_buf;
    g_cfg_info.count[idx] = 0;
    while (flag != NULL) {
        /* һ����ȷ�������н�����Ϊ ; */
        flag = os_str_chr(flag, CMD_LINE_SIGN);
        if (flag == NULL) {
            break;
        }
        g_cfg_info.count[idx]++;
        flag++;
    }
    ps_print_dbg("cfg file cmd count: al_count[%d] = %d\n", idx, g_cfg_info.count[idx]);

    /* ����洢����ռ� */
    l_len = ((g_cfg_info.count[idx]) + CFG_INFO_RESERVE_LEN) * sizeof(struct cmd_type_st);
    p_buf = os_kmalloc_gfp(l_len);
    if (p_buf == NULL) {
        ps_print_err("kmalloc cmd_type_st fail\n");
        return NULL;
    }
    memset_s((void *)p_buf, l_len, 0, l_len);

    return p_buf;
}

/*
 * �� �� ��  : delete_space
 * ��������  : ɾ���ַ������߶���Ŀո�
 * �������  : string: ԭʼ�ַ���
 *            len   : �ַ����ĳ���
 * �� �� ֵ  : ���󷵻�NULL�����򷵻�ɾ�����߿ո��Ժ��ַ������׵�ַ
 */
uint8_t *delete_space(uint8_t *string, int32_t *len)
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
STATIC int32_t string_to_num(const char *string, int32_t *number)
{
    int32_t i;
    int32_t l_num;

    if (string == NULL) {
        ps_print_err("string is NULL!\n");
        return -EFAIL;
    }

    l_num = 0;
    for (i = 0; (string[i] >= '0') && (string[i] <= '9'); i++) {
        l_num = (l_num * 10) + (string[i] - '0'); /* �ַ���ת�����߼���Ҫ*10 */
    }

    *number = l_num;

    return SUCC;
}

/*
 * �� �� ��  : num_to_string
 * ��������  : ��������ת�����ַ���
 * �������  : number:�����������
 * �������  : number:������ת���Ժ���ַ���
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
STATIC int32_t num_to_string(uint8_t *string, uint32_t number)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t tmp[INT32_STR_LEN];
    uint32_t num = number;

    if (string == NULL) {
        ps_print_err("string is NULL!\n");
        return -EFAIL;
    }

    do {
        tmp[i] = num % 10; /* ��������ת�ַ����߼���Ҫ%10 */
        num = num / 10; /* ��������ת�ַ����߼���Ҫ/10 */
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
STATIC os_kernel_file *open_file_to_readm(uint8_t *name)
{
    os_kernel_file *fp = NULL;
    uint8_t *file_name = NULL;
    mm_segment_t fs;

    if (name == NULL) {
        file_name = WIFI_DUMP_PATH "/readm_wifi";
    } else {
        file_name = name;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(file_name, O_RDWR | O_CREAT, 0664); /* 0664Ȩ�ޣ�ӵ���߿ɶ�д */
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
STATIC int32_t recv_device_mem(os_kernel_file *fp, uint8_t *data_buf, int32_t len)
{
    int32_t l_ret = -EFAIL;
    mm_segment_t fs;
    uint8_t retry = 3;
    int32_t lenbuf = 0;

    if (oal_is_err_or_null(fp)) {
        ps_print_err("fp is error,fp = 0x%p\n", fp);
        return -EFAIL;
    }

    if (data_buf == NULL) {
        ps_print_err("data_buf is NULL\n");
        return -EFAIL;
    }

    ps_print_dbg("expect recv len is [%d]\n", len);

    fs = get_fs();
    set_fs(KERNEL_DS);
    ps_print_dbg("pos = %d\n", (int)fp->f_pos);
    while (len > lenbuf) {
        l_ret = read_msg(data_buf + lenbuf, len - lenbuf);
        if (l_ret > 0) {
            lenbuf += l_ret;
        } else {
            retry--;
            lenbuf = 0;
            if (retry == 0) {
                l_ret = -EFAIL;
                ps_print_err("time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        oal_kernel_file_write(fp, data_buf, len);
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
    vfs_fsync(fp, 0);
#else
    vfs_fsync(fp, fp->f_path.dentry, 0);
#endif
    set_fs(fs);

    return l_ret;
}

/*
 * �� �� ��  : check_version
 * ��������  : ���������device�汾�ţ������device�ϱ��İ汾�ź�host�İ汾���Ƿ�ƥ��
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
STATIC int32_t check_version(void)
{
    int32_t l_ret;
    int32_t l_len;
    int32_t i;
    uint8_t rec_buf[VERSION_LEN];

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        memset_s(rec_buf, VERSION_LEN, 0, VERSION_LEN);

        l_ret = memcpy_s(rec_buf, sizeof(rec_buf), (uint8_t *)VER_CMD_KEYWORD, os_str_len(VER_CMD_KEYWORD));
        if (l_ret != EOK) {
            ps_print_err("rec_buf not enough\n");
            return -EFAIL;
        }
        l_len = os_str_len(VER_CMD_KEYWORD);

        rec_buf[l_len] = COMPART_KEYWORD;
        l_len++;

        l_ret = send_msg(rec_buf, l_len);
        if (l_ret < 0) {
            ps_print_err("send version fail![%d]\n", i);
            continue;
        }

        memset_s(g_cfg_info.dev_version, VERSION_LEN, 0, VERSION_LEN);
        memset_s(rec_buf, VERSION_LEN, 0, VERSION_LEN);
        msleep(1);

        l_ret = read_msg(rec_buf, VERSION_LEN);
        if (l_ret < 0) {
            ps_print_err("read version fail![%d]\n", i);
            continue;
        }

        memcpy_s(g_cfg_info.dev_version, VERSION_LEN, rec_buf, VERSION_LEN);

        if (!os_mem_cmp((char *)g_cfg_info.dev_version,
                        (char *)g_cfg_info.cfg_version,
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
 *             Value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ������ʾ�ɹ�
 */
int32_t number_type_cmd_send(const char *key, const char *value)
{
    int32_t l_ret;
    int32_t data_len;
    int32_t value_len;
    int32_t i;
    int32_t n;
    uint8_t num[INT32_STR_LEN];
    uint8_t buff_tx[SEND_BUF_LEN];

    value_len = os_str_len(value);

    memset_s(num, INT32_STR_LEN, 0, INT32_STR_LEN);
    memset_s(buff_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

    data_len = 0;
    data_len = os_str_len(key);
    l_ret = memcpy_s(buff_tx, sizeof(buff_tx), key, data_len);
    if (l_ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }

    buff_tx[data_len] = COMPART_KEYWORD;
    data_len = data_len + 1;

    for (i = 0, n = 0; (i <= value_len) && (n < INT32_STR_LEN); i++) {
        if ((value[i] == ',') || (value_len == i)) {
            ps_print_dbg("num = %s, i = %d, n = %d\n", num, i, n);
            if (n == 0) {
                continue;
            }
            l_ret = memcpy_s((uint8_t *)&buff_tx[data_len], sizeof(buff_tx) - data_len, num, n);
            if (l_ret != EOK) {
                ps_print_err("buff_tx not enough\n");
                return -EFAIL;
            }
            data_len = data_len + n;

            buff_tx[data_len] = COMPART_KEYWORD;
            data_len = data_len + 1;

            memset_s(num, INT32_STR_LEN, 0, INT32_STR_LEN);
            n = 0;
        } else if (value[i] == COMPART_KEYWORD) {
            continue;
        } else {
            num[n] = value[i];
            n++;
        }
    }

    l_ret = send_msg(buff_tx, data_len);

    return l_ret;
}

/*
 * �� �� ��  : update_device_cali_count
 * ��������  : ʹ��WRITEM�������device��У׼�������״��ϵ�ʱΪȫ0
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 *             recv_expect_result
 */
STATIC int32_t update_device_cali_count(uint8_t *key, uint8_t *value)
{
    int32_t l_ret;
    uint32_t len, value_len;
    uint32_t number = 0;
    uint8_t *addr = NULL;
    uint8_t buff_tx[SEND_BUF_LEN];

    /* �������Value�ַ��������Valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
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
    l_ret = memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len);
    if (l_ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len] = ',';
    len++;

    /* buff_tx="4,0xXXX," */
    l_ret = get_cali_count(&number);
    l_ret += num_to_string(&buff_tx[len], number);

    /* ��ʱbuff_tx="4,0xXXX,value" */
    /* ʹ��WMEM_CMD_KEYWORD������device����У׼���� */
    l_ret += number_type_cmd_send(WMEM_CMD_KEYWORD, buff_tx);
    if (l_ret < 0) {
        ps_print_err("send key=[%s],value=[%s] fail\n", key, buff_tx);
        return l_ret;
    }

    l_ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
    if (l_ret < 0) {
        ps_print_err("recv expect result fail!\n");
        return l_ret;
    }

    return SUCC;
}

/*
 * �� �� ��  : download_bfgx_cali_data
 * ��������  : ʹ��files�������bfgx��У׼����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 *             recv_expect_result
 */
STATIC int32_t download_bfgx_cali_data(uint8_t *key, uint8_t *value)
{
    int32_t l_ret;
    uint32_t len;
    uint32_t value_len;
    uint8_t *addr = NULL;
    uint8_t buff_tx[SEND_BUF_LEN];

    /* �������Value�ַ��������Valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
    /* ����Ժ����ʽΪ"FILES �ļ����� Ҫд�ĵ�ַ"---"FILES 1 0xXXXX " */
    memset_s(buff_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

    /* buff_tx="" */
    len = os_str_len(key);
    l_ret = memcpy_s(buff_tx, sizeof(buff_tx), key, len);
    if (l_ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    buff_tx[len++] = COMPART_KEYWORD;

    /* buff_tx="FILES " */
    buff_tx[len++] = '1';
    buff_tx[len++] = COMPART_KEYWORD;

    /* buff_tx="FILES 1 " */
    value_len = os_str_len(value);
    addr = delete_space(value, &value_len);
    if (addr == NULL) {
        ps_print_err("addr is NULL, value[%s] value_len[%d]", value, value_len);
        return -EFAIL;
    }
    l_ret = memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len);
    if (l_ret != EOK) {
        ps_print_err("buff_tx not enough\n");
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len++] = COMPART_KEYWORD;

    ps_print_info("download bfgx cali data addr:%s\n", addr);

    /* buff_tx="FILES 1 0xXXXX " */
    /* ���͵�ַ */
    l_ret = msg_send_and_recv_except(buff_tx, len, MSG_FROM_DEV_READY_OK);
    if (l_ret < 0) {
        ps_print_err("SEND [%s] addr error\n", key);
        return -EFAIL;
    }

    /* ��ȡbfgxУ׼���� */
    l_ret = get_bfgx_cali_data(g_firmware_down_buf, &len, g_firmware_down_buf_len);
    if (l_ret < 0) {
        ps_print_err("get bfgx cali data failed, len=%d\n", len);
        return -EFAIL;
    }

    /* Wait at least 5 ms */
    oal_usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);

    /* ����bfgxУ׼���� */
    l_ret = msg_send_and_recv_except(g_firmware_down_buf, len, MSG_FROM_DEV_FILES_OK);
    if (l_ret < 0) {
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
STATIC int32_t download_dcxo_cali_data(uint8_t *key, uint8_t *value)
{
    int32_t l_ret;
    uint32_t len;
    uint32_t value_len;
    uint8_t *addr = NULL;
    uint8_t *dcxo_buffer_addr = NULL;
    uint8_t buff_tx[SEND_BUF_LEN];

    dcxo_buffer_addr = (uint8_t *)get_dcxo_data_buf_addr();
    if (dcxo_buffer_addr == NULL) {
        ps_print_err("[dcxo] buffer alloc error");
        return -EFAIL;
    }

    /* �������Value�ַ��������Valueֻ��һ����ַ����ʽΪ"0xXXXXX" */
    /* ����Ժ����ʽΪ"FILES �ļ����� Ҫд�ĵ�ַ"---"FILES 1 0xXXXX " */
    memset_s(buff_tx, sizeof(buff_tx), 0, SEND_BUF_LEN);

    /* buff_tx="" */
    len = os_str_len(key);
    if (memcpy_s(buff_tx, sizeof(buff_tx), key, len) != EOK) {
        ps_print_err("memcpy_s faild");
        return -EFAIL;
    }
    buff_tx[len++] = COMPART_KEYWORD;

    /* buff_tx="FILES " */
    buff_tx[len++] = '1';
    buff_tx[len++] = COMPART_KEYWORD;

    /* buff_tx="FILES 1 " */
    value_len = os_str_len(value);
    addr = delete_space(value, &value_len);
    if (addr == NULL) {
        ps_print_err("addr is NULL, Value[%s] Value_len[%d]", value, value_len);
        return -EFAIL;
    }

    if (memcpy_s(&buff_tx[len], sizeof(buff_tx) - len, addr, value_len) != EOK) {
        ps_print_err("memcpy_s faild");
        return -EFAIL;
    }
    len += value_len;
    buff_tx[len++] = COMPART_KEYWORD;

    /* buff_tx="FILES 1 0xXXXX " */
    /* ���͵�ַ */
    l_ret = msg_send_and_recv_except(buff_tx, len, MSG_FROM_DEV_READY_OK);
    if (l_ret < 0) {
        ps_print_err("SEND [%s] addr error\n", key);
        return -EFAIL;
    }

    oal_usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);

    /* ����dcxoУ׼���� */
    l_ret = msg_send_and_recv_except(dcxo_buffer_addr, DCXO_CALI_DATA_BUF_LEN, MSG_FROM_DEV_FILES_OK);
    if (l_ret < 0) {
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
STATIC int32_t parse_file_cmd(uint8_t *string, unsigned long *addr, char **file_path)
{
    uint8_t *tmp = NULL;
    int32_t count = 0;
    char *after = NULL;

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

    /* "1,0xXXXX,file_path"
     *         ^
     *       after
     */
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

STATIC void oal_print_wcpu_reg(uint32_t *pst_buf, uint32_t ul_size)
{
    int32_t i = 0;
    int32_t remain = (int32_t)ul_size; /* per dword */
    if (ul_size) {
        oam_error_log0(0, OAM_SF_ANY, "print wcpu registers:");
    }

    forever_loop() {
        if (remain >= 4) { /* ����Ҫ��ӡ��32bit�Ĵ����������ڵ���4ʱ��һ���Դ�ӡ4�� */
            oam_error_log4(0, OAM_SF_ANY, "wcpu_reg: %x %x %x %x",
                           *(pst_buf + i + 0), *(pst_buf + i + 1),
                           *(pst_buf + i + 2), *(pst_buf + i + 3)); /* ָ������ƶ�2,3 */
            i += 4; /* ��ӡ����+4 */
            remain -= 4; /* �Ĵ�������-4 */
        } else if (remain >= 3) { /* ����Ҫ��ӡ��32bit�Ĵ�����������3ʱ��һ���Դ�ӡ3�� */
            oam_error_log3(0, OAM_SF_ANY, "wcpu_reg: %x %x %x",
                           *(pst_buf + i + 0), *(pst_buf + i + 1),
                           *(pst_buf + i + 2)); /* ָ������ƶ�2 */
            i += 3; /* ��ӡ����+3 */
            remain -= 3; /* �Ĵ�������-3 */
        } else if (remain >= 2) { /* ����Ҫ��ӡ��32bit�Ĵ�����������2ʱ��һ���Դ�ӡ2�� */
            oam_error_log2(0, OAM_SF_ANY, "wcpu_reg: %x %x",
                           *(pst_buf + i + 0), *(pst_buf + i + 1));
            i += 2; /* ��ӡ����+2 */
            remain -= 2; /* �Ĵ�������-2 */
        } else if (remain >= 1) { /* ����Ҫ��ӡ��32bit�Ĵ�����������1ʱ��һ���Դ�ӡ1�� */
            oam_error_log1(0, OAM_SF_ANY, "wcpu_reg: %x",
                           *(pst_buf + i + 0));
            i += 1; /* ��ӡ����+1 */
            remain -= 1; /* �Ĵ�������-1 */
        } else {
            break;
        }
    }
}

#define READ_DEVICE_MAX_BUF_SIZE 128
/* read device reg by bootloader */
int32_t read_device_reg16(uint32_t address, uint16_t *value)
{
    int32_t ret, buf_len;
    const uint32_t ul_read_msg_len = 4;
    const uint32_t ul_dump_len = 8;
    uint8_t buf_tx[READ_DEVICE_MAX_BUF_SIZE];
    uint8_t buf_result[READ_DEVICE_MAX_BUF_SIZE];
    void *addr = (void *)buf_result;

    memset_s(buf_tx, READ_DEVICE_MAX_BUF_SIZE, 0, READ_DEVICE_MAX_BUF_SIZE);
    memset_s(buf_result, READ_DEVICE_MAX_BUF_SIZE, 0, READ_DEVICE_MAX_BUF_SIZE);

    buf_len = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c0x%x%c%d%c",
                         RMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         address,
                         COMPART_KEYWORD,
                         4, /* 4��ʾ���� */
                         COMPART_KEYWORD); /* ��� READM 0x... 4 ���������4��ʾ���� */
    if (buf_len < 0) {
        ps_print_err("log str format err line[%d]\n", __LINE__);
        return buf_len;
    }

    ps_print_info("%s", buf_tx);

    ret = send_msg(buf_tx, buf_len);
    if (ret < 0) {
        ps_print_err("send msg [%s] failed, ret=%d", buf_tx, ret);
        return ret;
    }

    ret = read_msg(buf_result, ul_read_msg_len);
    if (ret > 0) {
        /* �����ض����ڴ�,����С��ֱ��ת�� */
        *value = (uint16_t)oal_readl((uintptr_t)addr);
        oal_print_hex_dump(buf_result, ul_dump_len, HEX_DUMP_GROUP_SIZE, "reg16: ");
        return 0;
    }

    ps_print_err("read_device_reg16 failed, ret=%d", ret);

    return -1;
}

/* write device regs by bootloader */
int32_t write_device_reg16(uint32_t address, uint16_t value)
{
    int32_t ret, buf_len;
    uint8_t buf_tx[READ_DEVICE_MAX_BUF_SIZE];

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
        ps_print_err("log str format err line[%d]\n", __LINE__);
        return buf_len;
    }

    ps_print_info("%s", buf_tx);

    ret = send_msg(buf_tx, buf_len);
    if (ret < 0) {
        ps_print_err("send msg [%s] failed, ret=%d", buf_tx, ret);
        return ret;
    }

    ret = recv_expect_result(MSG_FROM_DEV_WRITEM_OK);
    if (ret < 0) {
        ps_print_err("send msg [%s] recv failed, ret=%d", buf_tx, ret);
        return ret;
    }

    return 0;
}

STATIC int32_t recv_device_memdump(uint8_t *data_buf, int32_t len)
{
    int32_t l_ret = -EFAIL;
    uint8_t retry = 3;
    int32_t lenbuf = 0;

    if (data_buf == NULL) {
        ps_print_err("data_buf is NULL\n");
        return -EFAIL;
    }

    ps_print_dbg("expect recv len is [%d]\n", len);

    while (len > lenbuf) {
        l_ret = read_msg(data_buf + lenbuf, len - lenbuf);
        if (l_ret > 0) {
            lenbuf += l_ret;
        } else {
            retry--;
            lenbuf = 0;
            if (retry == 0) {
                l_ret = -EFAIL;
                ps_print_err("time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        wifi_memdump_enquenue(data_buf, len);
    }

    return l_ret;
}

static int32_t sdio_read_device_mem(struct st_wifi_dump_mem_info *pst_mem_dump_info,
                                    uint8_t *data_buf, uint32_t data_buf_len)
{
    uint8_t buf_tx[SEND_BUF_LEN];
    int32_t ret = 0;
    uint32_t size = 0;
    uint32_t offset;
    uint32_t remainder = pst_mem_dump_info->size;

    offset = 0;
    while (remainder > 0) {
        memset_s(buf_tx, SEND_BUF_LEN, 0, SEND_BUF_LEN);

        size = min(remainder, data_buf_len);
        ret = snprintf_s(buf_tx, sizeof(buf_tx), sizeof(buf_tx) - 1, "%s%c0x%lx%c%d%c",
                         RMEM_CMD_KEYWORD,
                         COMPART_KEYWORD,
                         pst_mem_dump_info->mem_addr + offset,
                         COMPART_KEYWORD,
                         size,
                         COMPART_KEYWORD);
        if (ret < 0) {
            ps_print_err("log str format err line[%d]\n", __LINE__);
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
            int8_t *pst_file_name = (pst_mem_dump_info->file_name ?
                                       ((int8_t *)pst_mem_dump_info->file_name) : (int8_t *)"default: ");
            if (!oal_strcmp("wifi_device_panic_mem", pst_file_name) && (size > CPU_PANIC_MEMDUMP_SIZE)) {
                oal_print_hex_dump(data_buf, CPU_PANIC_MEMDUMP_SIZE, HEX_DUMP_GROUP_SIZE, pst_file_name);
                /* ��Ĵ��������4�ֽڣ������������Ҫ��4 */
                oal_print_wcpu_reg((uint32_t *)(data_buf), CPU_PANIC_MEMDUMP_SIZE / 4);
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
int32_t wifi_device_mem_dump(struct st_wifi_dump_mem_info *mem_dump_info, uint32_t count)
{
    int32_t ret = -EFAIL;
    uint32_t i;
    uint8_t *data_buf = NULL;
    uint8_t buff[DEVICE_MEM_DUMP_MAX_BUFF_SIZE];
    uint32_t *pcount = (uint32_t *)&buff[0];
    uint32_t sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());

    if (!ps_is_device_log_enable()) {
        return 0;
    }

    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);
    if ((mem_dump_info == NULL) || (sdio_transfer_limit == 0)) {
        ps_print_err("pst_wifi_dump_info is NULL, or sdio_transfer_limit is 0\n");
        return -EFAIL;
    }

    do {
        ps_print_info("try to malloc mem dump buf len is [%d]\n", sdio_transfer_limit);
        data_buf = (uint8_t *)os_kmalloc_gfp(sdio_transfer_limit);
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
        *pcount = mem_dump_info[i].size;
        ps_print_info("mem dump data size [%d]==> [%d]\n", *pcount, mem_dump_info[i].size);
        wifi_memdump_enquenue(buff, 4); /* ��������sk_buff�Ĵ�СΪ4 */
        ret = sdio_read_device_mem(&mem_dump_info[i], data_buf, sdio_transfer_limit);
        if (ret < 0) {
            break;
        }
    }
    wifi_memdump_finish();

    os_mem_kfree(data_buf);

    return ret;
}

STATIC uint32_t get_read_mem_cmd_resopne_size(const char *cmd_value)
{
    uint32_t size = 0;

    /* cmd_value : "address, len" */
    const char *flag = os_str_chr(cmd_value, ',');
    if (flag == NULL) {
        ps_print_err("RECV LEN ERROR..\n");
        return 0;
    }
    flag++;
    ps_print_dbg("recv len [%s]\n", flag);
    while (*flag == COMPART_KEYWORD) {
        flag++;
    }
    string_to_num(flag, &size);

    return size;
}

STATIC int32_t alloc_sdio_dump_rx_buf(uint8_t **rx_buf, uint32_t *buf_len)
{
    uint8_t *data_buf = NULL;
    /* ���ڴ��ȿ��ǳɹ���, ҳ��С������ڴ���������ɹ��� */
    uint32_t sdio_transfer_limit = hcc_get_max_trans_size(hcc_get_110x_handler());
    sdio_transfer_limit = oal_min(PAGE_SIZE, sdio_transfer_limit);

    do {
        ps_print_info("try to malloc sdio mem read buf len is [%d]\n", sdio_transfer_limit);
        data_buf = (uint8_t *)os_kmalloc_gfp(sdio_transfer_limit);
        if (data_buf == NULL) {
            ps_print_warning("malloc mem len [%d] fail, continue to try in a smaller size\n", sdio_transfer_limit);
            sdio_transfer_limit = sdio_transfer_limit >> 1;
        }
    } while ((data_buf == NULL) && (sdio_transfer_limit >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (data_buf == NULL) {
        ps_print_err("data_buf KMALLOC failed\n");
        return -EFAIL;
    }

    *rx_buf = data_buf;
    *buf_len = sdio_transfer_limit;

    return SUCC;
}

STATIC int32_t sdio_read_mem(const char *key, const char *value, bool is_wifi)
{
    int32_t l_ret;
    uint32_t size, readlen;
    int32_t retry = 3;
    os_kernel_file *fp = NULL;
    uint8_t *data_buf = NULL;
    uint32_t sdio_transfer_limit;

    size = get_read_mem_cmd_resopne_size(value);
    if (size == 0) {
        return -EFAIL;
    }

    if (alloc_sdio_dump_rx_buf(&data_buf, &sdio_transfer_limit) != SUCC) {
        return -EFAIL;
    }

    fp = open_file_to_readm(is_wifi == true ? WIFI_DUMP_PATH "/readm_wifi" : WIFI_DUMP_PATH "/readm_bfgx");
    if (IS_ERR(fp)) {
        ps_print_err("create file error,fp = 0x%p\n", fp);
        os_mem_kfree(data_buf);
        return SUCC;
    }

    l_ret = number_type_cmd_send(key, value);
    if (l_ret < 0) {
        ps_print_err("send %s,%s fail \n", key, value);
        oal_file_close(fp);
        os_mem_kfree(data_buf);
        return l_ret;
    }

    ps_print_dbg("recv len [%d]\n", size);
    while (size > 0) {
        readlen = min(size, sdio_transfer_limit);
        l_ret = recv_device_mem(fp, data_buf, readlen);
        if (l_ret > 0) {
            size -= l_ret;
        } else {
            ps_print_err("read error retry:%d\n", retry);
            --retry;
            if (!retry) {
                ps_print_err("retry fail\n");
                break;
            }
        }
    }

    oal_file_close(fp);
    os_mem_kfree(data_buf);

    return l_ret;
}

/*
 * �� �� ��  : exec_number_wmem_cmd
 * ��������  : дw mem���ڴ��ַ
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_wmem_cmd(uint8_t *key, uint8_t *value)
{
    int32_t ret;

    if ((os_str_str((char *)value, (char *)STR_REG_PMU_CLK_REQ) != NULL) && pmu_clk_request_enable()) {
        ps_print_info("hi110x PMU clk request\n");
        return SUCC;
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

    return SUCC;
}

/*
 * �� �� ��  : exec_number_sleep_cmd
 * ��������  : host sleep��ʱ
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_sleep_cmd(uint8_t *key, uint8_t *value)
{
    int32_t delay_ms;

    delay_ms = simple_strtol(value, NULL, 10); /* ���ַ���ת����10������ */
    ps_print_info("firmware download delay %d ms\n", delay_ms);
    // ���ȴ�ʱ��5s����ֹcfg����ʱ��̫�����¼��س�ʱ
    if (delay_ms > 0 && delay_ms < 5000) { /* ��ʱ��ʱ����0��5000ms֮�� */
        msleep(delay_ms);
    } else {
        msleep(5); /* delay 5ms */
    }

    return SUCC;
}

/*
 * �� �� ��  : exec_number_cali_count_cmd
 * ��������  : ����У׼������device
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_cali_count_cmd(uint8_t *key, uint8_t *value)
{
    int32_t ret;

    ret = update_device_cali_count(key, value);
    if (ret < 0) {
        ps_print_err("update device cali count fail\n");
    }
    return ret;
}

/*
 * �� �� ��  : exec_number_cali_bfgx_data
 * ��������  : ����bfgxУ׼����
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_cali_bfgx_data(uint8_t *key, uint8_t *value)
{
    int32_t ret;
#ifndef  _PRE_WINDOWS_SUPPORT
    if (oal_atomic_read(&g_ir_only_mode) != 0) {
        ps_print_info("ir only pass the download cali data cmd\n");
        return SUCC;
    }
#endif
    /* ����BFGX��У׼���� */
    ret = download_bfgx_cali_data(FILES_CMD_KEYWORD, value);
    if (ret < 0) {
        ps_print_err("download bfgx cali data fail\n");
    }

    return ret;
}

/*
 * �� �� ��  : exec_number_cali_bfgx_data
 * ��������  : ���ع�ʱ����ص�����
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_cali_dcxo_data(uint8_t *key, uint8_t *value)
{
    int32_t ret;
    ps_print_dbg("download dcxo cali data begin\n");
    /* ����TCXO ��У׼������devcie */
    ret = download_dcxo_cali_data(FILES_CMD_KEYWORD, value);
    if (ret < 0) {
        ps_print_err("download dcxo cali data fail\n");
    }

    return ret;
}

/*
 * �� �� ��  : exec_number_jump_cmd
 * ��������  : ִ��deviceָ����ַ�ĺ���
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_jump_cmd(uint8_t *key, uint8_t *value)
{
    int32_t ret;
    ret = number_type_cmd_send(key, value);
    if (ret < 0) {
        ps_print_err("send key=[%s],value=[%s] fail\n", key, value);
        return ret;
    }

    /* 100000ms timeout */
    ret = recv_expect_result_timeout(MSG_FROM_DEV_JUMP_OK, READ_MEG_JUMP_TIMEOUT);
    if (ret >= 0) {
        ps_print_info("JUMP success!\n");
    } else {
        ps_print_err("CMD JUMP timeout! l_ret=%d\n", ret);
    }

    return ret;
}

/*
 * �� �� ��  : exec_number_rmem_cmd
 * ��������  : ��device���ڴ�
 * �� �� ֵ  : 0��ʾ�ɹ�������ֵ��ʾʧ��
 */
STATIC int32_t exec_number_rmem_cmd(uint8_t *key, uint8_t *value)
{
    return sdio_read_mem(key, value, true);
}

/*
 * �� �� ��  : exec_number_type_cmd
 * ��������  : ִ��number���͵�����
 * �������  : Key  : ����Ĺؼ���
 *             Value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
STATIC int32_t exec_number_type_cmd(uint8_t *key, uint8_t *value)
{
    int32_t ret = -EFAIL;
    int32_t loop;

    if (!os_mem_cmp(key, VER_CMD_KEYWORD, os_str_len(VER_CMD_KEYWORD))) {
        ret = check_version();
        if (ret < 0) {
            ps_print_err("check version FAIL [%d]\n", ret);
            return -EFAIL;
        }
    }

    for (loop = 0; loop < oal_array_size(g_number_type); loop++) {
        if (!os_str_cmp(key, g_number_type[loop].key)) {
            ret = (g_number_type[loop].cmd_exec)(key, value);
            break;
        }
    }

    return ret;
}

/*
 * �� �� ��  : exec_quit_type_cmd
 * ��������  : ִ��quit���͵�����
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
STATIC int32_t exec_quit_type_cmd(void)
{
    int32_t ret;
    int32_t l_len;
    uint8_t buf[QUIT_TYPE_CMD_MAX_BUFF_SIZE];

    memset_s(buf, sizeof(buf), 0, sizeof(buf));

    ret = memcpy_s(buf, sizeof(buf), (uint8_t *)QUIT_CMD_KEYWORD, os_str_len(QUIT_CMD_KEYWORD));
    if (ret != EOK) {
        ps_print_err("buf not enough\n");
        return -EFAIL;
    }
    l_len = os_str_len(QUIT_CMD_KEYWORD);

    buf[l_len] = COMPART_KEYWORD;
    l_len++;

    ret = msg_send_and_recv_except(buf, l_len, MSG_FROM_DEV_QUIT_OK);

    return ret;
}

STATIC int32_t file_open_get_len(char *path, os_kernel_file **fp, uint32_t *file_len)
{
#define RETRY_CONT 3
    mm_segment_t fs;
    int32_t i = 0;

    fs = get_fs();
    set_fs(KERNEL_DS);
    for (i = 0; i < RETRY_CONT; i++) {
        *fp = filp_open(path, O_RDONLY, 0);
        if (oal_is_err_or_null(*fp)) {
            ps_print_err("filp_open [%s] fail!!, fp=%pK, errno:%ld\n", path, *fp, PTR_ERR(*fp));
            *fp = NULL;
            continue;
        }
    }

    if (*fp == NULL) {
        ps_print_err("filp_open fail!!\n");
        set_fs(fs);
        return -OAL_ENOPERMI;
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

STATIC int32_t firmware_file_send(os_kernel_file *fp, uint32_t file_len, unsigned long addr)
{
    unsigned long addr_send;
    uint32_t per_send_len;
    uint32_t send_count;
    int32_t rdlen;
    int32_t ret;
    uint32_t i;
    uint32_t offset = 0;
    uint8_t buff_tx[SEND_BUF_LEN] = {0};

    // g_firmware_down_buf_len�ڵ���ǰ��֤Ϊ����0
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
            ps_print_err("log str format err line[%d]\n", __LINE__);
            break;
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

/*
 * �� �� ��  : exec_file_type_cmd
 * ��������  : ִ��file���͵�����
 * �������  : Key  : ����Ĺؼ���
 *             Value: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
 */
STATIC int32_t exec_file_type_cmd(uint8_t *key, uint8_t *value)
{
    unsigned long addr;
    char *path = NULL;
    int32_t ret;
    uint32_t file_len;
    os_kernel_file *fp = NULL;

#ifndef  _PRE_WINDOWS_SUPPORT
    if (oal_atomic_read(&g_ir_only_mode) != 0) {
        ps_print_info("ir only pass the download file cmd\n");
        return SUCC;
    }
#endif
    ret = parse_file_cmd(value, &addr, &path);
    if (ret < 0) {
        ps_print_err("parse file cmd fail!\n");
        return ret;
    }

    ps_print_info("download firmware:%s addr:0x%x\n", path, (uint32_t)addr);

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
 * �� �� ��  : execute_download_cmd
 * ��������  : ִ��firmware download������
 * �������  : cmd_type: �������������
 *             cmd_name: ����Ĺؼ���
 *             cmd_para: ����Ĳ���
 * �� �� ֵ  : -1��ʾʧ�ܣ��Ǹ�����ʾ�ɹ�
 */
STATIC int32_t execute_download_cmd(int32_t cmd_type, uint8_t *cmd_name, uint8_t *cmd_para)
{
    int32_t ret;

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
 * �������  : cfg_patch    : cfg�ļ���·��
 *             buf : ����cfg�ļ����ݵ�buffer
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32_t firmware_read_cfg(const char *cfg_patch, uint8_t *buf, uint32_t buf_len)
{
    os_kernel_file *fp = NULL;
    int32_t ret;
    mm_segment_t fs;

    if ((cfg_patch == NULL) || (buf == NULL)) {
        ps_print_err("para is NULL\n");
        return -EFAIL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(cfg_patch, O_RDONLY, 0);
    if (oal_is_err_or_null(fp)) {
        ps_print_err("open file %s fail, fp=%p, error %ld\n", cfg_patch, fp, PTR_ERR(fp));
        set_fs(fs);
        fp = NULL;
        return -EFAIL;
    }

    memset_s(buf, buf_len, 0, buf_len);

    ret = oal_file_read_ext(fp, fp->f_pos, buf, buf_len);

    filp_close(fp, NULL);
    set_fs(fs);
    fp = NULL;

    return ret;
}


/*
 * �� �� ��  : firmware_parse_special_cmd
 * ��������  : ����cfg�ļ��еĲ���"="�ַ�������������:
               QUIT
               SHUTDOWN_WIFI
               SHUTDOWN_BFGX
               BUCK_MODE
 * �� �� ֵ  : ��������������,�������������;���򷵻�-1.
 */
STATIC int32_t firmware_parse_special_cmd(uint8_t *puc_cfg_buffer, uint8_t *puc_cmd_name, uint32_t cmd_name_len,
                                          uint8_t *puc_cmd_para, uint32_t cmd_para_len)
{
#define MAX_BUFF_LEN    100
#define BUCK_PARA_LEN   16

    int32_t ret;

    /* �˳������� */
    if (os_str_str((char *)puc_cfg_buffer, QUIT_CMD_KEYWORD) != NULL) {
        return QUIT_TYPE_CMD;
    } else if (os_str_str((char *)puc_cfg_buffer, BUCK_MODE_CMD_KEYWORD) != NULL) {
        /*
         * 1105֧������buck������buck,CFG�ļ��н��йؼ��֣�ʵ��ֵ��ini�ļ��л�ȡ�����㶨�ƻ���
         * ini���ƻ���ʽ[buck_mode=2,0x50001850,value],����CFG�ļ���WRITEM���﷨,0x50001850Ϊ1105��BUCK_CUSTOM_REG
         * ��ַ, value����ʵ��Ҫ������:
         * ��1��   BIT[15,14]����ʾ�Ƿ��������buck
         *      2'b00:  ȫ����buck
         *      2'b01:  I2C���ƶ�������buck
         *      2'b10:  GPIO���ƶ�������buck
         *      2'b11:  host���ƹ�������buck��ѹ
         * ��2��   BIT[8]����ʾbuck_en,�ٵ���ѹ..
         * ��3��   BIT[7��0]: ����ͬ��Buck�����ͺ�
         */
        char buff[MAX_BUFF_LEN];
        char buck_para[BUCK_PARA_LEN];
        ps_print_info("buck_mode para,get from ini file\n");
        if (INI_FAILED == get_cust_conf_string(INI_MODU_PLAT, "buck_custom_addr", buff, sizeof(buff) - 1)) {
            ps_print_err("can't found buck_mode ini");
            return ERROR_TYPE_CMD;
        }

        oal_itoa(g_st_board_info.buck_param, buck_para, sizeof(buck_para));

        ret = strcat_s(buff, sizeof(buff), ",");
        if (ret != EOK) {
            ps_print_err("strcat_s failed!");
            return ERROR_TYPE_CMD;
        }
        ret = strcat_s(buff, sizeof(buff), buck_para);
        if (ret != EOK) {
            ps_print_err("strcat_s buck_para failed!");
            return ERROR_TYPE_CMD;
        }

        ret = memcpy_s(puc_cmd_para, cmd_para_len, buff, OAL_STRLEN(buff));
        if (ret != EOK) {
            ps_print_err("buck_mode para set error");
            return ERROR_TYPE_CMD;
        }

        ret = memcpy_s(puc_cmd_name, cmd_name_len, "WRITEM", 6); /* ��Ҫ�����ĸ���Ϊ6 */
        if (ret != EOK) {
            ps_print_err("buck_mode cmd name set error");
            return ERROR_TYPE_CMD;
        }

        return NUM_TYPE_CMD;
    }

    return ERROR_TYPE_CMD;
}

/*
 * �� �� ��  : firmware_parse_cmd
 * ��������  : ����cfg�ļ��е�����
 * �������  : puc_cfg_buffer: ����cfg�ļ����ݵ�buffer
 *             puc_cmd_name  : ��������Ժ�����ؼ��ֵ�buffer
 *             puc_cmd_para  : ��������Ժ����������buffer
 * �� �� ֵ  : �������������
 */
int32_t firmware_parse_cmd(uint8_t *cfg_buffer, uint8_t *cmd_name, uint32_t cmd_name_len,
                           uint8_t *cmd_para, uint32_t cmd_para_len)
{
    int32_t ret;
    int32_t cmd_type;
    int32_t l_cmdlen;
    int32_t l_paralen;
    uint8_t *begin = NULL;
    uint8_t *end = NULL;
    uint8_t *link = NULL;
    uint8_t *handle = NULL;
    uint8_t *handle_temp = NULL;

    begin = cfg_buffer;
    if ((cfg_buffer == NULL) || (cmd_name == NULL) || (cmd_para == NULL)) {
        ps_print_err("para is NULL\n");
        return ERROR_TYPE_CMD;
    }

    /* ע���� */
    if (cfg_buffer[0] == '@') {
        return ERROR_TYPE_CMD;
    }

    /* �����У������˳�������, ���������ֶ�,��INI�ļ��ж�̬��ȡ���� */
    link = os_str_chr((char *)begin, '=');
    if (link == NULL) {
        return firmware_parse_special_cmd(cfg_buffer, cmd_name, cmd_name_len, cmd_para, cmd_para_len);
    }

    /* �����У�û�н����� */
    end = os_str_chr(link, ';');
    if (end == NULL) {
        return ERROR_TYPE_CMD;
    }

    l_cmdlen = link - begin;

    /* ɾ���ؼ��ֵ����߿ո� */
    handle = delete_space((uint8_t *)begin, &l_cmdlen);
    if (handle == NULL) {
        return ERROR_TYPE_CMD;
    }

    /* �ж��������� */
    if ((l_cmdlen >= os_str_len(FILE_TYPE_CMD_KEY)) &&
        !os_mem_cmp(handle, FILE_TYPE_CMD_KEY, os_str_len(FILE_TYPE_CMD_KEY))) {
        handle_temp = os_str_str(handle, FILE_TYPE_CMD_KEY);
        if (handle_temp == NULL) {
            ps_print_err("'ADDR_FILE_'is not handle child string, handle=%s", handle);
            return ERROR_TYPE_CMD;
        }
        handle = handle_temp + os_str_len(FILE_TYPE_CMD_KEY);
        l_cmdlen = l_cmdlen - os_str_len(FILE_TYPE_CMD_KEY);
        cmd_type = FILE_TYPE_CMD;
    } else if ((l_cmdlen >= os_str_len(NUM_TYPE_CMD_KEY)) &&
               !os_mem_cmp(handle, NUM_TYPE_CMD_KEY, os_str_len(NUM_TYPE_CMD_KEY))) {
        handle_temp = os_str_str(handle, NUM_TYPE_CMD_KEY);
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

    ret = memcpy_s(cmd_name, cmd_name_len, handle, l_cmdlen);
    if (ret != EOK) {
        ps_print_err("cmd len out range! ret = %d\n", ret);
        return ERROR_TYPE_CMD;
    }
    /* ɾ��ֵ���߿ո� */
    begin = link + 1;
    l_paralen = end - begin;

    handle = delete_space(begin, &l_paralen);
    if (handle == NULL) {
        return ERROR_TYPE_CMD;
    }
    ret = memcpy_s(cmd_para, cmd_para_len, handle, l_paralen);
    if (ret != EOK) {
        ps_print_err("para len out of range!ret = %d\n", ret);
        return ERROR_TYPE_CMD;
    }

    return cmd_type;
}

STATIC int32_t firmware_parse_cur_cmd(uint32_t index, uint32_t pos)
{
    int32_t ret;
    uint32_t cmd_para_len = 0;

    /* ��ȡ���ð汾�� */
    if (!os_mem_cmp(g_cfg_info.cmd[index][pos].cmd_name,
                    VER_CMD_KEYWORD,
                    os_str_len(VER_CMD_KEYWORD))) {
        cmd_para_len = os_str_len(g_cfg_info.cmd[index][pos].cmd_para);
        ret = memcpy_s(g_cfg_info.cfg_version, sizeof(g_cfg_info.cfg_version),
                       g_cfg_info.cmd[index][pos].cmd_para, cmd_para_len);
        if (ret != EOK) {
            ps_print_dbg("cmd_para_len = %d over auc_CfgVersion length", cmd_para_len);
            return -EFAIL;
        }

        ps_print_dbg("g_CfgVersion = [%s].\n", g_cfg_info.cfg_version);
    } else if (!os_mem_cmp(g_cfg_info.cmd[index][pos].cmd_name,
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

    return SUCC;
}

/*
 * �� �� ��  : firmware_parse_cfg
 * ��������  : ����cfg�ļ����������Ľ��������g_st_cfg_infoȫ�ֱ�����
 * �������  : puc_cfg_info_buf: ������cfg�ļ����ݵ�buffer
 *             l_buf_len       : puc_cfg_info_buf�ĳ���
 *             ul_index        : ��������������������ֵ
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
STATIC int32_t firmware_parse_cfg(uint8_t *puc_cfg_info_buf, int32_t l_buf_len, uint32_t ul_index)
{
    int32_t i;
    int32_t l_len;
    uint8_t *flag = NULL;
    uint8_t *begin = NULL;
    uint8_t *end = NULL;
    int32_t cmd_type;
    uint8_t cmd_name[DOWNLOAD_CMD_LEN];
    uint8_t cmd_para[DOWNLOAD_CMD_PARA_LEN];

    g_cfg_info.cmd[ul_index] = (struct cmd_type_st *)malloc_cmd_buf(puc_cfg_info_buf, ul_index);
    if (g_cfg_info.cmd[ul_index] == NULL) {
        ps_print_err(" malloc_cmd_buf fail!\n");
        return -EFAIL;
    }

    /* ����CMD BUF */
    flag = puc_cfg_info_buf;
    l_len = l_buf_len;
    i = 0;
    while ((i < g_cfg_info.count[ul_index]) && (flag < &puc_cfg_info_buf[l_len])) {
        /*
         * ��ȡ�����ļ��е�һ��,�����ļ�������unix��ʽ.
         * �����ļ��е�ĳһ�к����ַ� @ ����Ϊ����Ϊע����
         */
        begin = flag;
        end = os_str_chr(flag, '\n');
        if (end == NULL) { /* �ļ������һ�У�û�л��з� */
            ps_print_dbg("lost of new line!\n");
            end = &puc_cfg_info_buf[l_len];
        } else if (end == begin) { /* ����ֻ��һ�����з� */
            ps_print_dbg("blank line\n");
            flag = end + 1;
            continue;
        }
        *end = '\0';

        ps_print_dbg("operation string is [%s]\n", begin);

        memset_s(cmd_name, DOWNLOAD_CMD_LEN, 0, DOWNLOAD_CMD_LEN);
        memset_s(cmd_para, DOWNLOAD_CMD_PARA_LEN, 0, DOWNLOAD_CMD_PARA_LEN);

        cmd_type = firmware_parse_cmd(begin, cmd_name, sizeof(cmd_name), cmd_para, sizeof(cmd_para));

        ps_print_dbg("cmd type=[%d],cmd_name=[%s],cmd_para=[%s]\n", cmd_type, cmd_name, cmd_para);

        if (cmd_type != ERROR_TYPE_CMD) { /* ��ȷ���������ͣ����� */
            g_cfg_info.cmd[ul_index][i].cmd_type = cmd_type;
            memcpy_s(g_cfg_info.cmd[ul_index][i].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN);
            memcpy_s(g_cfg_info.cmd[ul_index][i].cmd_para, DOWNLOAD_CMD_PARA_LEN, cmd_para, DOWNLOAD_CMD_PARA_LEN);

            firmware_parse_cur_cmd(ul_index, i);
            i++;
        }
        flag = end + 1;
    }

    /* ����ʵ������������޸����յ�������� */
    g_cfg_info.count[ul_index] = i;
    ps_print_info("effective cmd count: al_count[%d] = %d\n", ul_index, g_cfg_info.count[ul_index]);

    return SUCC;
}

/*
 * �� �� ��  : firmware_get_cfg
 * ��������  : ��ȡcfg�ļ����������������Ľ��������g_st_cfg_infoȫ�ֱ�����
 * �������  : puc_CfgPatch: cfg�ļ���·��
 *             ul_index     : ��������������������ֵ
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32_t firmware_get_cfg(uint8_t *cfg_patch, uint32_t idx)
{
    uint8_t *read_cfg_buf = NULL;
    int32_t readlen;
    int32_t ret;

    if (cfg_patch == NULL) {
        ps_print_err("cfg file path is null!\n");
        return -EFAIL;
    }

    /* cfg�ļ��޶���С��2048,���cfg�ļ��Ĵ�Сȷʵ����2048�������޸�READ_CFG_BUF_LEN��ֵ */
    read_cfg_buf = os_kmalloc_gfp(READ_CFG_BUF_LEN);
    if (read_cfg_buf == NULL) {
        ps_print_err("kmalloc READ_CFG_BUF fail!\n");
        return -EFAIL;
    }

    readlen = firmware_read_cfg(cfg_patch, read_cfg_buf, READ_CFG_BUF_LEN);
    if (readlen < 0) {
        ps_print_err("read cfg error!\n");
        os_mem_kfree(read_cfg_buf);
        read_cfg_buf = NULL;
        return -EFAIL;
    } else if (readlen > READ_CFG_BUF_LEN - 1) {
        /* ��1��Ϊ��ȷ��cfg�ļ��ĳ��Ȳ�����READ_CFG_BUF_LEN����Ϊfirmware_read_cfg���ֻ���ȡREAD_CFG_BUF_LEN���ȵ����� */
        ps_print_err("cfg file [%s] larger than %d\n", cfg_patch, READ_CFG_BUF_LEN);
        os_mem_kfree(read_cfg_buf);
        read_cfg_buf = NULL;
        return -EFAIL;
    } else {
        ps_print_dbg("read cfg file [%s] ok, size is [%d]\n", cfg_patch, readlen);
    }

    ret = firmware_parse_cfg(read_cfg_buf, readlen, idx);
    if (ret < 0) {
        ps_print_err("parse cfg error!\n");
    }

    os_mem_kfree(read_cfg_buf);
    read_cfg_buf = NULL;

    return ret;
}

STATIC int32_t firmware_download_buffer_alloc(void)
{
    hcc_bus *bus = NULL;
    STATIC uint64_t max_cost_time = 0;
    declare_time_cost_stru(cost);

    bus = hcc_get_current_110x_bus();
    if (bus == NULL) {
        ps_print_err("firmware curr bus is null\n");
        return -EFAIL;
    }

    oal_get_time_cost_start(cost);
    g_firmware_down_buf = (uint8_t *)oal_memtry_alloc(oal_min(bus->cap.max_trans_size, MAX_FIRMWARE_FILE_TX_BUF_LEN),
                                                      MIN_FIRMWARE_FILE_TX_BUF_LEN, &g_firmware_down_buf_len);
    g_firmware_down_buf_len = OAL_ROUND_DOWN(g_firmware_down_buf_len, 8); /* �����3bit����֤8�ֽڶ��� */
    if (g_firmware_down_buf == NULL || (g_firmware_down_buf_len == 0)) {
        ps_print_err("g_pucDataBuf KMALLOC failed, min request:%u\n", MIN_FIRMWARE_FILE_TX_BUF_LEN);
        return -ENOMEM;
    }

    oal_get_time_cost_end(cost);
    oal_calc_time_cost_sub(cost);
    if (time_cost_var_sub(cost) > max_cost_time) {
        max_cost_time = time_cost_var_sub(cost);
    }

    ps_print_info("download firmware file buf len is [%d], cost time %llu, max cost time %llu\n",
                  g_firmware_down_buf_len, time_cost_var_sub(cost), max_cost_time);
    return SUCC;
}

void firmware_uart_rate_set(void)
{
    int32_t ret;
    uint8_t cmd_name[DOWNLOAD_CMD_LEN] = { 0 };
    uint8_t cmd_para[DOWNLOAD_CMD_PARA_LEN] = { 0 };

    ret = get_cust_conf_string(INI_MODU_PLAT, "uart_rate", cmd_para, sizeof(cmd_para) - 1);
    if (ret != INI_SUCC) {
        ps_print_err("can't found uart rate ini");
        return;
    }

    ret = memcpy_s(cmd_name, DOWNLOAD_CMD_LEN, "WRITEM", 6); /* ��Ҫ�����ĸ���Ϊ6 */
    if (ret != EOK) {
        ps_print_err("uart rate cmd name set error");
        return;
    }

    ps_print_dbg("cmd_type[%d], name[%s], para[%s]\n", NUM_TYPE_CMD, cmd_name, cmd_para);

    ret = execute_download_cmd(NUM_TYPE_CMD, cmd_name, cmd_para);
    if (ret != SUCC) {
        ps_print_err("cmd_type[%d], name[%s], para[%s]\n", NUM_TYPE_CMD, cmd_name, cmd_para);
    }
}

/*
 * �� �� ��  : firmware_download
 * ��������  : firmware����
 * �������  : idx: ��Ч�����������������
 * �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
 */
int32_t firmware_download(uint32_t idx)
{
    int32_t ret;
    int32_t i;
    int32_t cmd_type;
    uint8_t *cmd_name = NULL;
    uint8_t *cmd_para = NULL;

    if (idx >= CFG_FILE_TOTAL) {
        ps_print_err("idx [%d] is error!\n", idx);
        return -EFAIL;
    }

    ps_print_info("start download firmware, idx = [%d]\n", idx);

    if (g_cfg_info.count[idx] == 0) {
        ps_print_err("firmware download cmd count is 0, idx = [%d]\n", idx);
        return -EFAIL;
    }

    if (firmware_download_buffer_alloc() < 0) {
        return -ENOMEM;
    }

    firmware_uart_rate_set();

    for (i = 0; i < g_cfg_info.count[idx]; i++) {
        cmd_type = g_cfg_info.cmd[idx][i].cmd_type;
        cmd_name = g_cfg_info.cmd[idx][i].cmd_name;
        cmd_para = g_cfg_info.cmd[idx][i].cmd_para;

        ps_print_dbg("cmd[%d]:type[%d], name[%s], para[%s]\n", i, cmd_type, cmd_name, cmd_para);

        ps_print_dbg("firmware down start cmd[%d]:type[%d], name[%s]\n", i, cmd_type, cmd_name);

        ret = execute_download_cmd(cmd_type, cmd_name, cmd_para);
        if (ret < 0) {
            os_mem_kfree(g_firmware_down_buf);
            g_firmware_down_buf = NULL;
            ps_print_err("download firmware fail\n");

            return ret;
        }

        ps_print_dbg("firmware down finish cmd[%d]:type[%d], name[%s]\n", i, cmd_type, cmd_name);
    }

    os_mem_kfree(g_firmware_down_buf);
    g_firmware_down_buf = NULL;

    ps_print_info("finish download firmware\n");

    return SUCC;
}

STATIC int32_t print_firmware_download_cmd(uint32_t ul_index)
{
    int32_t i;
    int32_t l_cmd_type;
    uint8_t *puc_cmd_name = NULL;
    uint8_t *puc_cmd_para = NULL;
    uint32_t count;

    count = g_cfg_info.count[ul_index];
    ps_print_info("[%s] download cmd, total count is [%d]\n", g_cfg_path[ul_index], count);

    for (i = 0; i < count; i++) {
        l_cmd_type = g_cfg_info.cmd[ul_index][i].cmd_type;
        puc_cmd_name = g_cfg_info.cmd[ul_index][i].cmd_name;
        puc_cmd_para = g_cfg_info.cmd[ul_index][i].cmd_para;

        ps_print_info("cmd[%d]:type[%d], name[%s], para[%s]\n", i, l_cmd_type, puc_cmd_name, puc_cmd_para);
    }

    return 0;
}

int32_t print_cfg_file_cmd(void)
{
    int32_t i;

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
STATIC int32_t firmware_cfg_path_init(void)
{
    if (get_hi110x_subchip_type() == BOARD_VERSION_HI1103) {
        g_cfg_path = g_hi1103_pilot_cfg_patch_in_vendor;
        set_hi110x_asic_type(HI1103_ASIC_PILOT);
    } else if (get_hi110x_subchip_type() == BOARD_VERSION_HI1105) {
        if (hi110x_is_asic()) {
            g_cfg_path = g_hi1105_asic_cfg_patch_in_vendor;
            set_hi110x_asic_type(HI1105_ASIC);
        } else {
            g_cfg_path = g_hi1105_fpga_cfg_patch_in_vendor;
            set_hi110x_asic_type(HI1105_FPGA);
        }
    } else if (get_hi110x_subchip_type() == BOARD_VERSION_HI1106) {
        if (hi110x_is_asic()) {
            g_cfg_path = g_hi1106_asic_cfg_patch_in_vendor;
            set_hi110x_asic_type(HI1106_ASIC);
        } else {
            g_cfg_path = g_hi1106_fpga_cfg_patch_in_vendor;
            set_hi110x_asic_type(HI1106_FPGA);
        }
    }

    return SUCC;
}

/*
 * �� �� ��  : firmware_cfg_init
 * ��������  : firmware���ص�cfg�ļ���ʼ������ȡ������cfg�ļ����������Ľ��������
 *             cfg_infoȫ�ֱ�����
 */
int32_t firmware_cfg_init(void)
{
    int32_t ret;
    uint32_t i;

    ret = firmware_cfg_path_init();
    if (ret != SUCC) {
        ps_print_err("firmware cfg path init fail!");
        return -EFAIL;
    }

     /* �������ڱ��湲ʱ��У׼���ݵ�buffer */
    ret = dcxo_data_buf_malloc();
    if (ret < 0) {
        ps_print_err("alloc dcxo data buf fail\n");
        return -EFAIL;
    }

    /* ����cfg�ļ� */
    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        ret = firmware_get_cfg(g_cfg_path[i], i);
        if (ret < 0) {
            if (i == RAM_REG_TEST_CFG) {
                ps_print_warning("ram_reg_test_cfg maybe not exist, please check\n");
                continue;
            }

            ps_print_err("get cfg file [%s] fail\n", g_cfg_path[i]);
            goto cfg_file_init_fail;
        }
    }

    return SUCC;

cfg_file_init_fail:
    firmware_cfg_clear();

    return -EFAIL;
}

/*
 * �� �� ��  : firmware_cfg_clear
 * ��������  : �ͷ�firmware_cfg_initʱ������ڴ�
 * �� �� ֵ  : ���Ƿ���0����ʾ�ɹ�
 */
int32_t firmware_cfg_clear(void)
{
    int32_t i;

    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        g_cfg_info.count[i] = 0;
        if (g_cfg_info.cmd[i] != NULL) {
            os_mem_kfree(g_cfg_info.cmd[i]);
            g_cfg_info.cmd[i] = NULL;
        }
    }

    dcxo_data_buf_free();

    return SUCC;
}

#define TSENSOR_VALID_HIGHEST_TEMP 125
#define TSENSOR_VALID_LOWEST_TEMP  (-40)

#define DEVICE_MEM_CHECK_SUCC 0x000f
#define RESULT_DETAIL_REG     "0x5000001c,4"
#define RESULT_TIME_LOW_REG   "0x50000010,4"
#define RESULT_TIME_HIGH_REG  "0x50000014,4"
#define RESULT_TSENSOR_C0_REG "0x5000348c,4"
#define RESULT_TSENSOR_C1_REG "0x500034cc,4"
#define GET_MEM_CHECK_FLAG    "0x50000018,4"
// 06�Ĵ���
#define RESULT_DETAIL_REG06     "0x4000140c,4"
#define RESULT_TIME_LOW_REG06   "0x40001400,4"
#define RESULT_TIME_HIGH_REG06  "0x40001404,4"
#define RESULT_TSENSOR_C0_REG06 "0x4000e48c,4"
#define RESULT_TSENSOR_C1_REG06 "0x4000e4cc,4"
#define GET_MEM_CHECK_FLAG06    "0x40001408,4"

#define PRO_RAM_TEST_CASE_NONE                                   0x0
#define PRO_RAM_TEST_CASE_TCM_SCAN                               0x1
#define PRO_RAM_TEST_CASE_RAM_ALL_SCAN                           0x2
#define PRO_RAM_TEST_CASE_REG_SCAN                               0x3
#define PRO_RAM_TEST_CASE_BT_EM_SCAN                             0x4
#define PRO_RAM_TEST_CASE_MBIST                                  0x5
#define PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_MAJORITY_FULL_TEST_NEW 0x6
#define PRO_RAM_TEST_CASE_MBIST_CLDO2_FULL_TEST_NEW              0x7
#define PRO_RAM_TEST_CASE_MBIST_CLDO1_OTHER_FULL_TEST_NEW        0x8
#define PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_320_FULL_TEST_NEW      0x9
#define PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_MAJORITY_TEST_NEW      0xa

#define PRO_DETAIL_RESULT_NOT_START                   0x0000
#define PRO_DETAIL_RESULT_SUCC                        0xf000
#define PRO_DETAIL_RESULT_RUNING                      0xffff
#define PRO_DETAIL_RESULT_CASE_0                      0x1
#define PRO_DETAIL_RESULT_CASE_1                      0x2
#define PRO_DETAIL_RESULT_CASE_2_1                    0x3
#define PRO_DETAIL_RESULT_CASE_2_2                    0x4
#define PRO_DETAIL_RESULT_CASE_3                      0x5
#define pro_detail_error_result(main_case, test_case) (0xf000 | ((main_case) << 8) | ((test_case)&0xff))
#define pro_get_detail_error_result_main(error)       (((error) >> 8) & 0xf)
#define pro_get_detail_error_result_sub(error)        (((error) >> 0) & 0xff)

// ������
enum product_test_result_enum {
    RESULT_DETAIL = 0, /* �������������� */
    RESULT_TIME_LOW,   /* ��������ʱ���16bit���� */
    RESULT_TIME_HIGH,  /* ��������ʱ���16bit���� */
    RESULT_TSENSOR_C0, /* ����TSENSOR_C0������� */
    RESULT_TSENSOR_C1, /* ����TSENSOR_C1������� */
    MEM_CHECK_FLAG, /* ����GET_MEM_CHECK_FLAG ���� */
    RESULT_TOTAL
};

STATIC uint8_t *g_hi1106_product_test_result[RESULT_TOTAL] = {
    RESULT_DETAIL_REG06,
    RESULT_TIME_LOW_REG06,
    RESULT_TIME_HIGH_REG06,
    RESULT_TSENSOR_C0_REG06,
    RESULT_TSENSOR_C1_REG06,
    GET_MEM_CHECK_FLAG06,
};

// 03&&05 �Ĵ�����ַһ��
STATIC uint8_t *g_hi110x_product_test_result[RESULT_TOTAL] = {
    RESULT_DETAIL_REG,
    RESULT_TIME_LOW_REG,
    RESULT_TIME_HIGH_REG,
    RESULT_TSENSOR_C0_REG,
    RESULT_TSENSOR_C1_REG,
    GET_MEM_CHECK_FLAG,
};

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tsensor_c0_auto_clr : 1;   /* [0]  */
        unsigned int tsensor_c0_rdy_auto : 1;   /* [1]  */
        unsigned int tsensor_c0_data_auto : 10; /* [11..2]  */
        unsigned int reserved : 4;              /* [15..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} tensor_auto;

typedef struct _mem_test_type_str_ {
    uint32_t id;
    char *name;
} mem_test_type_str;

STATIC mem_test_type_str g_mem_test_main_type[] = {
    { PRO_DETAIL_RESULT_CASE_0,   "SYSTEM_INIT" },
    { PRO_DETAIL_RESULT_CASE_1,   "CASE_1" },
    { PRO_DETAIL_RESULT_CASE_2_1, "CASE_2_1" },
    { PRO_DETAIL_RESULT_CASE_2_2, "CASE_2_2" },
    { PRO_DETAIL_RESULT_CASE_3,   "CASE_3" },
};

STATIC mem_test_type_str g_mem_test_sub_type[] = {
    { PRO_RAM_TEST_CASE_NONE,                                   "NOT_OVER" },
    { PRO_RAM_TEST_CASE_TCM_SCAN,                               "TCM_SCAN" },
    { PRO_RAM_TEST_CASE_RAM_ALL_SCAN,                           "RAM_ALL_SCAN" },
    { PRO_RAM_TEST_CASE_REG_SCAN,                               "REG_SCAN" },
    { PRO_RAM_TEST_CASE_BT_EM_SCAN,                             "BT_EM_SCAN" },
    { PRO_RAM_TEST_CASE_MBIST,                                  "MBIST_INIT" },
    { PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_MAJORITY_FULL_TEST_NEW, "CLDO1_WL0_MAJORITY_FULL_TEST_NEW" },
    { PRO_RAM_TEST_CASE_MBIST_CLDO2_FULL_TEST_NEW,              "CLDO2_FULL_TEST_NEW" },
    { PRO_RAM_TEST_CASE_MBIST_CLDO1_OTHER_FULL_TEST_NEW,        "CLDO1_OTHER_FULL_TEST_NEW" },
    { PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_320_FULL_TEST_NEW,      "CLDO1_WL0_320_FULL_TEST_NEW" },
    { PRO_RAM_TEST_CASE_MBIST_CLDO1_WL0_MAJORITY_TEST_NEW,      "CLDO1_WL0_MAJORITY_TEST_NEW" },
};

STATIC char *get_memcheck_error_main_string(uint32_t result)
{
    uint16_t main_type;
    uint32_t i;
    uint32_t num = sizeof(g_mem_test_main_type) / sizeof(mem_test_type_str);
    main_type = pro_get_detail_error_result_main(result);

    for (i = 0; i < num; i++) {
        if (main_type == g_mem_test_main_type[i].id) {
            return g_mem_test_main_type[i].name;
        }
    }

    return NULL;
}

STATIC char *get_memcheck_error_sub_string(uint32_t result)
{
    uint16_t sub_type;
    uint32_t i;
    uint32_t num = sizeof(g_mem_test_sub_type) / sizeof(mem_test_type_str);
    sub_type = pro_get_detail_error_result_sub(result);
    for (i = 0; i < num; i++) {
        if (sub_type == g_mem_test_sub_type[i].id) {
            return g_mem_test_sub_type[i].name;
        }
    }

    return NULL;
}

int32_t is_device_mem_test_succ(void)
{
    int32_t ret;
    int32_t test_flag = 0;
    uint8_t **result_reg = NULL;

    if (get_hi110x_subchip_type() == BOARD_VERSION_HI1106) {
        result_reg = g_hi1106_product_test_result;
    } else {
        result_reg = g_hi110x_product_test_result;
    }

    ret = number_type_cmd_send(RMEM_CMD_KEYWORD, result_reg[MEM_CHECK_FLAG]);
    if (ret < 0) {
        ps_print_warning("send cmd %s:%s fail,ret = %d\n", RMEM_CMD_KEYWORD, result_reg[MEM_CHECK_FLAG], ret);
        return -1;
    }

    ret = read_msg((uint8_t *)&test_flag, sizeof(test_flag));
    if (ret < 0) {
        ps_print_warning("read device test flag fail, read_len = %d, return = %d\n", (int32_t)sizeof(test_flag), ret);
        return -1;
    }
    ps_print_warning("get device test flag:0x%x\n", test_flag);
    if (test_flag == DEVICE_MEM_CHECK_SUCC) {
        return 0;
    }
    return -1;
}

STATIC void print_device_ram_test_detail_result(int32_t is_wcpu, uint32_t result)
{
    char *case_name = NULL;
    char *main_str = NULL;
    char *sub_str = NULL;
    if (is_wcpu == true) {
        case_name = "[wcpu_memcheck]";
    } else {
        case_name = "[bcpu_memcheck]";
    }
    ps_print_warning("%s detail result=0x%x\n", case_name, result);
    if (result == PRO_DETAIL_RESULT_NOT_START) {
        ps_print_warning("%s didn't start run\n", case_name);
        return;
    } else if (result == PRO_DETAIL_RESULT_SUCC) {
        ps_print_warning("%s run succ\n", case_name);
        return;
    } else if (result == PRO_DETAIL_RESULT_RUNING) {
        ps_print_warning("%s still running\n", case_name);
        return;
    }

    main_str = get_memcheck_error_main_string(result);
    sub_str = get_memcheck_error_sub_string(result);

    ps_print_err("%s error found [%s:%s]\n",
                 case_name,
                 (main_str == NULL) ? "unkown" : main_str,
                 (sub_str == NULL) ? "unkown" : sub_str);
}

STATIC int32_t number_cmd_send_and_read(const char *key, const char *value, uint32_t *result)
{
    int32_t ret;
    ret = number_type_cmd_send(key, value);
    if (ret < 0) {
        ps_print_warning("send cmd %s:%s fail,ret = %d\n", key, value, ret);
        return -OAL_EFAIL;
    }

    ret = read_msg((uint8_t *)result, sizeof(uint32_t));
    if (ret < 0) {
        ps_print_warning("send cmd %s:%s read result failed,ret = %d\n", key, value, ret);
        return -OAL_EFAIL;
    }
    return 0;
}

STATIC void tsensor_check(int32_t sensor_num, uint16_t reg_tsensor)
{
    tensor_auto *tsensor_auto_sts = (tensor_auto *)&reg_tsensor;
    uint16_t temp_data = tsensor_auto_sts->bits.tsensor_c0_data_auto;
    int32_t temp_val = ((((int32_t)temp_data - 118) * 165) / 815) - 40; /* �¶���ת����ʽl_val=(us-118)*165)/815)-40 */
    ps_print_info("[memcheck]TsensorC%d, val 0x%x data,%u rdy,%u temp %d\n", sensor_num,
                  reg_tsensor, tsensor_auto_sts->bits.tsensor_c0_data_auto,
                  tsensor_auto_sts->bits.tsensor_c0_rdy_auto, temp_val);
    if ((tsensor_auto_sts->bits.tsensor_c0_rdy_auto) &&
        ((temp_val < TSENSOR_VALID_LOWEST_TEMP) || (temp_val > TSENSOR_VALID_HIGHEST_TEMP))) {
        ps_print_info("[memcheck]TsensorC%d, invalid", sensor_num);
    }
}

void get_device_ram_test_result(int32_t is_wcpu, uint32_t *cost)
{
    uint32_t time_cost;
    uint32_t result = 0;
    uint8_t **result_reg = NULL;
    hcc_bus *pst_bus = hcc_get_current_110x_bus();
    if (pst_bus == NULL) {
        ps_print_err("pst_bus is null");
        return;
    }

    if (pst_bus->bus_type != HCC_BUS_PCIE) {
        if (is_wcpu != true) {
            /* bcpu ����wmbist �ᵼ��sdio�ӿ��޷��ض������ֳ������ض� */
            ps_print_err("bcpu ram test can't read detail result");
            return;
        }
    }

    if (get_hi110x_subchip_type() == BOARD_VERSION_HI1106) {
        result_reg = g_hi1106_product_test_result;
    } else {
        result_reg = g_hi110x_product_test_result;
    }
    /* ʧ�ܺ��ȡ��ϸ�Ľ�� */
    if (number_cmd_send_and_read(RMEM_CMD_KEYWORD, result_reg[RESULT_DETAIL], &result) < 0) {
        return;
    }

    print_device_ram_test_detail_result(is_wcpu, result);

    if (number_cmd_send_and_read(RMEM_CMD_KEYWORD, result_reg[RESULT_TIME_LOW], &result) < 0) {
        return;
    }
    time_cost = (result & 0xffff);

    if (number_cmd_send_and_read(RMEM_CMD_KEYWORD, result_reg[RESULT_TIME_HIGH], &result) < 0) {
        return;
    }
    time_cost |= ((result & 0xffff) << 16); /* ����16λ */
    ps_print_warning("%s_ram_test_time_cost tick:%u  %u us\n",
                     (is_wcpu == true) ? "wcpu" : "bcpu", time_cost, time_cost * 31); /* time from 31+1 k */
    *cost = time_cost * 31; /* time_cost * 31 */

    /* tsensor read */
    if (g_ram_test_detail_tsensor_dump) {
        uint32_t tsensor_c0 = 0;
        uint32_t tsensor_c1 = 0;

        if (number_cmd_send_and_read(RMEM_CMD_KEYWORD, result_reg[RESULT_TSENSOR_C0], &tsensor_c0) < 0) {
            return;
        }

        if (number_cmd_send_and_read(RMEM_CMD_KEYWORD, result_reg[RESULT_TSENSOR_C1], &tsensor_c1) < 0) {
            return;
        }

        tsensor_check(0, tsensor_c0); // tsensor 0
        tsensor_check(1, tsensor_c1); // tsensor 1
    }
}

int32_t get_device_test_mem(bool is_wifi)
{
    wlan_memdump_t *wlan_memdump_s = NULL;
    int32_t ret;
    char buff[DEVICE_MEM_DUMP_MAX_BUFF_SIZE];

    wlan_memdump_s = get_wlan_memdump_cfg();
    if (wlan_memdump_s == NULL) {
        ps_print_err("memdump cfg is NULL!\n");
        return -FAILURE;
    }
    ret = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "0x%x,%d", wlan_memdump_s->addr, wlan_memdump_s->len);
    if (ret < 0) {
        ps_print_err("log str format err line[%d]\n", __LINE__);
        return ret;
    }

    if (sdio_read_mem(RMEM_CMD_KEYWORD, buff, is_wifi) >= 0) {
        ps_print_warning("read device mem succ\n");
    } else {
        ps_print_warning("read device mem fail\n");
        return -FAILURE;
    }
    return 0;
}
