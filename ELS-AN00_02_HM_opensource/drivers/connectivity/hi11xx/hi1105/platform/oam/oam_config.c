

/* ͷ�ļ����� */
#include "oam_config.h"
#include "oal_aes.h"

/* ȫ�ֱ������� */
OAL_STATIC oam_customize_stru g_oam_customize;

/* ����ʵ�� */
void oam_register_init_hook(oam_msg_moduleid_enum_uint8 en_moduleid, p_oam_customize_init_func p_func)
{
    g_oam_customize.customize_init[en_moduleid] = p_func;
}

/*
 * �� �� ��  : oam_cfg_get_one_item
 * ��������  : ����һ���������ֵ��OAM�ڲ��ṹ��
 * �������  : pc_cfg_data_buf:�������ļ��л�ȡ�����ݣ����浽��buf��
 *             pc_section     :�����������ڵ�section
 *             pc_key         :�������������ļ��ж�Ӧ���ַ���
 *             pl_val         :�������ֵ
 */
int32_t oam_cfg_get_one_item(int8_t *pc_cfg_data_buf,
                             int8_t *pc_section,
                             int8_t *pc_key,
                             int32_t *pl_val)
{
    int8_t *pc_section_addr = NULL;
    int8_t *pc_key_addr = NULL;
    int8_t *pc_val_addr = NULL;
    int8_t *pc_equal_sign_addr = NULL; /* �Ⱥŵĵ�ַ */
    int8_t *pc_tmp = NULL;
    uint8_t uc_key_len;
    int8_t ac_val[OAM_CFG_VAL_MAX_LEN] = {0}; /* �ݴ��������ֵ */
    uint8_t uc_index = 0;

    /* ����section�������ļ��е�λ�� */
    pc_section_addr = oal_strstr(pc_cfg_data_buf, pc_section);
    if (pc_section_addr == NULL) {
        oal_io_print("oam_cfg_get_one_item::section not found!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��section����λ�ÿ�ʼ�����������Ӧ���ַ��� */
    pc_key_addr = oal_strstr(pc_section_addr, pc_key);
    if (pc_key_addr == NULL) {
        oal_io_print("oam_cfg_get_one_item::key not found!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* �����������ֵ */
    uc_key_len = (uint8_t)OAL_STRLEN(pc_key);

    /*
     * ���key�����Ƿ������'=',������ǵĻ����ǵ�ǰҪ���ҵ�key�п�������һ��
     * key��ǰ׺����Ҫ�����������
     */
    pc_equal_sign_addr = pc_key_addr + uc_key_len;
    while (*(pc_equal_sign_addr) != '=') {
        pc_key_addr = oal_strstr(pc_equal_sign_addr, pc_key);
        if (pc_key_addr == NULL) {
            oal_io_print("oam_cfg_get_one_item::key not found!\n");
            return OAL_ERR_CODE_PTR_NULL;
        }

        pc_equal_sign_addr = pc_key_addr + uc_key_len;
    }

    /* ���val�Ƿ���� */
    pc_val_addr = pc_equal_sign_addr + OAM_CFG_EQUAL_SIGN_LEN;
    if ((*(pc_val_addr) == '\n') || (*(pc_val_addr) == '\0')) {
        return OAL_FAIL;
    }

    for (pc_tmp = pc_val_addr; (*pc_tmp != '\n') && (*pc_tmp != '\0'); pc_tmp++) {
        ac_val[uc_index] = *pc_tmp;
        uc_index++;
    }

    *pl_val = oal_atoi(ac_val);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_cfg_read_file_to_buf
 * ��������  : �������ļ��ж�ȡ�������ݣ����浽һ��buf��
 * �������  : pc_cfg_data_buf:���ļ��������ݺ��ŵ�buf
 *             ul_file_size   :�ļ���С(�ֽ���)
 */
int32_t oam_cfg_read_file_to_buf(int8_t *pc_cfg_data_buf, uint32_t ul_file_size)
{
    oal_file_stru *p_file;
    int32_t l_ret;

    p_file = oal_file_open_readonly(OAL_CFG_FILE_PATH);
    if (p_file == NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    l_ret = oal_file_read(p_file, pc_cfg_data_buf, ul_file_size);
    if (l_ret <= 0) {
        oal_file_close(p_file);
        return OAL_FAIL;
    }

    oal_file_close(p_file);

    return OAL_SUCC;
}

/*
 * �� �� ��  : oam_cfg_decrypt_all_item
 * ��������  : ���������ļ��ж������ļ������ݽ���
 * �������  : pst_aes_key   :aes��Կ
 *             puc_ciphertext:����
 *             puc_plaintext :����
 *             ul_cipher_len :���ĳ���
 */
uint32_t oam_cfg_decrypt_all_item(oal_aes_key_stru *pst_aes_key,
                                  int8_t *pc_ciphertext,
                                  int8_t *pc_plaintext,
                                  uint32_t ul_cipher_len)
{
    uint32_t ul_loop = 0;
    uint32_t ul_round;
    uint8_t *puc_cipher_tmp = NULL;
    uint8_t *puc_plain_tmp = NULL;

    /* AES���ܿ�Ĵ�С��16�ֽڣ�������ĳ��Ȳ���16�ı�����������ȷ���� */
    if ((ul_cipher_len % OAL_AES_BLOCK_SIZE) != 0) {
        oal_io_print("oam_cfg_decrypt_all_item::ciphertext length invalid!\n");
        return OAL_FAIL;
    }

    if (ul_cipher_len == 0) {
        oal_io_print("oam_cfg_decrypt_all_item::ciphertext length is 0!\n");
        return OAL_FAIL;
    }

    ul_round = (ul_cipher_len >> 4); /* AES���ܿ�Ĵ�С��16�ֽڣ����������AES���ܿ������ul_cipher_len����4 */
    puc_cipher_tmp = (uint8_t *)pc_ciphertext;
    puc_plain_tmp = (uint8_t *)pc_plaintext;

    while (ul_loop < ul_round) {
        oal_aes_decrypt(pst_aes_key, puc_plain_tmp, puc_cipher_tmp);

        ul_loop++;
        puc_cipher_tmp += OAL_AES_BLOCK_SIZE;
        puc_plain_tmp += OAL_AES_BLOCK_SIZE;
    }

    return OAL_SUCC;
}

/*lint -e19*/
oal_module_symbol(oam_register_init_hook);
oal_module_symbol(oam_cfg_get_one_item);
oal_module_symbol(oam_cfg_read_file_to_buf);
oal_module_symbol(oam_cfg_decrypt_all_item);
