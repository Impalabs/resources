/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "at_cmd_proc.h"
#include "securec.h"
#include "at_check_func.h"
#include "ppp_interface.h"
#include "taf_mmi_str_parse.h"
#include "app_vc_api.h"
#include "at_data_proc.h"
#include "at_cmd_msg_proc.h"
#include "taf_std_lib.h"
#include "mn_comm_api.h"
#include "at_event_report.h"

#include "at_common.h"

#include "at_mdrv_interface.h" /* AT_SysbootShutdown AT_GetTimerAccuracyTimestamp */

#include "at_ltev_taf_rslt_proc.h"

/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_AT_CMDPROC_C

#define AT_VALID_COMMA_NUM 2
#define AT_SIMLOCK_VALID_PARA_NUM 2


#define AT_SIMLOCK_PLMN_MAX_NUM 61
#define AT_CMDTMP_MAX_LEN 100

#define AT_BLANK_ASCII_VALUE 0x20

#define AT_CLCC_PARA_MODE_VOICE 0 /* ��������ģʽ */
#define AT_CLCC_PARA_MODE_DATA  1 /* ���ݺ���ģʽ */
#define AT_CLCC_PARA_MODE_FAX   2 /* ����ģʽ */

#define AT_SEND_DATA_BUFF_INDEX_1 1
#if (FEATURE_MBB_CUST == FEATURE_ON)
#define AT_CPCM_CODEC_CLOCK_MODE_MAX_NUM 10
/* ATָ��֧��5��ʱ������ */
const CodecClockMode g_codecClockMode[AT_CPCM_CODEC_CLOCK_MODE_MAX_NUM] = {
    { CPCM_CODEC_CLOCK_MODE_2M, APP_VC_CPCM_CODEC_CLOCK_2M },
    { CPCM_CODEC_CLOCK_MODE_1M, APP_VC_CPCM_CODEC_CLOCK_1M },
    { CPCM_CODEC_CLOCK_MODE_512k, APP_VC_CPCM_CODEC_CLOCK_512K },
    { CPCM_CODEC_CLOCK_MODE_256k, APP_VC_CPCM_CODEC_CLOCK_256K },
    { CPCM_CODEC_CLOCK_MODE_4M, APP_VC_CPCM_CODEC_CLOCK_4M }
};
#endif
/*
 * The following tables maps the 256 characters of PBM8 to the corresponding
 * unicode characters.
 */
const AT_PB_ConversionTable g_iraToUnicode[AT_PB_IRA_MAX_NUM] = {
    { 0x00, 0, 0x0000 }, /* Null */
    { 0x01, 0, 0x0001 }, /* Start of heading */
    { 0x02, 0, 0x0002 }, /* Start of text */
    { 0x03, 0, 0x0003 }, /* End of text */
    { 0x04, 0, 0x0004 }, /* End of transmission */
    { 0x05, 0, 0x0005 }, /* Inquiry */
    { 0x06, 0, 0x0006 }, /* ACK */
    { 0x07, 0, 0x0007 }, /* Bell */
    { 0x08, 0, 0x0008 }, /* Backspace */
    { 0x09, 0, 0x0009 }, /* Tab */
    { 0x0A, 0, 0x000A }, /* Line feed */
    { 0x0B, 0, 0x000B }, /* Vertical tab */
    { 0x0C, 0, 0x000C }, /* Form feed */
    { 0x0D, 0, 0x000D }, /* Carriage return */
    { 0x0E, 0, 0x000E }, /* Shift out */
    { 0x0F, 0, 0x000F }, /* Shift in */
    { 0x10, 0, 0x0010 }, /* Data link escape */
    { 0x11, 0, 0x0011 }, /* Device control one */
    { 0x12, 0, 0x0012 }, /* Device control two */
    { 0x13, 0, 0x0013 }, /* Device control three */
    { 0x14, 0, 0x0014 }, /* Device control four */
    { 0x15, 0, 0x0015 }, /* NAK */
    { 0x16, 0, 0x0016 }, /* Synch */
    { 0x17, 0, 0x0017 }, /* End of block */
    { 0x18, 0, 0x0018 }, /* Cancel */
    { 0x19, 0, 0x0019 }, /* End of medium */
    { 0x1A, 0, 0x001A }, /* Substitute */
    { 0x1B, 0, 0x001B }, /* ESC */
    { 0x1C, 0, 0x001C }, /* File separator */
    { 0x1D, 0, 0x001D }, /* Group separator */
    { 0x1E, 0, 0x001E }, /* Record separator */
    { 0x1F, 0, 0x001F }, /* Unit separator */
    { 0x20, 0, 0x0020 }, /* Space */
    { 0x21, 0, 0x0021 }, /* Exclamation mark */
    { 0x22, 0, 0x0022 }, /* Quotation mark */
    { 0x23, 0, 0x0023 }, /* Number sign */
    { 0x24, 0, 0x0024 }, /* Dollar sign */
    { 0x25, 0, 0x0025 }, /* Percent sign */
    { 0x26, 0, 0x0026 }, /* Ampersand */
    { 0x27, 0, 0x0027 }, /* Apostrophe */
    { 0x28, 0, 0x0028 }, /* Left parenthesis */
    { 0x29, 0, 0x0029 }, /* Right parenthesis */
    { 0x2A, 0, 0x002A }, /* Asterisk */
    { 0x2B, 0, 0x002B }, /* Plus */
    { 0x2C, 0, 0x002C }, /* Comma */
    { 0x2D, 0, 0x002D }, /* Hyphen */
    { 0x2E, 0, 0x002E }, /* Full stop */
    { 0x2F, 0, 0x002F }, /* Solidus */
    { 0x30, 0, 0x0030 }, /* Zero */
    { 0x31, 0, 0x0031 }, /* One */
    { 0x32, 0, 0x0032 }, /* Two */
    { 0x33, 0, 0x0033 }, /* Three */
    { 0x34, 0, 0x0034 }, /* Four */
    { 0x35, 0, 0x0035 }, /* Five */
    { 0x36, 0, 0x0036 }, /* Six */
    { 0x37, 0, 0x0037 }, /* Seven */
    { 0x38, 0, 0x0038 }, /* Eight */
    { 0x39, 0, 0x0039 }, /* Nine */
    { 0x3A, 0, 0x003A }, /* Colon */
    { 0x3B, 0, 0x003B }, /* Semicolon */
    { 0x3C, 0, 0x003C }, /* < (Less than) */
    { 0x3D, 0, 0x003D }, /* = Equals */
    { 0x3E, 0, 0x003E }, /* > greater than */
    { 0x3F, 0, 0x003F }, /* Question mark */
    { 0x40, 0, 0x0040 }, /* AT Sign */
    { 0x41, 0, 0x0041 }, /* Latin A */
    { 0x42, 0, 0x0042 }, /* Latin B */
    { 0x43, 0, 0x0043 }, /* Latin C */
    { 0x44, 0, 0x0044 }, /* Latin D */
    { 0x45, 0, 0x0045 }, /* Latin E */
    { 0x46, 0, 0x0046 }, /* Latin F */
    { 0x47, 0, 0x0047 }, /* Latin G */
    { 0x48, 0, 0x0048 }, /* Latin H */
    { 0x49, 0, 0x0049 }, /* Latin I */
    { 0x4A, 0, 0x004A }, /* Latin J */
    { 0x4B, 0, 0x004B }, /* Latin K */
    { 0x4C, 0, 0x004C }, /* Latin L */
    { 0x4D, 0, 0x004D }, /* Latin M */
    { 0x4E, 0, 0x004E }, /* Latin N */
    { 0x4F, 0, 0x004F }, /* Latin O */
    { 0x50, 0, 0x0050 }, /* Latin P */
    { 0x51, 0, 0x0051 }, /* Latin Q */
    { 0x52, 0, 0x0052 }, /* Latin R */
    { 0x53, 0, 0x0053 }, /* Latin S */
    { 0x54, 0, 0x0054 }, /* Latin T */
    { 0x55, 0, 0x0055 }, /* Latin U */
    { 0x56, 0, 0x0056 }, /* Latin V */
    { 0x57, 0, 0x0057 }, /* Latin W */
    { 0x58, 0, 0x0058 }, /* Latin X */
    { 0x59, 0, 0x0059 }, /* Latin Y */
    { 0x5A, 0, 0x005A }, /* Latin Z */
    { 0x5B, 0, 0x005B }, /* Left square bracket */
    { 0x5C, 0, 0x005C }, /* Reverse solidus */
    { 0x5D, 0, 0x005D }, /* Right square bracket */
    { 0x5E, 0, 0x005E }, /* Circumflex accent */
    { 0x5F, 0, 0x005F }, /* Low line */
    { 0x60, 0, 0x0060 }, /* Grave accent */
    { 0x61, 0, 0x0061 }, /* Latin a */
    { 0x62, 0, 0x0062 }, /* Latin b */
    { 0x63, 0, 0x0063 }, /* Latin c */
    { 0x64, 0, 0x0064 }, /* Latin d */
    { 0x65, 0, 0x0065 }, /* Latin e */
    { 0x66, 0, 0x0066 }, /* Latin f */
    { 0x67, 0, 0x0067 }, /* Latin g */
    { 0x68, 0, 0x0068 }, /* Latin h */
    { 0x69, 0, 0x0069 }, /* Latin i */
    { 0x6A, 0, 0x006A }, /* Latin j */
    { 0x6B, 0, 0x006B }, /* Latin k */
    { 0x6C, 0, 0x006C }, /* Latin l */
    { 0x6D, 0, 0x006D }, /* Latin m */
    { 0x6E, 0, 0x006E }, /* Latin n */
    { 0x6F, 0, 0x006F }, /* Latin o */
    { 0x70, 0, 0x0070 }, /* Latin p */
    { 0x71, 0, 0x0071 }, /* Latin q */
    { 0x72, 0, 0x0072 }, /* Latin r */
    { 0x73, 0, 0x0073 }, /* Latin s */
    { 0x74, 0, 0x0074 }, /* Latin t */
    { 0x75, 0, 0x0075 }, /* Latin u */
    { 0x76, 0, 0x0076 }, /* Latin v */
    { 0x77, 0, 0x0077 }, /* Latin w */
    { 0x78, 0, 0x0078 }, /* Latin x */
    { 0x79, 0, 0x0079 }, /* Latin y */
    { 0x7A, 0, 0x007A }, /* Latin z */
    { 0x7B, 0, 0x007B }, /* Left curly bracket */
    { 0x7C, 0, 0x007C }, /* Vertical line */
    { 0x7D, 0, 0x007D }, /* Right curly bracket */
    { 0x7E, 0, 0x007E }, /* Tilde */
    { 0x7F, 0, 0x007F }, /* DEL */
    { 0x80, 0, 0x0080 }, /* control character 0x80 */
    { 0x81, 0, 0x0081 }, /* control character 0x81 */
    { 0x82, 0, 0x0082 }, /* control character 0x82 */
    { 0x83, 0, 0x0083 }, /* control character 0x83 */
    { 0x84, 0, 0x0084 }, /* control character 0x84 */
    { 0x85, 0, 0x0085 }, /* control character 0x85 */
    { 0x86, 0, 0x0086 }, /* control character 0x86 */
    { 0x87, 0, 0x0087 }, /* control character 0x87 */
    { 0x88, 0, 0x0088 }, /* control character 0x88 */
    { 0x89, 0, 0x0089 }, /* control character 0x89 */
    { 0x8A, 0, 0x008A }, /* control character 0x8A */
    { 0x8B, 0, 0x008B }, /* control character 0x8B */
    { 0x8C, 0, 0x008C }, /* control character 0x8C */
    { 0x8D, 0, 0x008D }, /* control character 0x8D */
    { 0x8E, 0, 0x008E }, /* control character 0x8E */
    { 0x8F, 0, 0x008F }, /* control character 0x8F */
    { 0x90, 0, 0x0394 }, /* Greek capital letter delta */
    { 0x91, 0, 0x20AC }, /* Euro sign */
    { 0x92, 0, 0x03A6 }, /* Greek capital letter phi */
    { 0x93, 0, 0x0393 }, /* Greek capital letter gamma */
    { 0x94, 0, 0x039B }, /* Greek capital letter lamda */
    { 0x95, 0, 0x03A9 }, /* Greek capital letter omega */
    { 0x96, 0, 0x03A0 }, /* Greek capital letter pi */
    { 0x97, 0, 0x03A8 }, /* Greek capital letter psi */
    { 0x98, 0, 0x03A3 }, /* Greek capital letter sigma */
    { 0x99, 0, 0x0398 }, /* Greek capital letter theta */
    { 0x9A, 0, 0x039E }, /* Greek capital letter xi */
    { 0x9B, 0, 0x009B }, /* control character 0x9B */
    { 0x9C, 0, 0x009C }, /* control character 0x9C */
    { 0x9D, 0, 0x009D }, /* control character 0x9D */
    { 0x9E, 0, 0x009E }, /* control character 0x9E */
    { 0x9F, 0, 0x009F }, /* control character 0x9F */
    { 0xA0, 0, 0x00A0 }, /* Non-Block Space */
    { 0xA1, 0, 0x00A1 }, /* Inverted exclamation mark */
    { 0xA2, 0, 0x00A2 }, /* Cent sign */
    { 0xA3, 0, 0x00A3 }, /* Pound sign */
    { 0xA4, 0, 0x00A4 }, /* Currency sign */
    { 0xA5, 0, 0x00A5 }, /* Yen sign */
    { 0xA6, 0, 0x00A6 }, /* Broken Vertical bar */
    { 0xA7, 0, 0x00A7 }, /* Section sign */
    { 0xA8, 0, 0x00A8 }, /* Diaeresis */
    { 0xA9, 0, 0x00A9 }, /* Copyright sign */
    { 0xAA, 0, 0x00AA }, /* Feminine ordinal indicator */
    { 0xAB, 0, 0x00AB }, /* Left-pointing double angle quotation mark */
    { 0xAC, 0, 0x00AC }, /* Not sign */
    { 0xAD, 0, 0x00AD }, /* Soft hyphen */
    { 0xAE, 0, 0x00AE }, /* Registered sign */
    { 0xAF, 0, 0x00AF }, /* Macron */
    { 0xB0, 0, 0x00B0 }, /* Degree sign */
    { 0xB1, 0, 0x00B1 }, /* Plus-minus sign */
    { 0xB2, 0, 0x00B2 }, /* Superscript two */
    { 0xB3, 0, 0x00B3 }, /* Superscript three */
    { 0xB4, 0, 0x00B4 }, /* Acute accent */
    { 0xB5, 0, 0x00B5 }, /* Micro sign */
    { 0xB6, 0, 0x00B6 }, /* Pilcrow sign */
    { 0xB7, 0, 0x00B7 }, /* Middle dot */
    { 0xB8, 0, 0x00B8 }, /* Cedilla */
    { 0xB9, 0, 0x00B9 }, /* Superscript one */
    { 0xBA, 0, 0x00BA }, /* Masculine ordinal indicator */
    { 0xBB, 0, 0x00BB }, /* Right-pointing double angle quotation mark */
    { 0xBC, 0, 0x00BC }, /* Fraction one quarter */
    { 0xBD, 0, 0x00BD }, /* Fraction one half */
    { 0xBE, 0, 0x00BE }, /* Fraction three quarters */
    { 0xBF, 0, 0x00BF }, /* Inverted question mark */
    { 0xC0, 0, 0x00C0 }, /* Latin A With grave */
    { 0xC1, 0, 0x00C1 }, /* Latin A With acute */
    { 0xC2, 0, 0x00C2 }, /* Latin A With circumflex */
    { 0xC3, 0, 0x00C3 }, /* Latin A With tilde */
    { 0xC4, 0, 0x00C4 }, /* Latin A With diaeresis */
    { 0xC5, 0, 0x00C5 }, /* Latin A With ring above */
    { 0xC6, 0, 0x00C6 }, /* Latin AE */
    { 0xC7, 0, 0x00C7 }, /* Latin C with cedilla */
    { 0xC8, 0, 0x00C8 }, /* Latin E with grave */
    { 0xC9, 0, 0x00C9 }, /* Latin E with acute */
    { 0xCA, 0, 0x00CA }, /* Latin E with circumflex */
    { 0xCB, 0, 0x00CB }, /* Latin E with diaeresis */
    { 0xCC, 0, 0x00CC }, /* Latin I with grave */
    { 0xCD, 0, 0x00CD }, /* Latin I with acute */
    { 0xCE, 0, 0x00CE }, /* Latin I with circumflex */
    { 0xCF, 0, 0x00CF }, /* Latin I with diaeresis */
    { 0xD0, 0, 0x00D0 }, /* Latin CAPITAL LETTER ETH (Icelandic) */
    { 0xD1, 0, 0x00D1 }, /* Latin CAPITAL LETTER N WITH TILDE */
    { 0xD2, 0, 0x00D2 }, /* Latin CAPITAL LETTER O WITH GRAVE */
    { 0xD3, 0, 0x00D3 }, /* Latin CAPITAL LETTER O WITH ACUTE */
    { 0xD4, 0, 0x00D4 }, /* Latin CAPITAL LETTER O WITH CIRCUMFLEX */
    { 0xD5, 0, 0x00D5 }, /* Latin CAPITAL LETTER O WITH TILDE */
    { 0xD6, 0, 0x00D6 }, /* Latin CAPITAL LETTER O WITH DIAERESIS */
    { 0xD7, 0, 0x00D7 }, /* MULTIPLICATION SIGN */
    { 0xD8, 0, 0x00D8 }, /* Latin CAPITAL LETTER O WITH STROKE */
    { 0xD9, 0, 0x00D9 }, /* Latin CAPITAL LETTER U WITH GRAVE */
    { 0xDA, 0, 0x00DA }, /* Latin CAPITAL LETTER U WITH ACUTE */
    { 0xDB, 0, 0x00DB }, /* Latin CAPITAL LETTER U WITH CIRCUMFLEX */
    { 0xDC, 0, 0x00DC }, /* Latin CAPITAL LETTER U WITH DIAERESIS */
    { 0xDD, 0, 0x00DD }, /* Latin CAPITAL LETTER Y WITH ACUTE */
    { 0xDE, 0, 0x00DE }, /* Latin CAPITAL LETTER THORN (Icelandic) */
    { 0xDF, 0, 0x00DF }, /* Latin SHARP S (German) */
    { 0xE0, 0, 0x00E0 }, /* Latin A WITH GRAVE */
    { 0xE1, 0, 0x00E1 }, /* Latin A WITH ACUTE */
    { 0xE2, 0, 0x00E2 }, /* Latin A WITH CIRCUMFLEX */
    { 0xE3, 0, 0x00E3 }, /* Latin A WITH TILDE */
    { 0xE4, 0, 0x00E4 }, /* Latin A WITH DIAERESIS */
    { 0xE5, 0, 0x00E5 }, /* Latin A WITH RING ABOVE */
    { 0xE6, 0, 0x00E6 }, /* Latin AE */
    { 0xE7, 0, 0x00E7 }, /* Latin C WITH CEDILLA */
    { 0xE8, 0, 0x00E8 }, /* Latin E WITH GRAVE */
    { 0xE9, 0, 0x00E9 }, /* Latin E WITH ACUTE */
    { 0xEA, 0, 0x00EA }, /* Latin E WITH CIRCUMFLEX */
    { 0xEB, 0, 0x00EB }, /* Latin E WITH DIAERESIS */
    { 0xEC, 0, 0x00EC }, /* Latin I WITH GRAVE */
    { 0xED, 0, 0x00ED }, /* Latin I WITH ACUTE */
    { 0xEE, 0, 0x00EE }, /* Latin I WITH CIRCUMFLEX */
    { 0xEF, 0, 0x00EF }, /* Latin I WITH DIAERESIS */
    { 0xF0, 0, 0x00F0 }, /* Latin ETH (Icelandic) */
    { 0xF1, 0, 0x00F1 }, /* Latin N WITH TILDE */
    { 0xF2, 0, 0x00F2 }, /* Latin O WITH GRAVE */
    { 0xF3, 0, 0x00F3 }, /* Latin O WITH ACUTE */
    { 0xF4, 0, 0x00F4 }, /* Latin O WITH CIRCUMFLEX */
    { 0xF5, 0, 0x00F5 }, /* Latin O WITH TILDE */
    { 0xF6, 0, 0x00F6 }, /* Latin O WITH DIAERESIS */
    { 0xF7, 0, 0x00F7 }, /* DIVISION SIGN */
    { 0xF8, 0, 0x00F8 }, /* Latin O WITH STROKE */
    { 0xF9, 0, 0x00F9 }, /* Latin U WITH GRAVE */
    { 0xFA, 0, 0x00FA }, /* Latin U WITH ACUTE */
    { 0xFB, 0, 0x00FB }, /* Latin U WITH CIRCUMFLEX */
    { 0xFC, 0, 0x00FC }, /* Latin U WITH DIAERESIS */
    { 0xFD, 0, 0x00FD }, /* Latin Y WITH ACUTE */
    { 0xFE, 0, 0x00FE }, /* Latin THORN (Icelandic) */
    { 0xFF, 0, 0x00FF }, /* Latin Y WITH DIAERESIS */
};

/*
 * The following table maps the gsm7 alphabet to the corresponding unicode
 * characters. This table is exactly 128 entries large.
 */
const AT_PB_ConversionTable g_gsmToUnicode[AT_PB_GSM_MAX_NUM] = {
    { 0x00, 0, 0x0040 }, /* COMMERCIAL AT */
    { 0x01, 0, 0x00A3 }, /* POUND SIGN */
    { 0x02, 0, 0x0024 }, /* DOLLAR SIGN */
    { 0x03, 0, 0x00A5 }, /* YEN SIGN */
    { 0x04, 0, 0x00E8 }, /* Latin E WITH GRAVE */
    { 0x05, 0, 0x00E9 }, /* Latin E WITH ACUTE */
    { 0x06, 0, 0x00F9 }, /* Latin U WITH GRAVE */
    { 0x07, 0, 0x00EC }, /* Latin I WITH GRAVE */
    { 0x08, 0, 0x00F2 }, /* Latin O WITH GRAVE */
    { 0x09, 0, 0x00E7 }, /* Latin C WITH CEDILLA */
    { 0x0A, 0, 0x000A }, /* LINE FEED */
    { 0x0B, 0, 0x00D8 }, /* Latin CAPITAL LETTER O WITH STROKE */
    { 0x0C, 0, 0x00F8 }, /* Latin O WITH STROKE */
    { 0x0D, 0, 0x000D }, /* CARRIAGE RETURN */
    { 0x0E, 0, 0x00C5 }, /* Latin CAPITAL LETTER A WITH RING ABOVE */
    { 0x0F, 0, 0x00E5 }, /* Latin A WITH RING ABOVE */
    { 0x10, 0, 0x0394 }, /* GREEK CAPITAL LETTER DELTA */
    { 0x11, 0, 0x005F }, /* LOW LINE */
    { 0x12, 0, 0x03A6 }, /* GREEK CAPITAL LETTER PHI */
    { 0x13, 0, 0x0393 }, /* GREEK CAPITAL LETTER GAMMA */
    { 0x14, 0, 0x039B }, /* GREEK CAPITAL LETTER LAMDA */
    { 0x15, 0, 0x03A9 }, /* GREEK CAPITAL LETTER OMEGA */
    { 0x16, 0, 0x03A0 }, /* GREEK CAPITAL LETTER PI */
    { 0x17, 0, 0x03A8 }, /* GREEK CAPITAL LETTER PSI */
    { 0x18, 0, 0x03A3 }, /* GREEK CAPITAL LETTER SIGMA */
    { 0x19, 0, 0x0398 }, /* GREEK CAPITAL LETTER THETA */
    { 0x1A, 0, 0x039E }, /* GREEK CAPITAL LETTER XI */
    { 0x1B, 0, 0x00A0 }, /* ESCAPE TO EXTENSION TABLE */
    { 0x1C, 0, 0x00C6 }, /* Latin CAPITAL LETTER AE */
    { 0x1D, 0, 0x00E6 }, /* Latin AE */
    { 0x1E, 0, 0x00DF }, /* Latin SHARP S (German) */
    { 0x1F, 0, 0x00C9 }, /* Latin CAPITAL LETTER E WITH ACUTE */
    { 0x20, 0, 0x0020 }, /* SPACE */
    { 0x21, 0, 0x0021 }, /* EXCLAMATION MARK */
    { 0x22, 0, 0x0022 }, /* QUOTATION MARK */
    { 0x23, 0, 0x0023 }, /* NUMBER SIGN */
    { 0x24, 0, 0x00A4 }, /* CURRENCY SIGN */
    { 0x25, 0, 0x0025 }, /* PERCENT SIGN */
    { 0x26, 0, 0x0026 }, /* AMPERSAND */
    { 0x27, 0, 0x0027 }, /* APOSTROPHE */
    { 0x28, 0, 0x0028 }, /* LEFT PARENTHESIS */
    { 0x29, 0, 0x0029 }, /* RIGHT PARENTHESIS */
    { 0x2A, 0, 0x002A }, /* ASTERISK */
    { 0x2B, 0, 0x002B }, /* PLUS SIGN */
    { 0x2C, 0, 0x002C }, /* COMMA */
    { 0x2D, 0, 0x002D }, /* HYPHEN-MINUS */
    { 0x2E, 0, 0x002E }, /* FULL STOP */
    { 0x2F, 0, 0x002F }, /* SOLIDUS */
    { 0x30, 0, 0x0030 }, /* DIGIT ZERO */
    { 0x31, 0, 0x0031 }, /* DIGIT ONE */
    { 0x32, 0, 0x0032 }, /* DIGIT TWO */
    { 0x33, 0, 0x0033 }, /* DIGIT THREE */
    { 0x34, 0, 0x0034 }, /* DIGIT FOUR */
    { 0x35, 0, 0x0035 }, /* DIGIT FIVE */
    { 0x36, 0, 0x0036 }, /* DIGIT SIX */
    { 0x37, 0, 0x0037 }, /* DIGIT SEVEN */
    { 0x38, 0, 0x0038 }, /* DIGIT EIGHT */
    { 0x39, 0, 0x0039 }, /* DIGIT NINE */
    { 0x3A, 0, 0x003A }, /* COLON */
    { 0x3B, 0, 0x003B }, /* SEMICOLON */
    { 0x3C, 0, 0x003C }, /* LESS-THAN SIGN */
    { 0x3D, 0, 0x003D }, /* EQUALS SIGN */
    { 0x3E, 0, 0x003E }, /* GREATER-THAN SIGN */
    { 0x3F, 0, 0x003F }, /* QUESTION MARK */
    { 0x40, 0, 0x00A1 }, /* INVERTED EXCLAMATION MARK */
    { 0x41, 0, 0x0041 }, /* Latin CAPITAL LETTER A */
    { 0x42, 0, 0x0042 }, /* Latin CAPITAL LETTER B */
    { 0x43, 0, 0x0043 }, /* Latin CAPITAL LETTER C */
    { 0x44, 0, 0x0044 }, /* Latin CAPITAL LETTER D */
    { 0x45, 0, 0x0045 }, /* Latin CAPITAL LETTER E */
    { 0x46, 0, 0x0046 }, /* Latin CAPITAL LETTER F */
    { 0x47, 0, 0x0047 }, /* Latin CAPITAL LETTER G */
    { 0x48, 0, 0x0048 }, /* Latin CAPITAL LETTER H */
    { 0x49, 0, 0x0049 }, /* Latin CAPITAL LETTER I */
    { 0x4A, 0, 0x004A }, /* Latin CAPITAL LETTER J */
    { 0x4B, 0, 0x004B }, /* Latin CAPITAL LETTER K */
    { 0x4C, 0, 0x004C }, /* Latin CAPITAL LETTER L */
    { 0x4D, 0, 0x004D }, /* Latin CAPITAL LETTER M */
    { 0x4E, 0, 0x004E }, /* Latin CAPITAL LETTER N */
    { 0x4F, 0, 0x004F }, /* Latin CAPITAL LETTER O */
    { 0x50, 0, 0x0050 }, /* Latin CAPITAL LETTER P */
    { 0x51, 0, 0x0051 }, /* Latin CAPITAL LETTER Q */
    { 0x52, 0, 0x0052 }, /* Latin CAPITAL LETTER R */
    { 0x53, 0, 0x0053 }, /* Latin CAPITAL LETTER S */
    { 0x54, 0, 0x0054 }, /* Latin CAPITAL LETTER T */
    { 0x55, 0, 0x0055 }, /* Latin CAPITAL LETTER U */
    { 0x56, 0, 0x0056 }, /* Latin CAPITAL LETTER V */
    { 0x57, 0, 0x0057 }, /* Latin CAPITAL LETTER W */
    { 0x58, 0, 0x0058 }, /* Latin CAPITAL LETTER X */
    { 0x59, 0, 0x0059 }, /* Latin CAPITAL LETTER Y */
    { 0x5A, 0, 0x005A }, /* Latin CAPITAL LETTER Z */
    { 0x5B, 0, 0x00C4 }, /* Latin CAPITAL LETTER A WITH DIAERESIS */
    { 0x5C, 0, 0x00D6 }, /* Latin CAPITAL LETTER O WITH DIAERESIS */
    { 0x5D, 0, 0x00D1 }, /* Latin CAPITAL LETTER N WITH TILDE */
    { 0x5E, 0, 0x00DC }, /* Latin CAPITAL LETTER U WITH DIAERESIS */
    { 0x5F, 0, 0x00A7 }, /* SECTION SIGN */
    { 0x60, 0, 0x00BF }, /* INVERTED QUESTION MARK */
    { 0x61, 0, 0x0061 }, /* Latin A */
    { 0x62, 0, 0x0062 }, /* Latin B */
    { 0x63, 0, 0x0063 }, /* Latin C */
    { 0x64, 0, 0x0064 }, /* Latin D */
    { 0x65, 0, 0x0065 }, /* Latin E */
    { 0x66, 0, 0x0066 }, /* Latin F */
    { 0x67, 0, 0x0067 }, /* Latin G */
    { 0x68, 0, 0x0068 }, /* Latin H */
    { 0x69, 0, 0x0069 }, /* Latin I */
    { 0x6A, 0, 0x006A }, /* Latin J */
    { 0x6B, 0, 0x006B }, /* Latin K */
    { 0x6C, 0, 0x006C }, /* Latin L */
    { 0x6D, 0, 0x006D }, /* Latin M */
    { 0x6E, 0, 0x006E }, /* Latin N */
    { 0x6F, 0, 0x006F }, /* Latin O */
    { 0x70, 0, 0x0070 }, /* Latin P */
    { 0x71, 0, 0x0071 }, /* Latin Q */
    { 0x72, 0, 0x0072 }, /* Latin R */
    { 0x73, 0, 0x0073 }, /* Latin S */
    { 0x74, 0, 0x0074 }, /* Latin T */
    { 0x75, 0, 0x0075 }, /* Latin U */
    { 0x76, 0, 0x0076 }, /* Latin V */
    { 0x77, 0, 0x0077 }, /* Latin W */
    { 0x78, 0, 0x0078 }, /* Latin X */
    { 0x79, 0, 0x0079 }, /* Latin Y */
    { 0x7A, 0, 0x007A }, /* Latin Z */
    { 0x7B, 0, 0x00E4 }, /* Latin A WITH DIAERESIS */
    { 0x7C, 0, 0x00F6 }, /* Latin O WITH DIAERESIS */
    { 0x7D, 0, 0x00F1 }, /* Latin N WITH TILDE */
    { 0x7E, 0, 0x00FC }, /* Latin U WITH DIAERESIS */
    { 0x7F, 0, 0x00E0 }, /* Latin A WITH GRAVE */
};

/*
 * GSM7BIT ��UCS2ת������չ��,GSM7BIT��չ����չ��־Ϊ0x1b�����±�GSM7BIT������
 * ��������0x1B65ʱ�����Ӧ��UCS2����Ϊ0x20AC�������ִ���ŷԪ��
 */
const AT_PB_ConversionTable g_gsm7extToUnicode[AT_PB_GSM7EXT_MAX_NUM] = {
    { 0x65, 0, 0x20AC }, /* EURO SIGN */
    { 0x0A, 0, 0x000C }, /* FORM FEED */
    { 0x14, 0, 0x005E }, /* CIRCUMFLEX ACCENT */
    { 0x28, 0, 0x007B }, /* LEFT CURLY BRACKET */
    { 0x29, 0, 0x007D }, /* RIGHT CURLY BRACKET */
    { 0x2F, 0, 0x005C }, /* REVERSE SOLIDUS */
    { 0x3C, 0, 0x005B }, /* LEFT SQUARE BRACKET */
    { 0x3D, 0, 0x007E }, /* TILDE */
    { 0x3E, 0, 0x005D }, /* RIGHT SQUARE BRACKET */
    { 0x40, 0, 0x007C }, /* VERTICAL LINE */
};

const AT_STRING_Type g_atStringTab[] = {
    { AT_STRING_SM, (TAF_UINT8 *)"\"SM\"" },
    { AT_STRING_ME, (TAF_UINT8 *)"\"ME\"" },
    { AT_STRING_ON, (TAF_UINT8 *)"\"ON\"" },
    { AT_STRING_EN, (TAF_UINT8 *)"\"EN\"" },
    { AT_STRING_FD, (TAF_UINT8 *)"\"FD\"" },
    { AT_STRING_BD, (TAF_UINT8 *)"\"BD\"" },
    { AT_STRING_REC_UNREAD_TEXT, (TAF_UINT8 *)"\"REC UNREAD\"" },
    { AT_STRING_REC_READ_TEXT, (TAF_UINT8 *)"\"REC READ\"" },
    { AT_STRING_STO_UNSENT_TEXT, (TAF_UINT8 *)"\"STO UNSENT\"" },
    { AT_STRING_STO_SENT_TEXT, (TAF_UINT8 *)"\"STO SENT\"" },
    { AT_STRING_ALL_TEXT, (TAF_UINT8 *)"\"ALL\"" },
    { AT_STRING_REC_UNREAD_PDU, (TAF_UINT8 *)"0" },
    { AT_STRING_REC_READ_PDU, (TAF_UINT8 *)"1" },
    { AT_STRING_STO_UNSENT_PDU, (TAF_UINT8 *)"2" },
    { AT_STRING_STO_SENT_PDU, (TAF_UINT8 *)"3" },
    { AT_STRING_ALL_PDU, (TAF_UINT8 *)"4" },
    { AT_STRING_IP, (TAF_UINT8 *)"\"IP\"" },
    { AT_STRING_IPv4, (VOS_UINT8 *)"\"IPv4\"" },
    { AT_STRING_PPP, (TAF_UINT8 *)"\"PPP\"" },
    { AT_STRING_IPV6, (VOS_UINT8 *)"\"IPV6\"" },
    { AT_STRING_IPV4V6, (VOS_UINT8 *)"\"IPV4V6\"" },
    { AT_STRING_IPv6, (VOS_UINT8 *)"\"IPv6\"" },
    { AT_STRING_IPv4v6, (VOS_UINT8 *)"\"IPv4v6\"" },
    { AT_STRING_Ethernet, (VOS_UINT8 *)"\"Ethernet\"" },

    { AT_STRING_0E0, (TAF_UINT8 *)"\"0E0\"" },
    { AT_STRING_1E2, (TAF_UINT8 *)"\"1E2\"" },
    { AT_STRING_7E3, (TAF_UINT8 *)"\"7E3\"" },
    { AT_STRING_1E3, (TAF_UINT8 *)"\"1E3\"" },
    { AT_STRING_1E4, (TAF_UINT8 *)"\"1E4\"" },
    { AT_STRING_1E5, (TAF_UINT8 *)"\"1E5\"" },
    { AT_STRING_1E6, (TAF_UINT8 *)"\"1E6\"" },
    { AT_STRING_1E1, (TAF_UINT8 *)"\"1E1\"" },
    { AT_STRING_5E2, (TAF_UINT8 *)"\"5E2\"" },
    { AT_STRING_5E3, (TAF_UINT8 *)"\"5E3\"" },
    { AT_STRING_4E3, (TAF_UINT8 *)"\"4E3\"" },
    { AT_STRING_6E8, (TAF_UINT8 *)"\"6E8\"" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: CS��״̬��Ϣ�����ϱ�
     * [˵��]: CS��״̬��Ϣ�����ϱ���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: ����ע���״̬�����ı�ʱ��
     *             <CR><LF>+CREG: <stat><CR><LF>
     *     [����]: -
     *     [���]: С����Ϣ�����ı�ʱ��
     *             <CR><LF>+CREG: <stat>[,<lac>,<ci>[,<AcT>]]<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CREG�������ϱ���
     *             1��ʹ��+CREG: <stat>�������ϱ���
     *             2��ʹ��+CREG: <stat>[,<lac>,<ci>[,<AcT>]]�������ϱ���
     *     <stat>: ����ֵ��ע��״̬��
     *             0��û��ע�ᣬMTû�д���������ע���µ���Ӫ��״̬��
     *             1��ע���˹������磻
     *             2��û��ע�ᣬMT����������ע���µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪״̬��
     *             5��ע�����������硣
     *     <lac>: 2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *     <ci>: 4byte��16���������ַ������ͣ�С����Ϣ��
     * [ʾ��]:
     *       ����ע���״̬�����ı�ʱ
     *       +CREG: 1
     *       С����Ϣ�����ı�ʱ
     *       +CREG: 1,"00C3","0000001A",4
     */
    { AT_STRING_CREG, (TAF_UINT8 *)"+CREG: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: PS��״̬��Ϣ�����ϱ�
     * [˵��]: PS��״̬��Ϣ�����ϱ���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: ����ע���״̬�����ı�ʱ��
     *             <CR><LF>+CGREG: <stat><CR><LF>
     *     [����]: -
     *     [���]: С����Ϣ�����ı�ʱ��
     *             <CR><LF>+CGREG: <stat>[,<lac>,<ci>[,<AcT>]]<CR><LF>
     * [����]:
     *     <n>: ����ֵ�������ϱ���ʽ��Ĭ��ֵΪ0��
     *             0����ֹ+CGREG�������ϱ���
     *             1��ʹ��+CGREG: <stat>�������ϱ���ʽ��
     *             2��ʹ��+CGREG: <stat>[,<lac>,<ci>[,<AcT>]]�������ϱ���ʽ��
     *     <stat>: ����ֵ��ע��״̬��
     *             0��û��ע�ᣬMT����û��������ע���µ���Ӫ�̣�
     *             1��ע���˹������磻
     *             2��û��ע�ᣬMT����������ע���µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪״̬��
     *             5��ע�����������硣
     *     <lac>: 2byte��16���������ַ������ͣ�λ������Ϣ�����磺��00C3����ʾ10���Ƶ�195��
     *     <ci>: 4byte��16���������ַ������ͣ�С����Ϣ��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             0��GSM��
     *             1��GSM Compact��
     *             2��UTRAN��
     *             3��GSM EGPRS��
     *             4��UTRAN HSDPA��
     *             5��UTRAN HSUPA��
     *             6��UTRAN HSDPA��HSUPA��
     *             7��E-UTRAN��
     *             10��EUTRAN-5GC��
     *             11��NR-5GC��
     * [ʾ��]:
     *       ����ע���״̬�����ı�ʱ
     *       +CGREG: 1
     *       С����Ϣ�����ı�ʱ
     *       +CGREG: 1,"00C3","0000001A",4
     */
    { AT_STRING_CGREG, (TAF_UINT8 *)"+CGREG: " },
#if (FEATURE_LTE == FEATURE_ON)
    { AT_STRING_CEREG, (TAF_UINT8 *)"+CEREG: " },
#endif
    /*
     * [���]: Э��AT-�绰����
     * [����]: ָʾ����״̬�仯
     * [˵��]: ������״̬�����仯ʱ��MT�����ϱ��µķ���״̬��TE��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SRVST: <srv_status><CR><LF>
     * [����]:
     *     <srv_status>: �����ͣ�ϵͳ����״̬��ȡֵ0��4��
     *             0���޷���
     *             1�������Ʒ���
     *             2��������Ч��
     *             3�������Ƶ��������
     *             4��ʡ�����˯״̬��
     * [ʾ��]:
     *       ����״̬�ı�Ϊ2ʱ
     *       ^SRVST: 2
     */
    { AT_STRING_SRVST, (TAF_UINT8 *)"^SRVST: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: ָʾϵͳģʽ�仯�¼�
     * [˵��]: ��ϵͳģʽ�仯ʱ��MT�����ϱ���ָʾ��TE��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^MODE: <sys_mode>,<sys_submode><CR><LF>
     * [����]:
     *     <sys_mode>: �����ͣ�ϵͳģʽ��
     *             0���޷���
     *             3��GSM/GPRS ģʽ��
     *             5��WCDMAģʽ��
     *             6��GPSģʽ����֧�֣���
     *             7��LTE ģʽ��
     *             10��EUTUAN-5GC;
     *             11��NR-5GC;
     *             15��TD_SCDMAģʽ;
     *     <sys_submode>: �����ͣ�ϵͳ��ģʽ��
     *             0���޷���
     *             1��GSMģʽ��
     *             2��GPRSģʽ��
     *             3��EDGEģʽ��
     *             4��WCDMAģʽ��
     *             5��HSDPAģʽ��
     *             6��HSUPA mode��
     *             7��HSDPA mode and HSUPA mode��
     *             8��TD_SCDMAģʽ��
     *             9��HSPA+ ģʽ��
     *             10��LTE ģʽ��
     *             17��DC-HSPA+ģʽ��
     *             18��MIMO-HSPA+ ģʽ��
     *             110��EUTUAN-5GC;
     *             111��NR-5GC��
     * [ʾ��]:
     *       �û�����ģʽ�ϱ�AT^MODE=1���緢��������ģʽ�仯��G->L, G->W,W->L�ȣ�^MODE�ϱ�ģʽ�仯�¼���
     *       ϵͳģʽ�仯ΪWCDMA��ģʱ
     *       ^MODE: 5,4
     *       ��ǰ������WCDMAģʽ�º�פ����TD-SCDMAģʽ
     *       ^MODE: 15,8
     *       ��ǰ������TD-SCDMAģʽ�º�פ����WCDMAģʽ��
     *       ^MODE: 5,4
     *       ��ǰ������GULģʽ���л�Lʱ����Ҫ��Lģ��ʾΪW
     *       ^MODE: 5,4
     */
    { AT_STRING_MODE, (TAF_UINT8 *)"^MODE: " },
    { AT_STRING_RSSI, (TAF_UINT8 *)"^RSSI: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: ����ʱ���ϱ���ʽ
     * [˵��]: ������涨����ʱ����ϱ���ʽ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^TIME: < date >,< time>,<dt><CR><LF>
     * [����]:
     *     <date>: ���ں�ʱ����Ϣ����ʽΪyy/mm/dd��
     *     <time>: ��ʽΪ"hh:mm:ss(+/-)tz"��<time>�ֶ���Ϣ����������ɣ�ʱ���ʱ�������硰05:56:13+32����tz:ʱ����Ϣ��ʱ����λΪ15����,  +32 ��ʾ32x15���� �� + 8Сʱ��
     *     <dt>: ����ʱ��������û���·��˲���ʱ�����ϱ�0�������ϱ���Ӧ������ʱ������ȡֵ������������ο�table 10.5.97a/3GPP TS 24.008
     * [ʾ��]:
     *       �ϱ�����ʱ��
     *       ^TIME: "12/06/06,09:02:22+32,00"
     */
    { AT_STRING_TIME, (TAF_UINT8 *)"^TIME: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: �����ϱ�����ʱ����Ϣ
     * [˵��]: �����·�NITZ (Network Identity and Time Zone)֪ͨ�ն˵�ǰ�����ʱ����ʱ�����Ϣ���ն�ͨ������������ʱ���ϱ���Ӧ�á�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CTZV: < tz ><CR><LF>
     * [����]:
     *     <tz>: ����ֵ��ʱ����Ϣ����Χ��-48~+56��
     *             ʱ����Ϣֵ���̶�Ϊ2λ���ֱ�ʾ������2λ����0��λ����-9���ʱΪ-09��+0���ʱΪ+00��
     * [ʾ��]:
     *       �յ������·���NITZ��Ϣ
     *       +CTZV: "+15"
     */
    { AT_STRING_CTZV, (TAF_UINT8 *)"+CTZV: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: �����ϱ�����ʱ����ʱ����Ϣ
     * [˵��]: �����·�NITZ(Network Identity and Time Zone)֪ͨ�ն˵�ǰ�����ʱ����ʱ����Ϣ���ն�ͨ������������ʱ��������ʱ�ƺ�ʱ����Ϣ�ϱ���Ӧ�á�
     *         ���������3GPP TS 27.007 Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CTZE: <tz>,<dst>,[<time>] <CR><LF>
     * [����]:
     *     <tz>: ����ֵ��ʱ����Ϣ����Χ��-48~+56��
     *             ʱ����Ϣֵ���̶�Ϊ2λ���ֱ�ʾ������2λ����0��λ����-9���ʱΪ-09��+0���ʱΪ+00��
     *     <dst>: 0��������ʱ��
     *             1������������ʱ��<tz>����+1Сʱ(�൱��tzֵ����4)������ʱʱ����ڡ�
     *             2������������ʱ��<tz>����+2Сʱ(�൱��tzֵ����8)������ʱʱ����ڡ�
     *     <time>: ʱ����Ϣ�ַ�����
     *             ��ʽΪyyyy/mm/dd,hh:mm:ss��
     * [ʾ��]:
     *       �յ������·���NITZ��Ϣ
     *       +CTZE: "+02",1,"2015/12/01,06:25:00"
     */
    { AT_STRING_CTZE, (TAF_UINT8 *)"+CTZE: " },
    { AT_STRING_ERRRPT, (TAF_UINT8 *)"^ERRRPT:" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �ϱ�����״̬
     * [˵��]: ʹ��^CURC�����ϱ�����������й����У������ϱ�����״̬�ı仯��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CCALLSTATE: <call_id>,<state>��<voice_domain><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *     <state>: �����ͣ�ָʾ��Ӧcall_id�ĺ���״̬��
     *             0��Call originate(MO)�������緢����У�
     *             1��Call is Proceeding�����з��������Ѿ���ʼ����
     *             2��Alerting�����壻
     *             3��Connected�������Ѿ���ͨ��
     *             4��Released�����б��ͷţ�
     *             5��InComing�������磻
     *             6��Waiting����һ���ȴ������磻
     *             7��Hold�����б��֣�
     *             8��Retrieve�����лָ���
     *     <voice_domain>: ����ֵ�������绰�ĺ�����
     *             0��3GPP CS�������绰
     *             1��IMS�������绰
     *             2��3GPP2 CS�������绰
     *             3��δ���κ����Ϸ���绰
     * [ʾ��]:
     *       ��IMS����������
     *       ^CCALLSTATE: 1,5,1
     */
    { AT_STRING_CCALLSTATE, (VOS_UINT8 *)"^CCALLSTATE" },

    { AT_STRING_CERSSI, (VOS_UINT8 *)"^CERSSI:" },

    { AT_STRING_ACINFO, (VOS_UINT8 *)"^ACINFO:" },

    /*
     * [���]: Э��AT-�绰����
     * [����]: �����ϱ�����������Ϣ
     * [˵��]: ������������ͨ�������������ϱ����������״̬��
     *         ������緢��ALERTING��Ϣ����ʱ����������������������Ҫ��A progress indicator IE indicates user attachment if it specifies a progress description in the set {1, 2, 3} or in the set {6, 7, 8, ..., 20}. ������ʱ֪ͨAP����б������������
     *         ����ڱ�����������У����յ�������֪ͨ���Խ��з����������пտ���ϢЯ����progress indicator������ progress description��ֵΪ{1, 2, 3}����{6, 7, 8, ..., 20}����֪ͨAP���������壬��ʱAP��ֹͣ���ص����塣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CSCHANNELINFO: <status>,<voice_domain><CR><LF>
     * [����]:
     *     <status>: 0�����壬�޴�������Ϣ��
     *             1�����������ã�խ��������
     *             2�����������ã����������
     *             3�����������ã�EVS NB������
     *             4�����������ã�EVS WB������
     *             5�����������ã�EVS SWB������
     *     <voice_domain>: ����ֵ�������绰�ĺ�����
     *             0��CS�������绰
     *             1��IMS�������绰
     * [ʾ��]:
     *       CS�������绰�������޴�����Ϣ�������ϱ����£�
     *       ^ CSCHANNELINFO: 0,0
     *       CS�������绰��ͨ���п��������Ϊխ������ʱ�������ϱ����£�
     *       ^ CSCHANNELINFO: 1,0
     *       IMS�������绰����ǰ�������ڿ������ʱ�������ϱ����£�
     *       ^ CSCHANNELINFO: 2,1
     */
    { AT_STRING_CS_CHANNEL_INFO, (VOS_UINT8 *)"^CSCHANNELINFO:" },

    { AT_STRING_RESET, (VOS_UINT8 *)"^RESET:" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: GPS�ο�ʱ��״̬�ϱ�
     * [˵��]: ����������AP-Modem��̬�������ϱ�GPS�ο�ʱ��״̬���Ա��ⲻȷ���Ĳο�ʱ��״̬��Ӱ�졣
     * [�﷨]:
     *     [����]: -
     *     [���]: GPS�ο�ʱ��״̬�����ı�ʱ��
     *             <CR><LF>^REFCLKFREQ: <ver>,<freq>,<precision> <status><CR><LF>
     * [����]:
     *     <ver>: ����ֵ��������İ汾ID����ǰΪ0��
     *     <freq>: ����ֵ��GPS�ο�ʱ�ӵ�Ƶ��ֵ����λHz��
     *     <precision>: ����ֵ����ǰGPS�ο�ʱ�ӵľ��ȣ���λppb��
     *     <status>: ����ֵ��GPS�ο�ʱ��Ƶ������״̬��
     *             0��δ������
     *             1��������
     * [ʾ��]:
     *       GPS�ο�ʱ��Ƶ�ʱ�Ϊ����״̬
     *       ^REFCLKFREQ: 0,19200000,100,1
     */
    { AT_STRING_REFCLKFREQ, (VOS_UINT8 *)"^REFCLKFREQ" },

    { AT_STRING_C5GNSSAA, (VOS_UINT8 *)"^C5GNSSAA" },

    { AT_STRING_CPENDINGNSSAI, (VOS_UINT8 *)"^CPENDINGNSSAI" },

    { AT_STRING_REJINFO, (VOS_UINT8 *)"^REJINFO: " },

    { AT_STRING_PLMNSELEINFO, (VOS_UINT8 *)"^PLMNSELEINFO:" },

    { AT_STRING_NETSCAN, (VOS_UINT8 *)"^NETSCAN: " },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�SRVCC״̬
     * [˵��]: �����ϱ�SRVCC״̬���ܷ������ϱ���+CIREP������ơ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CIREPH: <srvcch><CR><LF>
     * [����]:
     *     <srvcch>: ����ֵ��SRVCC״̬��
     *             0��SRVCC��ʼ��
     *             1��SRVCC�ɹ���
     *             2��SRVCCȡ����
     *             3��SRVCCʧ�ܡ�
     * [ʾ��]:
     *       SRVCC״̬�����ϱ�
     *       +CIREPH: 1
     */
    { AT_STRING_CIREPH, (VOS_UINT8 *)"+CIREPH" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�IMSVOPSָʾ
     * [˵��]: �����ϱ�IMSVOPSָʾ���ܷ������ϱ���+CIREP������ơ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CIREPI: <nwimsvops><CR><LF>
     * [����]:
     *     <nwimsvops>: ����ֵ�������Ƿ�֧��IMSVOPS��ָʾ��
     *             0�����粻֧��IMSVOPS��
     *             1������֧��IMSVOPS��
     * [ʾ��]:
     *       ����֧��IMSVOPSָʾ�����ϱ�
     *       +CIREPI: 1
     */
    { AT_STRING_CIREPI, (VOS_UINT8 *)"+CIREPI" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�IMS��ע��״̬
     * [˵��]: �����ϱ���ǰIMS������ע��״̬���ܷ������ϱ���+CIREG������ơ�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CIREGU: <reg_info>[,<ext_info>]<CR><LF>
     * [����]:
     *     <reg_info>: ����ֵ��ע��״̬��
     *             0��û��ע�᣻
     *             1��ע����IMS���硣
     *     <ext_info>: ����ֵ��IMS������ֵ��ȡֵ��Χ[1,0xFFFFFFFF]��ÿ��bitλ��ʾ��ͬ��������
     *             0��+CIREG=2ʱ ����չ��Ϣ��ʱ�ظ�0
     *             1��RTP-based transfer of voice
     *             2��RTP-based transfer of text
     *             4��SMS using IMS functionality
     *             8��RTP-based transfer of video
     *             ���磺=5��ʱ�򣬱�ʾ֧��RTP-based transfer of voice��SMS using IMS functionality��
     * [ʾ��]:
     *       ����+CIREG=2ʱ��IMS��ע��״̬�ϱ�
     *       +CIREGU: 1,0
     *       OK
     */
    { AT_STRING_CIREGU, (VOS_UINT8 *)"+CIREGU" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����Ƶͨ�����ͱ��֪ͨ
     * [˵��]: ����Ƶͨ�����ͱ��֪ͨ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^MODIFYIND: <idx>,<curr_call_type>,<curr_call_domain>,<dest_call_type>,<dest_call_domain>,<modify_reason><CR><LF>
     * [����]:
     *     <idx>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <curr_call_type>: ����ֵ����ǰ�������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <curr_call_domain>: ����ֵ����ǰ������
     *             0�� CS��
     *             1�� PS��
     *     <dest_call_type>: ����ֵ��Ŀ��������͡�
     *             0�� ��������
     *             1�� ��Ƶͨ������������Ƶ��˫������
     *             2�� ��Ƶͨ�������������Ƶ��˫������
     *             3�� ��Ƶͨ����˫����Ƶ��˫������
     *     <dest_call_domain>: ����ֵ��Ŀ�������
     *             0�� CS��
     *             1�� PS��
     *     <modify_reason>: ����ֵ��modify�¼�����ԭ��
     *             0��Զ���û���������modify�¼���
     *             1��Զ���û���RTP����RTCP��ʱ����modify�¼���
     *             2��Զ���û���RTP��RTCPͬʱ��ʱ����modify�¼���
     * [ʾ��]:
     *       ��Ƶ����Ƶͨ�����֪ͨ
     *       ^CALLMODIFYIND: 1,0,1,3,1,0
     */
    { AT_STRING_CALL_MODIFY_IND, (VOS_UINT8 *)"^CALLMODIFYIND:" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����Ƶͨ�����ͱ����ʼ֪ͨ
     * [˵��]: ����Ƶͨ�����ͱ����ʼ֪ͨ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CALLMODIFYBEG: <idx>,<callDomain><CR><LF>
     * [����]:
     *     <idx>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <callDomain>: ����ֵ��������
     *             0�� CS��
     *             1�� PS��
     * [ʾ��]:
     *       ����Ƶͨ�����ͱ����ʼ֪ͨ
     *       ^CALLMODIFYBEG: 1,1
     */
    { AT_STRING_CALL_MODIFY_BEG, (VOS_UINT8 *)"^CALLMODIFYBEG:" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ����Ƶͨ�����ͱ������֪ͨ
     * [˵��]: ����Ƶͨ�����ͱ������֪ͨ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CALLMODIFYEND: <idx>,<callDomain>,<err><CR><LF>
     * [����]:
     *     <idx>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <callDomain>: ����ֵ��������
     *             0�� CS��
     *             1�� PS��
     *     <err>: ����ԭ��ֵ(ͬCS�����Ժ��д���ԭ��ֵ)
     * [ʾ��]:
     *       ����Ƶͨ�����ͱ������֪ͨ
     *       ^CALLMODIFYEND: 1,1,1
     */
    { AT_STRING_CALL_MODIFY_END, (VOS_UINT8 *)"^CALLMODIFYEND:" },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �����ϱ�ECONF�����״̬�����ı�
     * [˵��]: ���������ϱ���ǿ�Ͷ෽ͨ������������״̬�����ı䡣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ECONFSTATE: <call_num><CR><LF>
     * [����]:
     *     <call_num>: ���и���
     * [ʾ��]:
     *       �����ϱ���ǿ�Ͷ෽ͨ���������״̬�����ı�
     *       ^ECONFSTATE: 1
     */
    { AT_STRING_ECONFSTATE, (VOS_UINT8 *)"^ECONFSTATE:" },

#endif

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    { AT_STRING_CDISP, (TAF_UINT8 *)"^CDISP: " },
    { AT_STRING_CCONNNUM, (TAF_UINT8 *)"^CCONNNUM: " },
    { AT_STRING_CCALLEDNUM, (TAF_UINT8 *)"^CCALLEDNUM: " },
    { AT_STRING_CCALLINGNUM, (TAF_UINT8 *)"^CCALLINGNUM: " },
    { AT_STRING_CREDIRNUM, (TAF_UINT8 *)"^CREDIRNUM: " },
    { AT_STRING_CSIGTONE, (TAF_UINT8 *)"^CSIGTONE: " },
    { AT_STRING_CLCTR, (TAF_UINT8 *)"^CLCTR: " },
    { AT_STRING_CCWAC, (TAF_UINT8 *)"^CCWAC: " },
#endif
    { AT_STRING_FILECHANGE, (VOS_UINT8 *)"^FILECHANGE:" },

#if ((FEATURE_LTE == FEATURE_ON) && (FEATURE_LTE_MBMS == FEATURE_ON))
    { AT_STRING_MBMSEV, (VOS_UINT8 *)"^MBMSEV:" },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �б������������ѵ�PLMN�������ϱ�
     * [˵��]: �����������û��б������������ѵ�PLMN�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SRCHEDPLMNLIST: <rslt1>[,<rslt2>,��,<rsltx>]<CR><LF>
     * [����]:
     *     <rslt>: �ϱ��Ľ����ÿһ�������һ��Ԫ����ɣ�(<stat>,<operLong>,<operShort>,<operNum>,<rat>,<tac>,<rscp>)
     *     <stat>: ����ֵ�������״̬��ʶ��
     *             0��δ֪��
     *             1�����ã�
     *             2����ע�᣻
     *             3����ֹ��
     *     <operLong>: �ַ������ͣ����ַ�����ʽ����Ӫ����Ϣ
     *     <operShort>: �ַ������ͣ����ַ�����ʽ����Ӫ����Ϣ
     *     <operNum>: �ַ������ͣ����ָ�ʽ����Ӫ����Ϣ
     *     <rat>: ����ֵ�����߽��뼼����Ĭ��ֵ��SYSCFG�����õ����ȼ���ͬ��
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE��
     *             12: NR��
     *     <tac>: ����ֵ����ʾ��������롣
     *     <rscp>: ����ֵ����NR��LTE�£��������С�����ź�ǿ����Ϣ����RSRP��NR/L-Rsrp�ĵ�λ:0.125dbm, ��Чֵ99������Ϊ��ֵ
     *             W�£��������С���ź�������Ϣ����RSCP��
     *             G�£��������С��RSSI��Ϣ��
     * [ʾ��]:
     *       �����������ѵ�4����Ч���磬NR,L,W,G��һ��:
     *       ^SRCHEDPLMNLIST: (2,"CHINA MOBILE","CMCC","46001",12,1,-65),(1,"CHINA MOBILE","CMCC","46001",7,2,-89),(1,"CHINA MOBILE","CMCC","46001",2,3,-70),(1,"CHN-UNICOM","UNICOM","46000",0,4,-65)
     *       OK
     */
    { AT_STRING_SRCHEDPLMNLIST, (VOS_UINT8 *)"^SRCHEDPLMNLIST:" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: MCC�����ϱ�
     * [˵��]: CSS�޸ù��Ҷ�Ӧ��Ƶ��Ƶ����Ϣʱ�����ϱ�MCC��AP�յ�^MCC���ϱ�����Ҫ�·�^MCC��Ӧ��Ԥ��Ƶ��Ƶ����Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^MCC: <VER> ,<MCC>[,MCC]<CR><LF>
     * [����]:
     *     <VER>: ��ͨѶƥ��Ƶ��/Ƶ�ΰ汾��
     *     <MCC>: MCC��Ϣ������һ����Ҳ���ܶ�������ʱ�ԡ����š�����
     * [ʾ��]:
     *       �����ϱ�MCC
     *       ^MCC: 00.00.000,460,230
     */
    { AT_STRING_MCC, (TAF_UINT8 *)"^MCC:" },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ����ж�λ�������
     * [˵��]: ���ж�λ���������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CMOLRE: <err><CR><LF>
     * [����]:
     *     <err>: ���ж�λ����Ĵ����롣��ʽ�ȿ���������Ҳ�������ַ�������+CMOLRE����������ơ�
     * [��]: +CMOLRE�������
     *       ������, ��������,                                                 ����˵��,
     *       0,      Method not supported,                                     ��λ������֧��,
     *       1,      Additional assistance data required,                      ��Ҫ����ĸ�����Ϣ,
     *       2,      Not enough satellites,                                    û���㹻������,
     *       3,      UE busy,                                                  �豸��æ,
     *       4,      Network error,                                            �������,
     *       5,      Failed to open internet connection, too many connections, ����������ʧ�ܣ���������,
     *       6,      Failed to open internet connection, too many users,       ����������ʧ�ܣ��û�����,
     *       7,      Failure due to handover,                                  �����л�����ʧ��,
     *       8,      Internet connection failure,                              ��������ʧ��,
     *       9,      Memory error,                                             �ڴ����,
     *       255,    Unknown error,                                            δ֪����,
     * [ʾ��]:
     *       �����ϱ����ж�λ��������������ָ�ʽ��
     *       +CMOLRE: 4
     *       �����ϱ����ж�λ�����������Ӵ����ַ�����ʽ��
     *       +CMOLRE: Internet connection failure
     */
    { AT_STRING_CMOLRE, (VOS_UINT8 *)"+CMOLRE: " },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ���λ����
     * [˵��]: NMEA�ַ�����ʽ�Ķ�λ�����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CMOLRN: <NMEA-string><CR><LF>
     * [����]:
     *     <NMEA-string>: UTF-8�ַ������ͣ��˲���ΪЭ���׼IEC 61162�ж����NMEA�ַ������������Ͳ��ܳ����ַ�ת������+CSCSӰ�졣
     * [ʾ��]:
     *       �����ϱ���λ��Ϣ
     *       +CMOLRN: "$GPRMC,235947.000,V,0000.0000,N,00000.0000,E,,,041299,,*1D"
     */
    { AT_STRING_CMOLRN, (VOS_UINT8 *)"+CMOLRN: " },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ���λ����
     * [˵��]: GAD��״���͵Ķ�λ�����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CMOLRG: <location_parameters><CR><LF>
     * [����]:
     *     <location_parameters>: UTF-8�ַ������ͣ��˲���ΪXML�ַ�����ʽ��ʾ��GAD��״���͵Ķ�λ��Ϣ��XML��ʽ�μ�3GPP 27007 Table 8.50-2��XML���Էֶ���ϱ���
     *             ע���������Ͳ��ܳ����ַ�ת������+CSCSӰ�졣
     * [ʾ��]:
     *       �����ϱ���λ��Ϣ
     *       +CMOLRG:
     *       <?xml version="1.0" ?>
     *       <location_parameters>
     *        <shape_data>
     *          <ellipsoid_point>
     *            <coordinate>
     *              <latitude>
     *                <north>0</north>
     *                <degrees>0</degrees>
     *             </latitude>
     *           <longitude>0</longitude>
     *          </coordinate>
     *         </ellipsoid_point>
     *        </shape_data>
     *       </location_parameters>
     */
    { AT_STRING_CMOLRG, (VOS_UINT8 *)"+CMOLRG: " },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ����ж�λ����ָʾ
     * [˵��]: ���ж�λ����ָʾ���ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CMTLR: <handle-id>,<notification-type>,<location-type>, [<client-external-id>], [<client-name>][,<plane>]<CR><LF>
     * [����]:
     *     <handle-id>: ����ֵ��ÿ��MT-LR��ID�������ֶ�������е�ָ������ȡֵ��ΧΪ0~255��
     *     <notification-type>: ����ֵ���й��û���˽��ָʾ���ͣ�
     *             0�������������λ�û����������ѡ���û��Ѻõ���ʾ��
     *             1�����û�����ָʾʱ������λ�û���
     *             2�����û�����ָʾʱ����ֹ��λ�û���
     *     <location-type>: ����ֵ�������λ�����ͣ�
     *             0����ǰλ�ã�
     *             1����ǰλ�û�����֪λ�ã�
     *             2����ʼλ�á�
     *     <client-external-id>: �ַ������ͣ�ָʾ�ⲿ�ͻ��˶�λ��Ϣ���͵�λ�ã���ѡ����
     *     <client-name>: �ַ������ͣ������û�λ�õ��ⲿ�ͻ��˱�ʶ��
     *     <plane>: ����ֵ��MT-LR�ϱ��ķ�ʽ��
     *             0�������棻
     *             1��SUPL��
     * [ʾ��]:
     *       �����ϱ����ж�λ����ָʾ
     *       +CMTLR: 1,0,0
     */
    { AT_STRING_CMTLR, (VOS_UINT8 *)"+CMTLR: " },

#if (FEATURE_IMS == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: IMS֪ͨAP��Ҫ����DM������
     * [˵��]: ����������IMSAָʾAP��Ҫ����DM��������
     * [�﷨]:
     *     [����]:
     *     [���]: <CR><LF>^DMCN<CR><LF>
     * [����]:
     *     <��>: �������ϱ�������������
     * [ʾ��]:
     *       IMSע���յ�403��Ӧ��֪ͨAP��Ҫ����DM����������
     *       ^DMCN
     */
    { AT_STRING_DMCN, (VOS_UINT8 *)"^DMCN" },
#endif

    /*
     * [���]: Э��AT-�绰����
     * [����]: IMS֪ͨAP��ǰ�ܾ���Ϣ
     * [˵��]: ����������IMSָʾAP��ǰ�ܾ������
     * [�﷨]:
     *     [����]: -
     *     [���]: ^IMSREGFAIL: <FAIL_CAUSE>
     * [����]:
     *     <FAIL_CAUSE>: 1����ʾIMS���ñ��ܣ�
     * [ʾ��]:
     *       IMSע���յ�����403��Ӧ��֪ͨAP��ǰIMS���ñ��ܣ�
     *       ^IMSREGFAIL: 1
     */
    { AT_STRING_IMS_REG_FAIL, (VOS_UINT8 *)"^IMSREGFAIL: " },

    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: IMSI��ˢ�µ������ϱ�
     * [˵��]: ����������IMSI��ˢ��ʱ�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSIREFRESH <CR><LF>
     * [����]:
     * [ʾ��]:
     *       IMSI��ˢ��
     *       ^IMSIREFRESH
     */
    { AT_STRING_IMSI_REFRESH, (VOS_UINT8 *)"^IMSIREFRESH" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: AFCʱ��ʧ��״ָ̬ʾ
     * [˵��]: ����������GPSоƬ��ʹ��Modem�ṩ��AFCʱ�ӽ���ʱ��У׼ʱ����ѯModem��AFCʱ��״̬Ϊ����̬��֮��Modem��AFCʱ�����ڶ�ģ�����˯�ߵ�ԭ���µ�AFCʱ��ʧ�����¼�֪ͨ��GPSоƬ���Ա�GPSоƬ��ʱ��ֹ����У׼���̡�
     *         ע�⣺������ֻ����AT^AFCCLKINFO?��ѯ���Ϊ����״̬�󣬳���<cause>�����AFCʱ��ʧ��ʱ�����㲥�ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^AFCCLKUNLOCK: <cause><CR><LF>
     * [����]:
     *     <cause>: �������ͣ�AFCʱ��ʧ����ԭ��
     *             0����ģ�����˯�ߵ��µ�ʧ����
     * [ʾ��]:
     *       GPSоƬ��ѯAFCʱ�Ӵ�������״̬�󣬶�ģ��������˯�ߵ���ʱ��ʧ��ʱ��
     *       ^AFCCLKUNLOCK: 0
     */
    { AT_STRING_AFCCLKUNLOCK, (VOS_UINT8 *)"^AFCCLKUNLOCK: " },

    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �Ƿ������ű�����ʾ�������ϱ�
     * [˵��]: ����������IMS�����Ƿ������ű�����ʾ�������ϱ���AP��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSHOLDTONE: <hold_tone><CR><LF>
     * [����]:
     *     <hold_tone>: �Ƿ񲥷ű�����ʾ�����ͣ�����ֵ
     *             0�������ţ�
     *             1�����š�
     * [ʾ��]:
     *       �յ��Ƿ������ű�����ʾ�������ϱ�
     *       ^IMSHOLDTONE: 1
     */
    { AT_STRING_IMS_HOLD_TONE, (VOS_UINT8 *)"^IMSHOLDTONE" },

    /*
     * [���]: Э��AT-������ҵ��
     * [����]: �����ϱ�PDP�����Ƿ񱻽�ֹ
     * [˵��]: ^LIMITPDPACT���������ϱ�PDP�����Ƿ񱻽�ֹ
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LIMITPDPACT: <FLG><CAUSE><CR><LF>
     * [����]:
     *     <FLG>: 1:��ֹPDP����
     *             0:����PDP����
     *     <CAUSE>: PDP�����ԭ��ֵ������FLGΪ1ʱ��Ч��
     *             FLGΪ0ʱ��CAUSEֵ�̶�Ϊ0
     * [ʾ��]:
     *       PDP�����#33�ﵽ3�Σ�APS��2h��ʱ������ֹPDP����ʱ��
     *       ^LIMITPDPACT: 1,289
     */
    { AT_STRING_LIMITPDPACT, (VOS_UINT8 *)"^LIMITPDPACT" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMSʧ��ԭ��ֵ�����ϱ�
     * [˵��]: IMSע��ʧ��ʱ�������ϱ���ǰIMSע��ʧ�ܵ�ԭ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSREGERR: <err_domain>,<err_stage>,<fail_cause>,<fail_str><CR><LF>
     * [����]:
     *     <err_domain>: �����ͣ�IMSע��ʧ����ȡֵ0��4��
     *             0��IMSע��ʧ����ΪLTE��
     *             1��IMSע��ʧ����ΪWIFI��
     *             2��IMSע��ʧ����ΪUtran��
     *             3��IMSע��ʧ����ΪGsm��
     *             4��IMSע��ʧ����ΪNR��
     *             ʾ����
     *             ��UE��LTE��IMSע��ɹ���IMS���LTE�л���WIFI�Ĺ����У���ʱ�ϱ�IMSREGERR��err_domain��ָ���ڳ���IMSע����Ǹ��򣬼�WIFI��
     *     <err_stage>: �����ͣ�IMSע��ʧ�ܽ׶Σ�ȡֵ0��1��
     *             0��PDN�����׶Σ�
     *             1��IMSע��׶Σ�
     *     <fail_cause>: �����ͣ�IMSע��ʧ�ܽ׶Σ�ȡֵ0~699��
     *             ע�⣺
     *             ���ܴ���err_stage��ͬ��fail_cause��ͬ�ĳ���������ʱ����ȫ��ͬ�����ִ���һ����APN�����׶εĴ���һ����IMSע��׶εĴ���ֻ����ԭ��ֵǡ����ͬ
     *     <fail_str>: �ַ����������·���ʧ��ԭ���ַ�����
     *             ����IMSЭ��ջ�ڲ�ԭ��ֵʱ���ַ���Ϊ�գ�
     *             ������Я����ʧ��ԭ��ֵ�ַ���Ϊ��ʱ���˲���ҲΪ�գ�
     *             ����ʧ��ԭ��ֵ�ַ�����󳤶�Ϊ255��
     * [ʾ��]:
     *       LTE��PDN��������ʧ�ܣ�ʧ��ԭ��ֵΪ10
     *       ^IMSREGERR: 0,0,10,"network fail"
     *       WIFI��ע�����ʧ�ܣ�ʧ��ԭ��ֵΪ111����Ӧ���ַ���Ϊ��
     *       ^IMSREGERR: 1,0,111,""
     */
    { AT_STRING_IMS_REG_ERR, (VOS_UINT8 *)"^IMSREGERR:" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ͨ������С����Ϣ�����ϱ�
     * [˵��]: CSS�޸ù��Ҷ�Ӧ������С����Ϣʱ�����ϱ�^REPORTBLOCKCELLMCC��AP�յ�^REPORTBLOCKCELLMCC���ϱ�����Ҫ�·�^BLOCKCELLLIST���ö�Ӧ������С����Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^REPORTBLOCKCELLMCC: <VER> ,<MCC>[,<MCC>��.]<CR><LF>
     * [����]:
     *     <VER>: ��ͨѶƥ��汾��,�����ڡ�����
     *     <MCC>: MCC��Ϣ������һ����Ҳ���ܶ����Ŀǰ���17���������ʱ�ԡ����š�����
     * [ʾ��]:
     *       �����ϱ���ͨ������С��MCC��Ϣ
     *       ^REPORTBLOCKCELLMCC: "00.00.001",460,230
     */
    { AT_STRING_BLOCK_CELL_MCC, (VOS_UINT8 *)"^REPORTBLOCKCELLMCC: " },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ͨ�������Ϣ�����ϱ�
     * [˵��]: ATת��CSS�ϱ�����ͨ�������Ϣ��AP��
     * [�﷨]:
     *     [����]:
     *     [���]: <CR><LF>^REPORTCLOUDDATA: <LENGTH>,<BSINFO><CR><LF>
     * [����]:
     *     <LENGTH>: ASCII������ʾ���ֽڳ��ȣ����Ȳ�����500�����У�����ASCII������ʾһ���ֽڣ�
     *     <BSINFO>: ASCII����
     * [ʾ��]:
     *       �ϱ���ͨ�������Ϣ
     *       ^REPORTCLOUDDATA: 21,000000000200000002000000FF0406000001000F00
     */
    { AT_STRING_CLOUD_DATA, (VOS_UINT8 *)"^REPORTCLOUDDATA: " },

#if (FEATURE_APN_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�APN backoff��ʱ����Ϣ
     * [˵��]: ATת��THROT�����ϱ���APN backoff��ʱ�������Ϣ��AP��
     *         �Ƿ��ϱ���+CABTSR������ƣ�+CABTSRΪ1ʱ�����ϱ���
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     *         2����������FEATURE_APN_BASED_BO_TIMER_PARA_RPT����ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CABTSRI: <apn>,<event_type>[,<residual_backoff_time>,<re-attempt_rat_indicator>,<re-attempt_eplmn_indicator>,<NSLPI>[,<procedure>]]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <apn>: APN��Ϣ���ַ������ͣ���󳤶�Ϊ99��
     *     <event_type>: ָʾBack-off timer�¼���Ϣ���������ͣ�ȡֵ���£�
     *             0������back-off timer��ʱ����
     *             1��Back-off timerֹͣ��
     *             2��Back-off timer��ʱ����ʱ��
     *             3��Back-off timer����deactivated��
     *     <residual_backoff_time>: �ò�����ʾbackoff��ʱ��ʣ��ʱ������λ���롣�������͡�
     *             ע��
     *             ����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������š�
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <NSLPI>: ָʾ��������ȼ����������ͣ�ȡֵ���£�
     *             0������Ϊ��������ȼ�;
     *             1��û������Ϊ��������ȼ���
     *     <procedure>: ָʾ�ܵ�backoff��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1: backoff��ʱ������STAND ALONE PDN���̣�
     *             2: backoff��ʱ������BEARER ALLOC���̣�
     *             3: backoff��ʱ������BEARER MODIFY���̣�
     *             4: backoff��ʱ������PDP_ACTIVE���̣�
     *             5: backoff��ʱ������SECOND_PDP_ACTIVE���̣�
     *             6: backoff��ʱ������PDP_MODIFY���̣�
     *             7: backoff��ʱ������PDU_ESTABLISH���̣�
     *             8: backoff��ʱ������PDU_MODIFY���̣�
     *             9: backoff��ʱ������ATTACH��STAND ALONE��PDN���̡�
     * [ʾ��]:
     *       ��ʱ�����ڿ���״̬���ϱ�APN backoff��ʱ�������Ϣ
     *       +CABTSRI: "abc",0,240,0,1,1,1
     *       ��ʱ������ֹͣ״̬ʱ���ϱ�����Ϣ
     *       +CABTSRI: "abcD",1
     */
    { AT_STRING_CABTSRI, (VOS_UINT8 *)"+CABTSRI: " },
#endif

#if (FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT == FEATURE_ON)
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�S-NSSAI backoff��ʱ����Ϣ
     * [˵��]: ATת��THROT�����ϱ���S-NSSAI backoff��ʱ�������Ϣ��AP��
     *         �Ƿ������ϱ���+CSBTSR������ơ�
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT�����
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CSBTSRI: <S-NSSAI>,<event_type>[,<S-NSSAI_backoff_time>,<re-attempt_rat_indicator>,<re-attempt_eplmn_indicator>[,<procedure>]]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <S-NSSAI>: �ַ������ͣ�5GS�ķ�Ƭ��Ϣ��
     *             ��Ƭ��ʽ������ʾ��
     *             1��sst
     *             2��sst.sd
     *             3��sst;mappedSst
     *             4��sst.sd;mappedSst
     *             5��sst.sd;mappedSst.mappedSd
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F,a-f��
     *     <event_type>: ָʾBack-off timer�¼���Ϣ���������ͣ�ȡֵ���£�
     *             0������back-off timer��ʱ����
     *             1��Back-off timerֹͣ��
     *             2��Back-off timer��ʱ����ʱ��
     *             3��Back-off timer����deactivated��
     *     <S-NSSAI_backoff_time>: �ò�����ʾS-NSSAI backoff��ʱ��ʣ��ʱ������λ���룬�������͡�
     *             ע�⣺����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������š�
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <procedure>: ָʾ�ܵ�T3585��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1:��ʱ������PDU_ESTABLISH���̣�
     *             2:��ʱ������PDP_MODIFY���̡�
     * [ʾ��]:
     *       ��ʱ�����ڿ���״̬���ϱ�S-NSSAI backoff��ʱ�������Ϣ
     *       +CSBTSRI: ,0,180,0,1,0
     *       ��ʱ������ֹͣ״̬ʱ���ϱ�����Ϣ
     *       +CSBTSRI: "01.010101",1
     */
    { AT_STRING_CSBTSRI, (VOS_UINT8 *)"+CSBTSRI: " },

    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ�����S-NSSAI��DNN��backoff��ʱ����Ϣ
     * [˵��]: ATת��THROT�����ϱ��Ļ���S-NSSAI��DNN��backoff��ʱ�������Ϣ��AP��
     *         �Ƿ������ϱ���+CSDBTSR������ơ�
     *         ���������3GPP TS 27.007Э�顣
     *         ע��
     *         1����������FEATURE_NSSAI_BASED_BO_TIMER_PARA_RPT�����
     *         2��modem2�ݲ�֧�֣�����modem2���ʹ�����ʱ��ֱ�ӷ���error��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CSDBTSRI: <S-NSSAI>,<DNN>,<event_type>[,<S-NSSAI_DNN_backoff_time>,<re-attempt_rat_indicator>,<re-attempt_eplmn_indicator>[,<procedure>]]<CR><LF><CR><LF>OK<CR><LF>
     * [����]:
     *     <S-NSSAI>: �ַ������ͣ�5GS�ķ�Ƭ��Ϣ��
     *             ��Ƭ��ʽ������ʾ��
     *             1��sst
     *             2��sst.sd
     *             3��sst;mappedSst
     *             4��sst.sd;mappedSst
     *             5��sst.sd;mappedSst.mappedSd
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F,a-f��
     *     <DNN>: DNN��Ϣ���ַ������ͣ���󳤶�Ϊ99��
     *     <event_type>: ָʾBack-off timer�¼���Ϣ���������ͣ�ȡֵ���£�
     *             0������back-off timer��ʱ����
     *             1��Back-off timerֹͣ��
     *             2��Back-off timer��ʱ����ʱ��
     *             3��Back-off timer����deactivated��
     *     <S-NSSAI_DNN_backoff_time>: �ò�����ʾ����S-NSSAI��DNN��backoff��ʱ��ʣ��ʱ������λ���룬�������͡�
     *             ע�⣺����ʱ������deactivated ״̬ʱ����ֵʡ�ԣ�������š�
     *     <re-attempt_rat_indicator>: ��ϵͳ�Ƿ�������·���Ự�������̡��������ͣ�ȡֵ���£�
     *             0����ϵͳ���������·���Ự�������̣�
     *             1����ϵͳ�󣬲��������·���Ự�������̡�
     *     <re-attempt_eplmn_indicator>: eplmn���Ƿ���Է���Ự�������̡��������ͣ�ȡֵ���£�
     *             0: ����eplmn���Ƿ���Է���Ự��������;
     *             1: ������eplmn���Ƿ���Է���Ự�������̡�
     *     <procedure>: ָʾ�ܵ�T3585��ʱ��Ӱ��ĻỰ�������̡��������ͣ�ȡֵ���£�
     *             0: backoff��ʱ�������������̣�
     *             1:��ʱ������PDU_ESTABLISH���̣�
     *             2:��ʱ������PDP_MODIFY���̡�
     * [ʾ��]:
     *       ��ʱ�����ڿ���״̬���ϱ�����S-NSSAI��DNN��backoff��ʱ�������Ϣ
     *       +CSDBTSRI: ,"huawei.com",0,15,0,1,0
     *       ��ʱ������ֹͣ״̬ʱ���ϱ�����Ϣ
     *       +CSDBTSRI: ,"huawei.com",1
     */
    { AT_STRING_CSDBTSRI, (VOS_UINT8 *)"+CSDBTSRI: " },
#endif
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ECC״̬�����ϱ�
     * [˵��]: ���������������ϱ�ECC״̬��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ECCSTATUS: <ValidFlg>,<RxAntNum>,<TmMode>,<CorrQR1Data0011>,<CorrQR1Data01IQ>,<CorrQR2Data0011>,<CorrQR2Data01IQ>,<CorrQR3Data0011>,<CorrQR3Data01IQ>,<Rssi0>,<Rssi1>,<Rssi2>,<Rssi3>,<Rssi0>,<Rssi1>,<Rssi2>,<Rssi3><CR><LF>
     * [����]:
     *     <ValidFlg>: ����ֵ���޷����ַ��ͣ�
     *             1��������CC TM1ʱ������Ч��
     *             0����Ч���ݡ�
     *     <RxAntNum>: ����ֵ��UE������������
     *     <TmMode>: ����ֵ������ģʽ
     *     <CorrQR1Data0011>: ����ֵ��CorrQR1Data0011�Ĵ���ֵ��
     *     <CorrQR1Data01IQ>: ����ֵ��CorrQR1Data01IQ�Ĵ���ֵ��
     *     <CorrQR2Data0011>: ����ֵ��CorrQR2Data0011�Ĵ���ֵ����������Ϊ4ʱ��Ч��
     *     <CorrQR2Data01IQ>: ����ֵ��CorrQR2Data01IQ�Ĵ���ֵ����������Ϊ4ʱ��Ч��
     *     <CorrQR3Data0011>: ����ֵ��CorrQR3Data0011�Ĵ���ֵ����������Ϊ4ʱ��Ч��
     *     <CorrQR3Data01IQ>: ����ֵ��CorrQR3Data01IQ�Ĵ���ֵ����������Ϊ4ʱ��Ч��
     *     <RsrpR0>: ����ֵ, ����0 rsrp
     *     <RsrpR1>: ����ֵ, ����1 rsrp
     *     <RsrpR2>: ����ֵ, ����2 rsrp
     *     <RsrpR3>: ����ֵ, ����3 rsrp
     *     <Rssi0>: ����ֵ, ����0 rssi
     *     <Rssi1>: ����ֵ, ����1 rssi
     *     <Rssi2>: ����ֵ, ����2 rssi
     *     <Rssi3>: ����ֵ, ����3 rssi
     * [ʾ��]:
     *       �����ϱ�ECC״̬
     *       ^ECCSTATUS:
     *       1,2,0,483661589,723914177,0,0,0,0,-85,-86,-141,-141,-66,-68,-130,-130
     */
    { AT_STRING_ECC_STATUS, (VOS_UINT8 *)"^ECCSTATUS: " },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �������·���EPDU�����ϱ���AP
     * [˵��]: ���������ڽ�Modem�յ������·���ePDU���й㲥���ϲ�Ӧ�ã�GPSоƬ�ȣ���
     *         ��AT�����ϱ��������ƣ����÷�Ƭ�ķ�ʽ��ePDU������������ϱ����ϲ�Ӧ�á�
     *         �������޲�ѯ����������
     * [�﷨]:
     *     [����]: -
     *     [���]: ^EPDUR: <transaction_id>,<msg_type>,<common_info_valid_flg>,<end_flag>, <id>,[<name>], <total>,<index>,[<data>]
     * [����]:
     *     <transaction_id>: ePDU������Ӧ��Transaction Id���������ͣ�ȡֵ��ΧΪ0~65535��
     *     <msg_type>: ePDU��������Ϣ���ͣ��������ͣ�ȡֵ��Χ0~7��
     *             0������������
     *             1���ṩ������
     *             2�����������ݣ�
     *             3���ṩ�������ݣ�
     *             4������λ����Ϣ��
     *             5���ṩλ����Ϣ��
     *             6��ȡ����
     *             7������
     *     <common_info_valid_flg>: LPP CommonIEs��Ϣ�Ƿ���Ч��־λ���������ͣ�ȡֵ��Χ0~1��
     *             0����Ч��
     *             1����Ч��
     *     <end_flag>: ENDFLAG��־λ���������ͣ�ȡֵ��Χ0~1��
     *             0��ePDU���̽�����
     *             1�����к���ePDU���̡�
     *     <id>: ePDU Identifier��ePDU��ʶ���������ͣ�ȡֵ��Χ1~256��
     *     <name>: ePDU Name��ePDU���ƣ��ַ������ͣ����32���ַ���
     *     <total>: ePDU Body�����ܷ�Ƭ�������������ͣ�ȡֵ��Χ1~5��
     *     <index>: ePDU Body������Ƭ�������������ͣ�ȡֵ��Χ1~5��
     *     <data>: ePDU Body��Ƭ������ͨ��Half-Byte��16���ƣ�����ɵ��ַ�������󳤶�Ϊ1000���ַ���
     * [ʾ��]:
     *       �����ϱ���
     *       ^EPDUR: 1,2,0,0,121,"LPPE",1,1,0123456789ABCDEF0123456789ABCDEF
     */
    { AT_STRING_EPDUR, (VOS_UINT8 *)"^EPDUR: " },

    /*
     * [���]: Э��AT-LTE���
     * [����]: LTEС��CA״̬��Ϣ�����ϱ�
     * [˵��]: UE��פ��LTE����ʱ��SCell����״̬�仯��CA���/�ͷŵȳ���ͨ�������������ϱ�LTEС����CA״̬��Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LCACELLEX: <total_num >,<cell_index>,<ul_cfg>, <dl_cfg>, <act_flg>,<laa_flg>,<band>,<band_width>,<earfcn>,<CR><LF> [<CR><LF>^LCACELLEX: <total_num >,<cell_index>,<ul_cfg>,<dl_cfg>, <act_flg>,<laa_flg>, <band>,<band_width>,<earfcn>,<CR><LF>[��]]
     * [����]:
     *     <total_num>: ����ֵ�������ϱ�LTEС��������
     *     <cell_index>: ����ֵ��LTEС��������0��ʾPCell������ΪSCell��
     *     <ul_cfg>: ����ֵ����С������CA�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <dl_cfg>: ����ֵ����С������CA�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <act_flg>: ����ֵ����С�� CA�Ƿ񱻼��
     *             0��δ���
     *             1���Ѽ��
     *     <laa_flg>: ����ֵ����С���Ƿ�ΪLaaС����
     *             0������LaaС����
     *             1����LaaС����
     *     <band>: ����ֵ����С����BandƵ�Σ���7����Band VII��
     *     <band_width>: ����ֵ����С��ռ�ô���
     *             0������Ϊ1.4MHz��
     *             1������Ϊ3MHz��
     *             2������Ϊ5MHz��
     *             3������Ϊ10MHz��
     *             4������Ϊ15MHz��
     *             5������Ϊ20MHz��
     *     <earfcn>: ����ֵ����С��Ƶ�㡣
     * [ʾ��]:
     *       LTE������SCell����״̬�仯��CA���/�ͷŵȳ��������ϱ�С��CA״̬��Ϣ
     *       ^LCACELLEX: 2,0,1,1,1,0,7,2,21120
     *       ^LCACELLEX: 2,1,1,1,1,1,34,1,36230
     */
    { AT_STRING_LCACELLEX, (VOS_UINT8 *)"^LCACELLEX: " },
    { AT_STRING_VT_FLOW_RPT, (VOS_UINT8 *)"^VTFLOWRPT: " },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ģʽVoWiFi�£���ͨ���������380�����ϱ�
     * [˵��]: ��������MODEM����ģʽʱ��VoWiFiע��ɹ���ͨ��VoWiFi������ͨ���У������380����ʱ�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CALLALTSRV<CR><LF>
     * [����]:
     * [ʾ��]:
     *       �����ϱ���
     *       ^CALLALTSRV
     */
    { AT_STRING_CALL_ALT_SRV, (VOS_UINT8 *)"^CALLALTSRV" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: GPS Fine Time�����ϱ�
     * [˵��]: GPSоƬ�·�^FINETIMEREQ����󴥷�GPS Fine Time�ϱ����̣�UEͨ���������ϱ���ӦSYNCʱ�������GPS Fine Time��Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^FINETIMEINFO: <rat>,<ta>,<sinr>,<state>,<fine_time>, <fine_time_offset>[, <leap_sec>]<CR><LF>
     * [����]:
     *     <rat>: �������ͣ��ṩFine Time�Ľ��뼼����
     *             0��GSM���ݲ�֧�֣���
     *             1��WCDMA���ݲ�֧�֣���
     *             2��LTE��
     *             3��TDS-CDMA���ݲ�֧�֣���
     *             4��CDMA���ݲ�֧�֣���
     *             5��HRPD���ݲ�֧�֣���
     *             6��NR��
     *     <ta>: �������ͣ���������õ�ʱ����ǰ����
     *     <sinr>: �������ͣ���ǰ����ȡ�
     *     <state>: �������ͣ���Ӧ���뼼���������״̬��
     *             0��IDLE̬��
     *             1��CONNECTED̬��
     *             2��INACTIVE̬����NRģʽ�£���
     *             3������״̬��
     *     <fine_time>: 16���Ƴ��������ͣ�
     *             LTEģʽʱ������UTCʱ�䣬��λ10ms��
     *     <fine_time_offset>: 16���Ƴ��������ͣ�
     *             LTEģʽʱ������UTCʱ��ƫ�ƣ���λ10ms��
     *     <leap_sec>: �������ͣ�UTCʱ����GPSʱ������롣
     * [ʾ��]:
     *       LTE���������̬�ϱ�GPS Fine Time��Ϣ
     *       ^FINETIMEINFO: 2,1,1,1,41478472,683957289,16
     */
    { AT_STRING_FINETIMEINFO, (VOS_UINT8 *)"^FINETIMEINFO: " },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: LTEģʽSFN�ϱ�
     * [˵��]: GPSоƬ�·�^FINETIMEREQ����󴥷�SFN֡�������̣�UEͨ���������ϱ���ǰLTEģʽ��SFN֡�ţ���ͬʱ����һ��SYNCʱ�����塣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SFN: <num><CR><LF>
     * [����]:
     *     <num>: �������ͣ�LTEģʽ��SFN֡�š�
     * [ʾ��]:
     *       LTEģʽ���ϱ�SFN֡��
     *       ^SFN: 12850
     */
    { AT_STRING_SFN, (VOS_UINT8 *)"^SFN: " },
    /*
     * [���]: Э��AT-�绰����
     * [����]: Fine Time�쳣�ϱ�
     * [˵��]: ����^FINETIMEREQ����GPS Fine Time/SFN�����쳣ʱ�ϱ�������֪ͨGPSоƬ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^FINETIMEFAIL: <cause><CR><LF>
     * [����]:
     *     <cause>: ����ֵ������GPS Fine Time/SFN�����쳣������
     *             1��ǿ�ƴ��������GPS Fine Time����ʧ�ܣ�
     *             2������SFN֡����ʧ�ܡ�
     * [ʾ��]:
     *       ����GPS Fine Time�����쳣��
     *       ^FINETIMEFAIL: 1
     */
    { AT_STRING_FINETIMEFAIL, (VOS_UINT8 *)"^FINETIMEFAIL: " },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ȡAP��LTE��UE�������ݲ���
     * [˵��]: ����������LRRC�����ϱ�UE��ǰ�Ĳ�����Ϣ��AP�࣬��ȡAP�ඨ��LTE��UE����������Ϣ����^LRRCUECAPPARAMSET�������ʹ�á�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LRRCUECAPINFONTF: <Len>,<Param><CR><LF>
     * [����]:
     *     <Len>: ���ͣ�<Param>�ĳ���
     *     <Param>: MODEM�ϱ��Ĳ�����Ϣ���������Ȳ�����4800�ֽڡ�
     * [ʾ��]:
     *       �����ϱ�UE�Ĳ�����Ϣ��
     *       ^LRRCUECAPINFONTF: 8,"12345678"
     */
    { AT_STRING_LRRCUECAPINFONTF, (VOS_UINT8 *)"^LRRCUECAPINFONTF: " },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����ACKͨ������
     * [˵��]: ����������֧�������ACK�����ϱ�ͨ������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^PHYCOMACK: <ack type>,<value1>,<value2>,<value3><CR><LF>
     * [����]:
     *     <ack type>: ����ʮ���� ��Χ��0-4294967295��
     *             ACK����
     *     <value1>: ����ʮ���� ��Χ��0-4294967295��
     *     <value2>: ����ʮ���� ��Χ��0-4294967295��
     *     <value3>: ����ʮ���� ��Χ��0-4294967295��
     * [ʾ��]:
     *       ������ϱ�ACK������ack type��value1��value2��value3ֵ��Ϊ1��
     *       ^PHYCOMACK: 1,1,1,1
     */
    { AT_STRING_PHYCOMACK, (VOS_UINT8 *)"^PHYCOMACK: " },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ײ����ͨ�õ�BOOSTER�����ϱ�����
     * [˵��]: �ײ����ͨ�õ�BOOSTER�����ϱ����^BOOSTERNTF��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^BOOSTERNTF: <MODULD_ID>,<LEN>,<PARAM><CR><LF>
     * [����]:
     *     <MODULEID>: ��Ϣ������������Σ�
     *             0��LRRC��
     *             1��LPHY��
     *             2��MMC��
     *             3��BASTET��
     *     <LEN>: <PARAM>�ĳ���
     *     <PARAM>: �ַ�����TLV��ʽ�Ĳ�����T��L��2���ֽڣ����嶨���ɸ����ά��������ʧ�ܵĴ���ԭ��ֵҲ�ɸ����������ά��
     * [ʾ��]:
     *       LRRC��BOOSTER�����ϱ�����
     *       ^BOOSTERNTF: 0,4,"01021236"
     */
    { AT_STRING_BOOSTERNTF, (VOS_UINT8 *)"^BOOSTERNTF: " },

    { AT_STRING_MTREATTACH, (VOS_UINT8 *)"^MTREATTACH" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IP��ý�幫����ʶ���������ϱ�
     * [˵��]: ��������IP��ý�幫����ʶ���������ϱ���Ӧ�á�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMPU: <type><CR><LF>
     * [����]:
     *     <type>: ����ֵ��ȡֵ��ΧΪ(0,1)��
     *             0��MSISDN���ͣ�
     *             1��IMSI���͡�
     * [ʾ��]:
     *       IP��ý�幫����ʶ���������ϱ�
     *       ^IMPU: 1
     */
    { AT_STRING_IMPU, (VOS_UINT8 *)"^IMPU: " },

#if (FEATURE_DSDS == FEATURE_ON)
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: DSDS״̬�����ϱ�
     * [˵��]: ����������Modem��ǰ��DSDS״̬�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: ^DSDSSTATE: <status>
     * [����]:
     *     <status>: ����ֵ��Modem��DSDS״̬��
     *             0��DSDS2.0��
     *             1��DSDS3.0��
     * [ʾ��]:
     *       DSDS�ϱ�DSDS3.0��״̬
     *       ^DSDSSTATE: 1
     */
    { AT_STRING_DSDSSTATE, (VOS_UINT8 *)"^DSDSSTATE: " },
#endif

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����±������ϱ�
     * [˵��]: �����������ϱ�оƬ�¶ȱ仯����0�Ȼ����0��ʱ�±��¼���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ TEMPPROTECT:<n><CR><LF>
     * [����]:
     *     <n>: ����ֵ��
     *             0���±�����
     *             1����Ҫ�����±�
     * [ʾ��]:
     *       �ϱ��±��¼�
     *       ^TEMPPROTECT: 0
     */
    { AT_STRING_TEMPPROTECT, (VOS_UINT8 *)"^TEMPPROTECT: " },

    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: RTT�¼��ϱ�
     * [˵��]: ����������RTT�¼��ϱ����ϱ��������£�
     *         Voice���У�RTT������Text�������ɹ���
     *         Voiceͨ���У�Զ�˷���RTT��������
     *         Voiceͨ���У�Զ�˷���RTT������Text�������ɹ���
     *         Voiceͨ���У����˷���RTT������Text�������ɹ���
     *         RTTͨ���У�Զ��RTT������Text��Close�ɹ���
     *         RTTͨ���У�����RTT������Text��Close�ɹ���
     *         RTT�����������ǰ��CPSֵ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^RTTEVT: <call_id>,<event>,<reason><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ�����б�ʶ��ȡֵ��Χ1~7��
     *     <event>: ����ֵ��RTT�¼����ͣ�
     *             0��Add Textý������
     *             1��Close Textý������
     *             2��Զ������Add Textý������
     *             3��RTT���������
     *     <reason>: ����ֵ��RTT�¼�ԭ��
     *             0��Local User Trigger��
     *             1��Remote User Trigger��
     * [ʾ��]:
     *       Voiceͨ���У����˷���RTT������Text�������ɹ�
     *       ^RTTEVT: 1,1,0
     */
    { AT_STRINT_RTTEVENT, (VOS_UINT8 *)"^RTTEVT" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: RTT�쳣�����ϱ�
     * [˵��]: ����������RTT�����¼��ϱ����ϱ��������£�
     *         RTT���У�Text������ʧ�ܣ�
     *         Voice���У�RTT������Text������ʧ�ܣ�
     *         Voiceͨ���У����˷���RTT������Text������ʧ�ܣ�
     *         RTTͨ���У����˷���RTT������Text��Closeʧ�ܡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^RTTERR: <call_id>,<operation>,<cause>, <reason_text><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ�����б�ʶ��ȡֵ��Χ1~7��
     *     <operation>: ����ֵ���쳣���¼���Ӧ�Ĳ������ͣ�
     *             0��Add Textý������
     *             1��Close Textý������
     *     <cause>: ����ֵ���쳣��Ӧ��ΪSipStatusCode����Q850��ȱʡԭ��Ϊ-1��
     *             1~127��Q850����Ӧcs��ԭ��
     *             400~699��Sip��Ӧ�롣
     *     <reason_text>: �ַ������ͣ��쳣��Ӧԭ���ı���Ϣ��ȱʡԭ��Ϊ��unknown����
     * [ʾ��]:
     *       Voiceͨ���У����˷���RTT������Text������ʧ��
     *       ^RTTERR: 1,0,-1,��unknown��
     */
    { AT_STRINT_RTTERROR, (VOS_UINT8 *)"^RTTERR" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: RSSIԤ���ϱ�����
     * [˵��]: ��RSSI�仯������ֵʱ��MT�����ϱ���ָʾ��TE��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^EMRSSIRPT: <rat>,<rssi><CR><LF>
     * [����]:
     *     <rat>: ����ֵ��������ʽ��
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     *     <rssi>: ����ֵ�������ź�ǿ��ָʾ��
     *             0��rssi <-120 dBm
     *             1��-120 dBm ?0?5 rssi <-119 dBm
     *             2��-119 dBm ?0?5 rssi <-118 dBm
     *             :   :::
     *             94��-27 dBm ?0?5 rssi <-26 dBm
     *             95��-26 dBm ?0?5 rssi <-25 dBm
     *             96��-25 dBm ?0?5 rssi
     *             255��δ֪�򲻿ɲ�
     * [ʾ��]:
     *       LTE����С��RSSI�仯������ֵ
     *       ^EMRSSIRPT: 2,30
     */
    { AT_STRING_EMRSSIRPT, (VOS_UINT8 *)"^EMRSSIRPT: " },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ģʽ״̬�ϱ�
     * [˵��]: ���������ڵ���ģʽ״̬�����仯ʱ��������ģʽ״̬�͵�ǰ�������״̬�ϱ���AP�����ں���ָʾAP�·�ָ����ʷ��������ٻ�LTE��
     *         ע�������ǰ���ò�֧��LTE��SYSCFG���ã����޿�������Ч���򲻻��ϱ�����Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF> ^ELEVATOR: <sensor state>,<service state><sensor scene><CR><LF>
     * [����]:
     *     <sensor state>: ����ֵ�����ݻ򳵿�״̬��
     *             0�������ݻ������״̬��
     *             1�������ݻ������״̬��
     *     <Service state>: ����ֵ����ǰ�������״̬��
     *             0���޷���
     *             1���з���
     *     <Sensor scene>: ����ֵ��Sensorָʾ��״̬�仯������
     *             0�����ݳ�����
     *             1�����ⳡ����
     * [ʾ��]:
     *       SENSOR��⵱ǰ������ݣ��ҵ�ǰ��������״̬��
     *       ^ELEVATOR: 1,1,0
     *       SENSOR��⵱ǰ�����⣬�ҵ�ǰ�޷���״̬��
     *       ^ELEVATOR: 0,0,1
     */
    { AT_STRING_ELEVATOR, (VOS_UINT8 *)"^ELEVATOR" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����С������Ƶ���ϱ�
     * [˵��]: ���������������ϱ���LTE/WCDMA/HRPDƵ�ʱ仯ʱ�ķ���С������Ƶ����Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ULFREQRPT: <rat>,<ulfreq>,<ulbw><CR><LF>
     * [����]:
     *     <rat>: ����ֵ�����뼼����
     *             0���޷���
     *             1��WCDMA
     *             2��HRPD
     *             3��LTE
     *     <ulfreq>: ����ֵ������С������Ƶ�ʡ���λ100kHz��
     *     <ulbw>: ����ֵ������С�����д�����λ1kHz��
     * [ʾ��]:
     *       �����ϱ�����С������Ƶ��
     *       ^ULFREQRPT: 1,19200,5000
     */
    { AT_STRING_ULFREQRPT, (VOS_UINT8 *)"^ULFREQRPT" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: α��վ���������ϱ�
     * [˵��]: ���������ڽ����ʶ��α��վ���������ʱ�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: ^PSEUDBTS: <RAT>
     * [����]:
     *     <RAT>: 1:GSM
     *             2:WCDMA���ݲ�֧�֣�
     *             3:LTE
     * [ʾ��]:
     *       2Gα��վ���������ϱ�
     *       ^PSEUDBTS: 1
     */
    { AT_STRING_PSEUDBTS, (VOS_UINT8 *)"^PSEUDBTS" },
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    /*
     * [���]: Э��AT-�绰����
     * [����]: 5GCע��״̬�����ϱ�
     * [˵��]: ���ն�֧��NR��������
     *         ��n=2ʱ������ն���5G��ע�ᣬ��������ϱ�������С������Ƭ��Ϣ
     *         +C5GREG: <stat>,<tac>,<ci>,<AcT>,<Allowed_NSSAI_length>[,<Allowed_NSSAI>]
     *         ���򣬴����������ϱ�PS���ע����+C5GREG: <stat>��
     *         ��AT��������������£�
     *         ��������ڵ���֧��NRʱ����ʹ�ã�����ǰ����֧��NR��������
     * [�﷨]:
     *     [����]: -
     *     [���]: ����PS��ע��״̬�����ı�ʱ
     *             <CR><LF>+C5GREG: <stat><CR><LF>
     *     [����]: -
     *     [���]: С����Ϣ�����ı����Allowed NSSAI�����仯ʱ��
     *             <CR><LF>+C5GREG: <stat>[,[<tac>],[<ci>],[<AcT>],[<Allowed_NSSAI_length>],[<Allowed_NSSAI>]]<CR><LF>
     * [����]:
     *     <n>: ����ֵ��Ĭ��ֵΪ0��
     *             0����ֹ+C5GREG�������ϱ���
     *             1��ʹ��+C5GREG: <stat>�������ϱ���
     *             2��ʹ��+C5GREG: <stat>[,[<tac>],[<ci>],[<AcT>],[<Allowed_NSSAI_length>],[<Allowed_NSSAI>] ]�������ϱ���
     *     <stat>: 0��û��ע�ᣬMT���ڲ�û������ѰҪע����µ���Ӫ�̣�
     *             1��ע���˱������磻
     *             2��û��ע�ᣬ��MT������ѰҪע����µ���Ӫ�̣�
     *             3��ע�ᱻ�ܾ���
     *             4��δ֪ԭ��
     *             5��ע�����������硣
     *             8��registered for emergency services only��
     *     <tac>: λ������Ϣ�������ַ���16���Ʊ�ʾ����������0000C3����10���Ƶ�195��
     *     <ci>: С����Ϣ��8���ֽڣ�16���ַ���16���Ʊ�ʾ��ʹ��Half-Byte�루���ֽڣ����룬����000000010000001A����ʾ��4�ֽ���0x00000001����4�ֽ���0x0000001A��
     *     <AcT>: ����ֵ����ǰ����Ľ��뼼����
     *             10��EUTRAN-5GC
     *             11��NR-5GC
     *     <Allowed_NSSAI_length>: ����ֵ����ʾ<Allowed_NSSAI>��Ϣ����Ƭ�ֽڸ�����
     *             ��<Allowed_NSSAI>��ϢʱֵΪ0��
     *     <Allowed_NSSAI>: �ַ������͡�ÿ��S-NSSAIʹ�á�:�����֡�����S-NSSAI�ĸ�ʽ
     *             "sst" if only slice/service type (SST) is present
     *             "sst;mapped_sst" if SST and mapped configured SST are present
     *             "sst.sd" if SST and slice differentiator (SD) are present
     *             "sst.sd;mapped_sst" if SST, SD and mapped configured SST are present
     *             "sst.sd;mapped_sst.mapped_sd" if SST, SD, mapped configured SST and mapped configured SD are present
     *             ע�⣺�ַ����е�����Ϊ16���ƣ��ַ�ȡֵ��Χ��0-9��A-F��a-f��
     *             ��Allowed_NSSAI�б���ϢʱֵΪ�ա�
     * [ʾ��]:
     *       ����ע���״̬�����ı�ʱ
     *       +C5GREG: 1
     *       ע����5G��С����Ϣ�����ı�ʱ
     *       +C5GREG: 1,"0000C3","000000010000001A",11,9,"01:01.123456;2F.654321"
     */
    { AT_STRING_C5GREG, (TAF_UINT8 *)"+C5GREG: " },
    { AT_STRING_CSERSSI, (VOS_UINT8 *)"^CSERSSI:" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��ȡAP��NR��UE�������ݲ���
     * [˵��]: ����������NRRC�����ϱ�UE��ǰ�Ĳ�����Ϣ��AP�࣬��ȡAP�ඨ��NR��UE����������Ϣ����^NRRCUECAPPARAMSET�������ʹ�á�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^NRRCUECAPINFONTF: <Len>,<Param><CR><LF>
     * [����]:
     *     <Len>: ���ͣ�<PARAM>�ĳ���
     *     <Param>: MODEM�ϱ��Ĳ�����Ϣ���������Ȳ�����4800�ֽڡ�
     * [ʾ��]:
     *       �����ϱ�UE�Ĳ�����Ϣ��
     *       ^NRRCUECAPINFONTF: 8,"12345678"
     */
    { AT_STRING_NRRCUECAPINFONTF, (VOS_UINT8 *)"^NRRCUECAPINFONTF: " },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR CAС����Ϣ�����ϱ�
     * [˵��]: UE��פ��NR��LTE NSA����ʱ��CA��Ϣ�仯�����ϱ�NR CA��Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^NRCACELLRPT: <total_num>[,<cell_index>,<dl_cfg>, <nul_cfg>,<sul_cfg>[,<cell_index>,<dl_cfg>,<nul_cfg>,<sul_cfg> <[��]]] <CR><LF>
     * [����]:
     *     <total_num>: ����ֵ�������ϱ���NRС������
     *     <cell_index>: ����ֵ��NRС��������0��ʾPCell������ΪPSCell��Scell��
     *     <dl_cfg>: ����ֵ����С�������Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <nul_cfg>: ����ֵ����С������Nul�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     *     <sul_flg>: ����ֵ����С������Sul�Ƿ����ã�
     *             0��δ���ã�
     *             1�������á�
     * [ʾ��]:
     *       NR CA��Ϣ�����ϱ�
     *       ^NRCACELLRPT: 2,0,1,1,1,1,0,1,1
     *       NR CAС��������Ϊ0ʱ�����ϱ�
     *       ^NRCACELLRPT: 0
     */
    { AT_STRING_NRCACELLRPT, (VOS_UINT8 *)"^NRCACELLRPT" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: NR�������������ϱ�����
     * [˵��]: UE�����ϱ�NRĳ���������������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^NRNWCAP:  <type>,<para1>,<para2>,<para3>,<para4>,<para5>,<para6>,<para7>,<para8>,<para9>,<para10><CR><LF>
     * [����]:
     *     <type>: ����ֵ��ȡֵ0��255��
     *             1������SA��������Ӧ����<para1>������Ч
     *             �����������ر��壬Ԥ��ʹ��
     *     <para1>: ����ֵ����ӦNR���������ĵ�һ�����
     *             <type>Ϊ1ʱ��ȡֵ��Χ:
     *             0�����粻֧��SA;
     *             1������֧��SA��
     *     <para2>: ����ֵ����ӦNR���������ĵڶ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para3>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para4>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para5>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para6>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para7>: ����ֵ����ӦNR���������ĵ��������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para8>: ����ֵ����ӦNR���������ĵڰ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para9>: ����ֵ����ӦNR���������ĵھ������
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     *     <para10>: ����ֵ����ӦNR���������ĵ�ʮ�����
     *             Ĭ��Ϊ0�������ر��壬������֧�ָ���NR��������ʱʹ��
     * [ʾ��]:
     *       �����ϱ������SA����֧��
     *       ^NRNWCAP: 1,1,0,0,0,0,0,0,0,0,0
     */
    { AT_STRING_NRNWCAP, (VOS_UINT8 *)"^NRNWCAP" },
#endif
    /*
     * [���]: Э��AT-NDIS����
     * [����]: ����״̬�����ϱ�
     * [˵��]: ���豸������״̬�����仯ʱ��MT�����ϱ���ָʾ��TE��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^NDISSTAT: <stat>,[<err>],[<wx_state>],<PDP_type><CR ><LF>
     * [����]:
     *     <stat>: ����ֵ������״̬��
     *             0�����ӶϿ���
     *             1�������ӣ�
     *             2�������У����ڵ����Զ�����ʱ�ϱ����ݲ�֧�֣���
     *             3���Ͽ��У����ڵ����Զ��Ͽ�ʱ�ϱ����ݲ�֧�֣���
     *     <err>: ����ֵ�������룬���ڲ���ʧ��ʱ�ϱ��ò�����
     *             0��unknow error/ unspecified error��
     *             ����ֵ����ѭ��3GPP TS 24.008 V5.5.0 (2002-09)���������汾��10.5.6.6SM Cause������
     *     <wx_state>: ����ֵ��WiMax���ݿ���״̬���ݲ�֧�֡�
     *             1��DL Synchronization��
     *             2��Handover DL acquisition��
     *             3��UL Acquisition��
     *             4��Ranging��
     *             5��Handover ranging��
     *             6��Capabilities negotiation��
     *             7��Authorization��
     *             8��Registration��
     *     <PDP_type>: ȡֵΪ�ַ�����
     *             "IPV4"
     *             "IPV6"
     *             "Ethernet"
     * [ʾ��]:
     *       NDIS�豸�ӶϿ�����״̬��Ϊ������״̬
     *       ^NDISSTAT: 1,,,"IPV6"
     *       NDIS�豸��������״̬��Ϊ�Ͽ�����״̬
     *       ^NDISSTAT: 0,,,"IPV4"
     *       NDIS����ΪIPV4V6���ͣ�����ʧ�ܣ�SM causeΪ��27��
     *       ^NDISSTAT: 0,27,,"IPV4"
     *       ^NDISSTAT: 0,27,,"IPV6"
     */
    { AT_STRING_NDISSTAT, (VOS_UINT8 *)"^NDISSTAT" },
    /*
     * [���]: Э��AT-NDIS����
     * [����]: ����״̬�����ϱ���չ����
     * [˵��]: ���豸������״̬�����仯ʱ��MT�����ϱ���ָʾ��TE��ֻ����E5��̬APP�˿ڲ����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^NDISSTATEX: <cid>,<stat>,[<err>],[<wx_state>],<PDP_type><CR><LF>
     * [����]:
     *     <cid>: cid��Ϣ
     *     <stat>: ����ֵ������״̬��
     *             0�����ӶϿ���
     *             1�������ӣ�
     *             2�������У����ڵ����Զ�����ʱ�ϱ����ݲ�֧�֣���
     *             3���Ͽ��У����ڵ����Զ��Ͽ�ʱ�ϱ����ݲ�֧�֣���
     *     <err>: ����ֵ�������룬���ڲ���ʧ��ʱ�ϱ��ò�����
     *             0��unknow error/ unspecified error��
     *             ����ֵ����ѭ��3GPP TS 24.008 V5.5.0 (2002-09)���������汾��10.5.6.6SM Cause������
     *     <wx_state>: ����ֵ��WiMax���ݿ���״̬���ݲ�֧�֡�
     *             1��DL Synchronization��
     *             2��Handover DL acquisition��
     *             3��UL Acquisition��
     *             4��Ranging��
     *             5��Handover ranging��
     *             6��Capabilities negotiation��
     *             7��Authorization��
     *             8��Registration��
     *     <PDP_type>: ȡֵΪ�ַ�����
     *             "IPV4"
     *             "IPV6"
     *             "Ethernet"
     * [ʾ��]:
     *       �豸�ӶϿ�����״̬��Ϊ������״̬
     *       ^NDISSTATEX: 1,1,,,"IPV6"
     *       �豸��������״̬��Ϊ�Ͽ�����״̬
     *       ^NDISSTAT: 1,0,,,"IPV4"
     *       ����ΪIPV4V6���ͣ�����ʧ�ܣ�SM causeΪ��27��
     *       ^NDISSTATEX: 1,0,27,,"IPV4"
     *       ^NDISSTATEX: 1,0,27,,"IPV6"
     *       ����ΪEthernet���ͣ�����ȥ���SM causeΪ��36��
     *       ^NDISSTATEX: 1,0,36,,"Ethernet"
     */
    { AT_STRING_NDISSTATEX, (VOS_UINT8 *)"^NDISSTATEX" },

    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �ź�ǿ��̽���������ϱ�����
     * [˵��]: �����ǰ��Ҫ����AP�·�������̽�������̽��������ʱ��ͨ���������������ϱ��ķ�ʽ֪ͨAPָ��PLMN������ź�ǿ�ȡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^DETECTPLMN: <plmn>,<rat>,<rsrp><CR><LF>
     * [����]:
     *     <plmn>: �������ͣ�Ҫ������PLMN��Ϣ��
     *     <rat>: ����ֵ�����߽��뼼��(��ǰֻ֧��LTE)��
     *             0��GSM/GPRS��
     *             2��WCDMA��
     *             7��LTE��
     *     <rsrp>: ��LTE��
     *             <rsrp>��ȡֵ��Χ<-141,-44>��0Ϊ��Чֵ������ʧ�ܣ�����Чֵ0����
     * [ʾ��]:
     *     �� AP�·�ָ��PLMN+RAT̽�������̽��ɹ��������ϱ�AP̽������
     *       AT^DETECTPLMN=46085,7
     *       OK
     *       ^DETECTPLMN: 46085,7,-100
     *     �� AP�·�ָ��PLMN+RAT̽��������������㷵��̽����������̽����̱�������ϣ������ϱ�AP̽������
     *       AT^DETECTPLMN=46085,7
     *       OK
     *       ^DETECTPLMN: 46085,7,0
     *       ע���ϱ���PLMN��RATΪAP̽��������Я����PLMN��RAT���Թ�AP���ָô��ϱ��Ƿ�Ϊ֮ǰ��ѯ���������������̽��ɹ�����͸������㷵�ص��ź�ǿ�ȣ��κ�ԭ�������̽��ʧ�ܣ��ϱ��ź�ǿ�Ⱦ�Ϊ0��
     */
    { AT_STRING_DETECTPLMN, (VOS_UINT8 *)"^DETECTPLMN" },

    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����ָʾ�ϱ�������չģʽ��
     * [˵��]: ����ָʾ�����緢���PDP���������ϱ�������չģʽ����
     *         ���ʹ�÷���չģʽ���ϱ������û��޷�����RING��ʾPS��Packet Switched domain������CS��Circuit Switched domain���򡣽���ʹ����չģʽ������ָʾ�ϱ�+CRING��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>RING<CR><LF>
     * [����]:
     * [ʾ��]:
     *       ������ʱ������+CLIP=1
     *       RING
     *       ��PDP����ָʾʱ
     *       RING
     */
    { AT_STRING_RING, (VOS_UINT8 *)"RING" },
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: ����ָʾ�ϱ�����չģʽ��
     * [˵��]: ����ָʾ�����緢���PDP���������ϱ�����չģʽ����
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CRING: <type><CR><LF>
     * [����]:
     *     <type>: ҵ�����͡�
     *             VOICE����ͨ�������У�
     *             SYNC����·��64kbit/s����ҵ����У�ͬ��͸��ģʽ����
     *             GPRS <PDP_type>,<PDP_addr>[,[<L2P>][,<APN>]]�����緢���PDP����ָʾ��
     *     <PDP_type>: �ַ������ͣ���������Э�����͡�
     *             IP������Э�飨IPV4����
     *             IPV6��IPV6Э�顣
     *             IPV4V6��IPV4��IPV6��
     *     <PDP_address>: �ַ������ͣ�IPV4���͵�PDP��ַ��
     *     <L2P>: �ַ������ͣ���ʶTE��MT֮��ʹ�õĲ����Э�飬�ò��������ԡ�
     *     <APN>: �ַ������ͣ���������ƣ�����ѡ��GGSN���ⲿ�����������磻����ֵΪ�գ���ʹ��ǩԼֵ��
     * [ʾ��]:
     *       ������ʱ������+CLIP=1
     *       +CRING: VOICE
     *       +CLIP: "13900000000",129
     *       ��PDP����ָʾʱ
     *       +CRING: GPRS "IP","10.161.91.51",,"abc.com"
     */
    { AT_STRING_CRING, (VOS_UINT8 *)"+CRING" },
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: IMS������ָʾ�ϱ�
     * [˵��]: IMS������ָʾ���ܷ������ϱ���+CLIP������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>IRING<CR><LF>
     * [����]:
     * [ʾ��]:
     *       ��IMS������ʱ������+CLIP=1
     *       IRING
     */
    { AT_STRING_IRING, (VOS_UINT8 *)"IRING" },
    /*
     * [���]: Э��AT-ͨ�ò���
     * [����]: USIMM�ڽ��յ�����ص��Ĳ�ο���ָʾ��ͨ��AT���������ϱ���Ӧ��
     * [˵��]: ������Ϊ�����ϱ������֧�ֲ�ѯ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SIMHOTPLUG: <status><CR><LF>
     * [����]:
     *     <status>: Status״̬��
     *             0��������OUT״̬��
     *             1��������IN״̬��
     * [ʾ��]:
     *       ����λ״̬�����ϱ�
     *       ^SIMHOTPLUG: 1
     */
    { AT_STRING_SIMHOTPLUG, (VOS_UINT8 *)"^SIMHOTPLUG" },
    /*
     * [���]: Э��AT-�绰����
     * [����]: IMS URSP�汾�ϱ�
     * [˵��]: ����������AP�ཫIMS��ص�URSP������Ϣ���͸�Modem�����Modem���������쳣�����ϱ���ǰ֧�ֵİ汾��Ϣ
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSURSPVERISON: <version><CR><LF>
     * [����]:
     *     <version>: AP��IMSAԼ�������������汾����ǰ��֧��ȡֵ1
     * [ʾ��]:
     *       ����IMS URSP Version��Ϣ�ϱ���
     *       ^IMSURSPVERSION: 1
     */
    { AT_STRING_IMSURSPVERSION, (VOS_UINT8 *)"^IMSURSPVERSION" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���з���ָʾ
     * [˵��]: �����ϱ�������û������·�����ʱ��֪ͨ�û�UE���������緢����С�
     *         �����������ϱ�ͨ��Ϊ�û��������ʱʹ�õ�ATͨ�����������ϱ�������^CURC������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ORIG: <call_id>,<call_type><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *     <call_type>: ����ֵ���������͡�
     *             0���������У�
     *             1����Ƶͨ������������Ƶ��˫��������
     *             2����Ƶͨ�������������Ƶ��˫��������
     *             3����Ƶͨ����˫����Ƶ��˫��������
     *             9���������С�
     * [ʾ��]:
     *       UE�����緢��һ����������
     *       ^ORIG: 1,0
     */
    { AT_STRING_ORIG, (VOS_UINT8 *)"^ORIG" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �����ָͨʾ
     * [˵��]: �����ϱ����֪ͨ�û�������к�UE�յ��������ָͨʾ��
     *         �����������ϱ�ͨ��Ϊ�û��������ʱʹ�õ�ATͨ�����������ϱ�������^CURC������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CONF: <call_id><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     * [ʾ��]:
     *       UE�յ������ָͨʾ
     *       ^CONF: 1
     */
    { AT_STRING_CONF, (VOS_UINT8 *)"^CONF" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ���н�ָͨʾ
     * [˵��]: �����ϱ���������н�ͨ��UE֪ͨ�û������Ѿ���ͨ��
     *         �������ϱ�������^CURC������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CONN: <call_id>,<call_type><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *     <call_type>: ����ֵ���������͡�
     *             0���������У�
     *             3����Ƶ���У�
     *             9����������
     * [ʾ��]:
     *       ���������Ѿ���ͨ
     *       ^CONN: 1,0
     */
    { AT_STRING_CONN, (VOS_UINT8 *)"^CONN" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ͨ������ָʾ
     * [˵��]: �����ϱ������ͨ��������UE֪ͨ�û�ͨ������ԭ���ͨ��ʱ����
     *         �û���������ͨ�������ϱ����û��·�����ͨ�������ͨ�����������ͨ�������ϱ����������е�ͨ���ϡ��������ϱ�������^CURC������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CEND: <call_id>, <duration>,<no_cli_cause>, <cc_cause><CR><LF>
     * [����]:
     *     <call_id>: ����ֵ������ID��ȡֵ��Χ1~7��
     *     <duration>: ����ֵ��ͨ��ʱ������λs��
     *     <no_cli_cause>: ����ֵ�����������ԭ��ֵ��
     *             0�����벻���ã�
     *             1���û��ܾ��ṩ���룻
     *             2������������Ӱ�죻
     *             3�����ѵ绰��
     *             4����ʼֵ(����Ҫ��ע)��
     *     <cc_cause>: ����ֵ�����д���ԭ��ֵ��
     *             ��4.5�½���������Ӧ��
     * [ʾ��]:
     *       ͨ������
     *       ^CEND: 1,10,0,0
     */
    { AT_STRING_CEND, (VOS_UINT8 *)"^CEND" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ת��IMSA��MAPCON����Ϣ
     * [˵��]: ת��IMSA��MAPCON����Ϣ
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSCTRLMSGU: <msg_id>,<msg_len>,<msg_context><CR><LF>
     * [����]:
     *     <msg_id>: ��Ӧ����ģ��
     *             0��IMSA
     *             1��MAPCONN
     *     <msg_len>: ��Ϣ����<msg_context>�ĳ���
     *     <msg_context>: ģ���ӿ���Ϣ����
     * [ʾ��]:
     *       ת��IMSA��MAPCON����Ϣ
     *       ^IMSCTRLMSGU: 1,4,"12345678"
     */
    { AT_STRING_IMSCTRLMSGU, (VOS_UINT8 *)"^IMSCTRLMSGU" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: �����ϱ�IMS����״̬
     * [˵��]: ����������IMS�򱻽д���״̬��Ϣ�����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSMTRPT: < number >,<status>,< cause><CR><LF>
     * [����]:
     *     <number>: �����������
     *     <status>: 0���յ����У���ʼ�����й���
     *             1�������쳣��δ�յ�RING�ͽ���
     *     <cause>: �ο�CS��������½���IMSA�Ĵ���ԭ��ֵ
     * [ʾ��]:
     *       IMSA��ʼ�����������ϱ�
     *       ^IMSMTRPT: 18910963686,0,0
     */
    { AT_STRING_IMSMTRPT, (VOS_UINT8 *)"^IMSMTRPT" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: ECALL�Ự״̬�����ϱ�
     * [˵��]: �����ϱ�ECALL�Ự��״̬
     *         ˵���������������MBB��Ʒ��ecall����֧�ֵİ汾������BalongV722C60/C70��V711C60��V7R5C60��B5000C60��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ECLSTAT: <ecall_state>[,<description>]<CR><LF>
     * [����]:
     *     <ecall_state>: ����ֵ����ʾECALL�Ự��״̬��
     *             0����ʼ����MSD����
     *             1��MSD���ݴ���ɹ�
     *             2��MSD���ݴ����쳣
     *             3���յ�PSAPҪ�����MSD���ݲ��ش���ָʾ
     *     <description>: ECALL�Ự״̬�ĸ�������
     *             ��<ecall_state>Ϊ 2��MSD���ݴ�������쳣��ʱ��<description> ��ʾMSD�����쳣��ԭ��
     *             0���ȴ�PSAP�Ĵ���ָʾ��ʱ
     *             1��MSD���ݴ��䳬ʱ
     *             2���ȴ�Ӧ�ò��ȷ�ϳ�ʱ
     *             3�������쳣����
     *             4���������
     *             ��<ecall_state>Ϊ 3���յ�PSAPҪ�����MSD���ݲ��ش���ָʾ��ʱ��<description> ��ʾ�������MSD��ʱ�䴰���ȣ�����ֵ����λ�����룬��Χ��500~10000��
     * [ʾ��]:
     *       MED��ʼ����MSD����
     *       ^ECLSTAT: 0
     */
    { AT_STRING_ECLSTAT, (VOS_UINT8 *)"^ECLSTAT" },
    /*
     * [���]: Э��AT-��·��ҵ��
     * [����]: eCall AL_ACK��Ϣ�����ϱ�����
     * [˵��]: ����������ָʾeCall�Ự�ɹ����Զ�����AL_ACK��ֵ��ʱ�����Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ECLREC: <timestamp>,<AL_ACK><CR><LF>
     * [����]:
     *     <timestamp>: �ַ������ͣ�ָʾIVS����PSAP���͵�AL_ACK��ʱ�䡣
     *             ��ʽΪ��
     *             ��YYYY/MM/DD,HH:MM:SS+ZZ��
     *             YYYY��ȡֵ��Χ��2000��2100��
     *     <AL_ACK>: ����ֵ����PSAP���ղ��ɹ���֤MSD����ʱ��PSAP��AL_ACK���͵�IVS������Clear Down��־��AL_ACKҪ��IVS�Ҷ�eCall�����ο�Э��16062  7.5.5��
     *             AL_ACK��4 Bit��ɣ�����Bit3��Bit4Ϊ����λ
     *             Bit1ΪFormat version��ȡֵ���£�
     *             0��Format version0��
     *             1��Format version1��
     *             Bit2ΪStatus��ȡֵ���£�
     *             0��MSD���ݱ���֤��
     *             1��MSD���ݱ���֤������Ҫ��IVS�Ҷ�eCall��
     * [ʾ��]:
     *       ��eCall MSD����ɹ��������ϱ���Ϣ
     *       ^ECLSTAT: 3,4000
     *       ^ECLSTAT: 0
     *       ^ECLSTAT: 1
     *       ^ECLREC: "2018/11/28,03:18:16+32",0
     */
    { AT_STRING_ECLREC, (VOS_UINT8 *)"^ECLREC" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ��������
     * [˵��]: ����ʹ��CLIP������ʱ�ϱ����к���+CLIP: <number>,<type>��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CLIP: <number>,<type>,,,,<CLI validity><CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: ����ֵ�������ַ���ͣ���ṹ���2-8��ʾ��
     *             Type-of-number��bit[6:4]����ȡֵ���£�
     *             000���û����˽�Ŀ�ĵ�ַ����ʱ��ѡ�ô�ֵ����ʱĿ�ĵ�ַ������������д��
     *             001�����û���ʶ���ǹ��ʺ���ʱ��ѡ�ô�ֵ��������Ϊ�ǹ��ڷ�ΧҲ������д��
     *             010�����ں��룬�������ǰ׺���ߺ�׺�����û����͹��ڵ绰ʱ��ѡ�ô�ֵ��
     *             011���������ڵ��ض����룬���ڹ�����߷����û�����ѡ�ô�ֵ��
     *             101����������ΪGSM��ȱʡ7bit���뷽ʽ��
     *             110���̺��롣
     *             111����չ������
     *             Numbering-plan-identification��bit[3:0]����ȡֵ���£�
     *             0000������������ĺ��뷽��ȷ����
     *             0001��ISDN/telephone numbering plan��
     *             0011��Data numbering plan��
     *             0100��Telex numbering plan��
     *             1000��National numbering plan��
     *             1001��Private numbering plan��
     *             1010��ERMES numbering plan��
     *             ˵������bit[6:4]ȡֵΪ000��001��010ʱbit[3:0]����Ч��
     *     <CLI validity>: ���������ʾ��Ч�ԣ���ȡֵ���£�
     *             0��CLI��Ч��
     *             1��CLI�����з��𷽾ܾ���
     *             2�����ڷ�����������ƻ������������CLI�����á�
     *             <type>��<CLI validity>֮�䱣���������ֶΡ�
     * [��]: �����ַ���͵Ľṹ
     *       bit:   7,    6,              5,              4,              3,                             2,                             1,                             0,
     *              1,    Type-of-number, Type-of-number, Type-of-number, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification, Numbering-plan-identification,
     * [ʾ��]:
     *       ������ʱ
     *       +CLIP: "13988868668",129,"",,"",0
     */
    { AT_STRING_CLIP, (VOS_UINT8 *)"+CLIP" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ����Ӻ���
     * [˵��]: ����ʹ��COLP������ʱ�ϱ����Ӻ��롣
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+COLP: <number>,<type>[,<subaddr>,<satype>[,<alpha>]]<CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <subaddr>: �ַ������ͣ��ӵ绰���룬��ʽ��<satype>������Ŀǰ������˲�����
     *     <satype>: �ӵ绰�������ͣ�����ֵ��Ŀǰ������˲�����
     *     <alpha>: �ַ����������ڵ绰���ж�Ӧ��������Ŀǰ������˲�����
     * [ʾ��]:
     *     �� ��������ʱ
     *       ATD13902100013;
     *       +COLP: "1234567890",129
     *       OK
     */
    { AT_STRING_COLP, (VOS_UINT8 *)"+COLP" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�����ҵ��ָʾ
     * [˵��]: ���+CSSI��ʹ�ܣ�����ҵ����ʱ���ϱ�+CSSI: <code1>[,<index>]��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CSSI: <code1>[,<index>]<CR><LF>
     * [����]:
     *     <code1>: ����ֵ������ҵ��ָʾ�롣
     *             0���Ѽ�������������ת�ƣ�
     *             1���Ѽ���ĳЩ����������ת�ƣ�
     *             2�������ĺ��б�ת�ƣ�
     *             3�������ĺ������ڵȴ�״̬��
     *             4������һ��CUG���У�������CUG������<index>����
     *             5�������ĺ��б���ֹ��
     *             6�����з���ֹ���룻
     *             7����CLIR�Ľ�ֹ���ܾ���
     *             8�����з���ƫת��
     *             16�����������緢����еĻغ��������лغ�������<index>����
     *     <index>: ��μ�2.4.4 �պ��û�Ⱥ��+CCUG�в���<index>�ľ��嶨�塣
     * [ʾ��]:
     *       ���б�ת�ƣ��յ�+CSSI�¼��ϱ�
     *       +CSSI: 2
     */
    { AT_STRING_STD_CSSI, (VOS_UINT8 *)"+CSSI" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�����ҵ��ָʾ
     * [˵��]: ����IMS��
     *         ���+CSSI��ʹ�ܣ�����ҵ����ʱ���ϱ�^CSSI: <code1>,[<index>],<callId>[,<number>,<type>]��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CSSI: <code1> ,[<index>],<callId>[,<number>,<type>]<CR><LF>
     * [����]:
     *     <code1>: ����ֵ������ҵ��ָʾ�롣
     *             0���Ѽ�������������ת�ƣ�
     *             1���Ѽ���ĳЩ����������ת�ƣ�
     *             2�������ĺ��б�ת�ƣ�������CUG������<index>����
     *             3�������ĺ������ڵȴ�״̬��
     *             4������һ��CUG���У�������CUG������<index>����
     *             5�������ĺ��б���ֹ��
     *             6�����з���ֹ���룻
     *             7����CLIR�Ľ�ֹ���ܾ���
     *             8�����з���ƫת��
     *             16�����������緢����еĻغ��������лغ�������<index>����
     *     <callId>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <index>: ��μ�2.4.4 �պ��û�Ⱥ��+CCUG�в���<index>�ľ��嶨�塣
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>����
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     * [ʾ��]:
     *       IMS���������б�ת�ƣ��յ�^CSSI�¼��ϱ�
     *       ^CSSI: 2,1,1,"13800138000",129
     */
    { AT_STRING_CUST_CSSI, (VOS_UINT8 *)"^CSSI" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�����ҵ��ָʾ
     * [˵��]: ���+CSSU��ʹ�ܣ�����ҵ����ʱ���ϱ�+CSSU: <code2>[,<index>[, <number>,<type>]]��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CSSU: <code2>[,<index>[,<number>,<type>]]<CR><LF>
     * [����]:
     *     <index>: ��μ�2.4.4 �պ��û�Ⱥ��+CCUG�в���<index>�ľ��嶨�塣
     *     <code2>: ����ֵ������ҵ��ָʾ�롣
     *             0��������һ����ת�Ƶĺ��У�
     *             1��������һ��CUG���У�
     *             2��ͨ�����Է����֣�
     *             3��ͨ�����Է��ָ���
     *             4������෽ͨ���У�
     *             5�����ֵ�ͨ�����Ҷϣ�
     *             6���յ�ǰ����Ĳ���ҵ����Ϣ��
     *             7��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             8��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             9��������һ����ƫת�ĺ��У�
     *             10�����类ת�ƣ�
     *             22������绰�����緢��ĺ��С�
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     * [ʾ��]:
     *       ͨ���У����Է�����෽ͨ�����յ�+CSSU�¼��ϱ�
     *       +CSSU: 4
     */
    { AT_STRING_STD_CSSU, (VOS_UINT8 *)"+CSSU" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�����ҵ��ָʾ
     * [˵��]: ����IMS��
     *         ���+CSSU��ʹ�ܣ�����ҵ����ʱ���ϱ�^CSSU: <code2> [,<index>[, <number>,<type>]]��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CSSU: <code2>[,<index>[,<number>,<type>]]<CR><LF>
     * [����]:
     *     <index>: ����ֵ���պ��û�Ⱥ������ֵ��Ĭ��ֵΪ0��
     *             0��9��Ⱥ�������ֵ��
     *             10�����������û�������ǩԼ����ѡ��Ⱥ�飩��
     *     <code2>: ����ֵ������ҵ��ָʾ�롣
     *             0��������һ����ת�Ƶĺ��У�
     *             1��������һ��CUG���У�
     *             2��ͨ�����Է����֣�
     *             3��ͨ�����Է��ָ���
     *             4������෽ͨ���У�
     *             5�����ֵ�ͨ�����Ҷϣ�
     *             6���յ�ǰ����Ĳ���ҵ����Ϣ��
     *             7��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             8��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             9��������һ����ƫת�ĺ��У�
     *             10�����类ת�ƣ�
     *             22������绰�����緢��ĺ��С�
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             Type-of-number��bit[6:4]����ȡֵ���£�
     *             000���û����˽�Ŀ�ĵ�ַ����ʱ��ѡ�ô�ֵ����ʱĿ�ĵ�ַ������������д��
     *             001�����û���ʶ���ǹ��ʺ���ʱ��ѡ�ô�ֵ��������Ϊ�ǹ��ڷ�ΧҲ������д��
     *             010�����ں��룬�������ǰ׺���ߺ�׺�����û����͹��ڵ绰ʱ��ѡ�ô�ֵ��
     *             011���������ڵ��ض����룬���ڹ�����߷����û�����ѡ�ô�ֵ��
     *             101����������ΪGSM��ȱʡ7bit���뷽ʽ��
     *             110���̺��롣
     *             111����չ������
     *             Numbering-plan-identification��bit[3:0]����ȡֵ���£�
     *             0000������������ĺ��뷽��ȷ����
     *             0001��ISDN/telephone numbering plan��
     *             0011��Data numbering plan��
     *             0100��Telex numbering plan��
     *             1000��National numbering plan��
     *             1001��Private numbering plan��
     *             1010��ERMES numbering plan��
     *             ˵������bit[6:4]ȡֵΪ000��001��010ʱbit[3:0]����Ч��
     * [ʾ��]:
     *       IMS������ͨ���У����Է�����෽ͨ�����յ�^CSSU�¼��ϱ�
     *       ^CSSU: 4
     */
    { AT_STRING_CUST_CSSU, (VOS_UINT8 *)"^CSSU" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�����ҵ��ָʾ
     * [˵��]: ����IMS��
     *         ���+CSSU��ʹ�ܣ�����ҵ����ʱ���ϱ�^CSSUEX: <code2>,[<index>],<callId>[,<number>,<type>[,<forward_cause>]]��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CSSUEX: <code2>,[<index>],<callId>[,<number>,<type>[,<forward_cause>]]<CR><LF>
     * [����]:
     *     <code2>: ����ֵ������ҵ��ָʾ�롣
     *             0��������һ����ת�Ƶĺ��У�
     *             1��������һ��CUG���У�
     *             2��ͨ�����Է����֣�
     *             3��ͨ�����Է��ָ���
     *             4������෽ͨ���У�
     *             5�����ֵ�ͨ�����Ҷϣ�
     *             6���յ�ǰ����Ĳ���ҵ����Ϣ��
     *             7��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             8��ͨ����ת�Ƶ���������������ʾ�ı��к����ǵ��������룻
     *             9��������һ����ƫת�ĺ��У�
     *             10�����类ת�ƣ�
     *             22������绰�����緢��ĺ��С�
     *     <callId>: ����ֵ������ID��ȡֵ��Χ1~7
     *     <index>: ����ֵ���պ��û�Ⱥ������ֵ��Ĭ��ֵΪ0��
     *             0��9��Ⱥ�������ֵ��
     *             10�����������û�������ǩԼ����ѡ��Ⱥ�飩��
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>����
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             Type-of-number��bit[6:4]����ȡֵ���£�
     *             000���û����˽�Ŀ�ĵ�ַ����ʱ��ѡ�ô�ֵ����ʱĿ�ĵ�ַ������������д��
     *             001�����û���ʶ���ǹ��ʺ���ʱ��ѡ�ô�ֵ��������Ϊ�ǹ��ڷ�ΧҲ������д��
     *             010�����ں��룬�������ǰ׺���ߺ�׺�����û����͹��ڵ绰ʱ��ѡ�ô�ֵ��
     *             011���������ڵ��ض����룬���ڹ�����߷����û�����ѡ�ô�ֵ��
     *             101����������ΪGSM��ȱʡ7bit���뷽ʽ��
     *             110���̺��롣
     *             111����չ������
     *             Numbering-plan-identification��bit[3:0]����ȡֵ���£�
     *             0000������������ĺ��뷽��ȷ����
     *             0001��ISDN/telephone numbering plan��
     *             0011��Data numbering plan��
     *             0100��Telex numbering plan��
     *             1000��National numbering plan��
     *             1001��Private numbering plan��
     *             1010��ERMES numbering plan��
     *             ˵������bit[6:4]ȡֵΪ000��001��010ʱbit[3:0]����Ч��
     * [ʾ��]:
     *       IMS������ͨ���У����Է����֣��յ�^CSSU�¼��ϱ�
     *       ^CSSU: 2,,1
     */
    { AT_STRING_CSSUEX, (VOS_UINT8 *)"^CSSUEX" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ����еȴ�
     * [˵��]: ����ʹ�ܺ��еȴ�������ϱ���ͨ����������ʱ�ϱ�+CCWA: <number>,<type>,<class>,,<CLI_validity>��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CCWA: [<number>],[<type>],<class>,,<CLI_validity><CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <class>: ����ֵ��ҵ�����ȡֵΪ����ҵ��������ϣ�Ĭ��ֵΪ255��
     *             1��voice (telephony)��
     *             2��data��
     *             4��fax (facsimile services)��
     *             8��short message service��
     *             16��data circuit sync��
     *             32��data circuit async��
     *             64��dedicated packet access��
     *             128��dedicated PAD access��
     *     <CLI_validity>: ����ֵ������<number>��δ����ʾ���к����ԭ��
     *             0����ʾ������Ч
     *             1���û��ܾ��ṩ
     *             2�������ڲ�ԭ��
     *             3�������绰
     *             4��������
     * [ʾ��]:
     *       ��ͨ���У����µ����磬���粻�ṩ����
     *       +CCWA: "",128,1,,0
     *       ��ͨ���У����µ����磬�����ṩ����
     *       +CCWA: "12345678",145,1,,0
     */
    { AT_STRING_STD_CCWA, (VOS_UINT8 *)"+CCWA" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ����еȴ�
     * [˵��]: ����IMS��
     *         ����ʹ��+CCWA���еȴ�������ϱ���ͨ����������ʱ�ϱ�^CCWA: <number>,<type>,<class>��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CCWA: [<number>],[<type>],<class><CR><LF>
     * [����]:
     *     <number>: �ַ������ͣ��绰���룬��ʽ��<type>������
     *     <type>: �����ַ���ͣ�����ֵ��
     *             129����ͨ���룻
     *             145�����ʺ��루�ԡ�+����ͷ����
     *             ����ȡֵ��μ�2.4.11 �����ϱ�������룺+CLIP�в���<type>�ľ��嶨�塣
     *     <class>: ҵ�����
     *             1������ҵ��
     *             2����Ƶҵ��
     * [ʾ��]:
     *       ��ͨ���У����µ�IMS����������
     *       ^CCWA: "12345678",145,1
     */
    { AT_STRING_CUST_CCWA, (VOS_UINT8 *)"^CCWA" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ�USSDҵ��
     * [˵��]: ������ӦTE�����USSDҵ�񣬻����緢��USSDҵ�������ʱ���ϱ�
     *         +CUSD: <m>[,<str>,<dcs>]��
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CUSD: <m>[,<str>,<dcs>]<CR><LF>
     * [����]:
     *     <m>: ����ֵ��
     *             0�����粻��ҪTE�ظ������緢���USSD-Notify������TE��������Ժ����粢����Ҫ��һ������Ϣ����
     *             1��������ҪTE�ظ������緢���USSD-Request������TE��������Ժ�������Ҫ��һ������Ϣ����
     *             2��USSD�Ự�������ͷţ�
     *             3���������ؿͻ����Ѿ�������Ӧ��
     *             4��������֧�֣�
     *             5�����糬ʱ��
     *     <str>: USSD�ַ�������󳤶�Ϊ160���ַ���
     *     <dcs>: ����ֵ�����뷽ʽ��
     *             15����ָ�����Ե�ȱʡ7bit���룻
     *             68��8bit���룻
     *             72��UCS2���롣
     * [ʾ��]:
     *       ����һ��USSD����������ظ�
     *       +CUSD: 1,"12345678",68
     */
    { AT_STRING_CUSD, (VOS_UINT8 *)"+CUSD" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �û��������1�����ϱ�ָʾ
     * [˵��]: MO�����л������ҵ��绰�����У��յ����緢�͵���Ϣ�а���User-to-User Information Element��Ϣʱ�����+CUUS1Iʹ�ܿ��ش򿪣���Ҫ��+CUUS1I��ʽ�ϱ���Ӧ�á�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CUUS1I: <messageI>,<UUIE><CR><LF>
     * [����]:
     *     <messageI>: +CUUS1I�Ľ�����ϱ�����Ϣ���͡�
     *             0��ANY��
     *             1��ALERT��
     *             2��PROGRESS��
     *             3��CONNECT (sent after +COLP if enabled)��
     *             4��RELEASE��
     *     <UUIE>: 16�����ַ�����ͬ��Э��24008�涨��User-to-User Information ElementЭ���ʽ��
     * [ʾ��]:
     *       �յ�������ϢPROGRESS��Я��User-to-User Information Element��Ϣ
     *       +CUUS1I: 2,7e0110
     */
    { AT_STRING_CUUS1I, (VOS_UINT8 *)"+CUUS1I" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �û��������1�����ϱ�ָʾ
     * [˵��]: MT�����л�Է��ҵ��绰�����У��յ����緢�͵���Ϣ�а���User-to-User Information Element��Ϣʱ�����+CUUS1Uʹ�ܿ��ش򿪣���Ҫ��+CUUS1U��ʽ�ϱ���Ӧ�á�
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CUUS1U: <messageU>,<UUIE><CR><LF>
     * [����]:
     *     <messageU>: +CUUS1U�Ľ�����ϱ�����Ϣ���͡�
     *             0��ANY��
     *             1��SETUP (returned after +CLIP if presented, otherwise after every RING or +CRING)��
     *             2��DISCONNECT��
     *             3��RELEASE_COMPLETE��
     *     <UUIE>: 16�����ַ�����ͬ��Э��24008�涨��User-to-User Information ElementЭ���ʽ��
     * [ʾ��]:
     *       �յ�������ϢDISCONNECT��Я��User-to-User Information Element��Ϣ
     *       +CUUS1U: 2,7e021010
     */
    { AT_STRING_CUUS1U, (VOS_UINT8 *)"+CUUS1U" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �����ϱ���������
     * [˵��]: ������������ʾ���ܺ��������ϱ���������+CNAP: <name>,<CNI_validity>��
     *         ���������3GPP TS 27.007Э�顣
     *         �����֧����ʾUCS-2�����ʽ���������ƣ������ϱ�UCS-2��ʽʱ������<name>��ʾΪ�գ���ͨ��^CNAP��ѯUnicode�����ʽ������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CNAP: <name>,<CNI_validity><CR><LF>
     * [����]:
     *     <name>: �ַ������ͣ����з����ƣ����80���ַ���
     *     <CNI_validity>: ����ֵ������������ʾ��Ч�ԣ�
     *             0��CNI��Ч��
     *             1��CNI�����з��ܾ���ʾ��
     *             2�����ڷ�����������ƻ������������CNI�����á�
     * [ʾ��]:
     *       ����ʱ�����ϱ���������
     *       +CNAP: "John Doe",0
     */
    { AT_STRING_CNAP, (VOS_UINT8 *)"+CNAP" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ���������
     * [˵��]: ���������ϱ���
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: if text mode (+CMGF=1):
     *             <CR><LF>+CMT: <oa>,[<alpha>],<scts>[,<tooa>,<fo>,<pid>,<dcs>,<sca>,<tosca>,<length>]<CR><LF><data><CR><LF>
     *             if PDU mode (+CMGF=0):
     *             <CR><LF>+CMT: [<alpha>],<length><CR><LF><pdu><CR><LF>
     * [����]:
     * [ʾ��]:
     *       �ı�ģʽ���������ϱ�
     *       +CMT: "13900000000",,"02/11/19,09:58:42+00",129,36,0,0,"+8613800000000",145,5
     *       Hello
     *       +CSCS����TE��MT֮����ַ���ΪGSM7BIT���룬��������ͨ��������������������磺�ı�@123
     *       +CMT: "13900000000",,"02/11/19,09:58:42+00",129,36,0,0,"+8613800000000",145,5
     *       00313233
     *       PDUģʽ���������ϱ�
     *       +CMT: ,24
     *       0891683108703705F1040D91683109120090F60008603070
     */
    { AT_STRING_CMT, (VOS_UINT8 *)"+CMT" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ�����״̬
     * [˵��]: ����״̬�ϱ���
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: if text mode (+CMGF=1):
     *             <CR><LF>+CDS: <fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st><CR><LF>
     *             if PDU mode (+CMGF=0):
     *             <CR><LF>+CDS: <length><CR><LF><pdu><CR><LF>
     * [����]:
     * [ʾ��]:
     *       �ı�ģʽ����״̬�ϱ�
     *       +CDS: 2,10,"358501234567",129,"98/07/04/13:12:14+04",
     *       "98/07/04/13:12:20+04",0
     */
    { AT_STRING_CDS, (VOS_UINT8 *)"+CDS" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ���������
     * [˵��]: ���������ϱ���
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CMTI: <mem>,<index><CR><LF>
     * [����]:
     * [ʾ��]:
     *       ���������ϱ�
     *       +CMTI: "SM",1
     */
    { AT_STRING_CMTI, (VOS_UINT8 *)"+CMTI" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ�����״̬����
     * [˵��]: ����״̬�����ϱ���
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>+CDSI: <mem>,<index><CR><LF>
     * [����]:
     * [ʾ��]:
     *       ����״̬�����ϱ�
     *       +CDSI: "SM",1
     */
    { AT_STRING_CDSI, (VOS_UINT8 *)"+CDSI" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �洢���������ϱ�
     * [˵��]: ��ǰ�Ĵ洢����ʱ���յ��¶���ʱ�����ϱ�^SMMEMFULL�����ظ�������ն���ʧ�ܣ�ԭ��Ϊ�޿��ô洢�ռ䡣
     *         ��ǰ�Ĵ洢����ʱ���յ��û���д���������Ȼظ�������ʧ�ܣ�Ȼ�������ϱ�^SMMEMFULL��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SMMEMFULL: [<mem>]<CR><LF>
     * [����]:
     * [ʾ��]:
     *       ��ǰ�Ĵ洢��Ϊ(U)SIM���Ҵ洢��������ʱ���յ��¶����ϱ�
     *       ^SMMEMFULL: "SM"
     *     �� ��ǰд�����Ĵ洢��Ϊ(U)SIM���Ҵ洢��������ʱ���յ��û����ı���ʽд��������
     *       AT+CMGW="13800000000"
     *       ��HELLO,WORLD.
     *       +CMS ERROR: memory full
     *       ^SMMEMFULL: "SM"
     */
    { AT_STRING_SMMEMFULL, (VOS_UINT8 *)"^SMMEMFULL" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: �ϱ�С���㲥����
     * [˵��]: С���㲥(CBS)�����ϱ���
     *         ���������3GPP TS 27.005Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: if text mode (+CMGF=1):
     *             <CR><LF>+CBM: <sn>,<mid>,<dcs>,<page>,<pages><CR><LF><data><CR><LF>
     *             if PDU mode (+CMGF=0):
     *             <CR><LF>+CBM: <length><CR><LF><pdu><CR><LF>
     * [����]:
     *     <sn>: С���㲥�����кš�
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <mid>: С���㲥����ϢID��
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <dcs>: С���㲥��Ϣ�ı��뷽����
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <page>: С���㲥��Ϣ��ҳ��š�
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <pages>: С���㲥��Ϣ����ҳ����
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <data>: ������С���㲥���ݡ�
     *     <length>: ����ֵ��PDU���ݵ��ֽ�����
     *     <pdu>: 16�������ַ�����PDU���ݡ�
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     * [ʾ��]:
     *       �ı�ģʽ���������ϱ�
     *       +CBM: 12288,4354,0,1,2
     *       CHARSTRING
     *       +CBM: 12288,4354,0,2,2
     *       CHARSTRING
     */
    { AT_STRING_CBM, (VOS_UINT8 *)"+CBM" },
    /*
     * [���]: Э��AT-����ҵ��
     * [����]: ETWS����Ϣ�ϱ�
     * [˵��]: ETWS����Ϣ�ϱ������������3GPP TS 23.041Э�顣
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ETWSPN: <plmn_id>,<warning_type>,<msg_id>,<sn>,<auth>[,<warning_security_information>]<CR><LF >
     * [����]:
     *     <plmn_id>: ��ǰפ�������PLMN��
     *     <warning_type>: �澯���ͣ�ʮ��������ʽ��
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <msg_id>: ��ϢID��ʮ��������ʽ��
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <sn>: ���кš�ʮ��������ʽ��
     *             ����μ�3GPP TS 23.041 9.4�ڡ�
     *     <auth>: ��Ȩ�����
     *             0��ͨ����Ȩ
     *             1��δͨ����Ȩ
     * [ʾ��]:
     *       ETWS����Ϣ�ϱ�
     *       ^ETWSPN: "24005",0000,1105,1000,1
     */
    { AT_STRING_ETWSPN, (VOS_UINT8 *)"^ETWSPN" },
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ���ųɹ������ϱ�
     * [˵��]: �������ڲ��ųɹ��������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^DCONN: <cid>,<pdpType><CR><LF>
     * [����]:
     *     <cid>: ��ǰ���ųɹ���cid��
     *     <pdpType>: �������ͣ�IPV4��IPV6����Ethernet��
     * [ʾ��]:
     *       ���ųɹ����ϱ�
     *       ^DCONN: 1,"IPV4"
     */
    { AT_STRING_DCONN, (VOS_UINT8 *)"^DCONN" },
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: ���ŶϿ������ϱ�
     * [˵��]: �������ڲ��ŶϿ��������ϱ���
     *         ��IPV6��ַ����ʧ�ܻ��ߵ�ַʧЧ��Ҳ�������ϱ�IPV6���ŶϿ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^DEND: <cid>,<endCause>,<pdpType>[,<backOffTimer>[,<allowedSscMode>]] <CR><LF>
     * [����]:
     *     <cid>: ��ǰ���ŶϿ���cid��
     *     <endCause>: ���ŶϿ���ԭ��ֵ��
     *     <pdpType>: �������ͣ�IPV4��IPV6����Ethernet��
     *     <backOffTimer>: ����ֵ��backOff timerʱ������λs
     *     <allowedSscMode>: ����ֵ��allowed-SSC-MODE���ͣ�ʹ�������ʾ��
     *             Bit0��Ϊ1��ʾ֧��SSC MODE1��Ϊ0��ʾ��֧��
     *             Bit1��Ϊ1��ʾ֧��SSC MODE2��Ϊ0��ʾ��֧��
     *             Bit2��Ϊ1��ʾ֧��SSC MODE3��Ϊ0��ʾ��֧��
     * [ʾ��]:
     *       IPV6��ַ����ʧ�ܻ��ߵ�ַʧЧ���ϱ�
     *       ^DEND: 1,34,IPV6
     *       ���ŶϿ����ϱ�
     *       ^DEND: 1,255,"IPV4"
     *       ���ŶϿ����ϱ�ʱ��Я��backoff timer��allowed SSC mode
     *       ^DEND: 1,255,"IPV4",25,0x03
     */
    { AT_STRING_DEND, (VOS_UINT8 *)"^DEND" },
    /*
     * [���]: Э��AT-������ҵ��
     * [����]: IP��ַ��Ϣ����ϱ�
     * [˵��]: IP ��ַ������ɾ��������Modemʹ�ô�����֪ͨAP���������ϱ���ʱ��Я��������ͺ����ݡ�
     *         ע�⣺AP���Ը��ݱ�������Я����IP��ַˢ��������Ҳ�����յ��������ʹ��^DHCP/^DHCPV6�����ѯIP��ַ�����ݲ�ѯ�Ľ������ԭ�е����̴���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IPCHANGE: <cid>,<opreate>,<ip_type>,<ipv4_addr>,<ipv6_addr>
     *             <CR><LF>
     * [����]:
     *     <cid>: ����ֵ������ҵ�񲦺�ʱ��AP�·��Ĳ���������Я����CIDֵ
     *     <opreate>: ����ֵ��IP��ַ�������
     *             0������IP��ַ
     *             1��ɾ��IP��ַ
     *             2�����IP��ַ
     *     <ip_type>: �ַ������ͣ���������Э������
     *             IP������Э�飨IPv4��
     *             IPV6��IPv6Э��
     *             IPV4V6��IPv4��IPv6
     *     <ipv4_addr>: ����ֵ������IPv4��ַ
     *             ȡֵ��Χ0x0000_0000��0xFFFF_FFFF
     *     <ipv6_addr>: ����ֵ������IPv6��ַ��ʹ��ѹ����ʽ��
     * [ʾ��]:
     *       ����IPv4v6��ַ�ϱ�
     *       ^IPCHANGE: 1,0,IPV4V6,0AA80036,fe80::0001:0000
     *       ɾ��IPv6��ַfe80:0000:0000:0000:0000:0000:0001:0000�ϱ�
     *       ^IPCHANGE: 1,1,IPV6,,fe80::0001:0000
     *       �޸�IPv4��ַ�ϱ����µ�ַΪ0A.168.0.54
     *       ^IPCHANGE: 1,2,IP,0AA80036
     */
    { AT_STRING_IPCHANGE, (VOS_UINT8 *)"^IPCHANGE" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: ָʾ(U)SIM״̬�仯
     * [˵��]: ��(U)SIM״̬�����ı�ʱ�������ϱ���ǰ(U)SIM����״̬��ͬʱ�ϱ�LOCK״̬��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^SIMST: <sim_state>[,<lock_state>]<CR><LF>
     * [����]:
     *     <sim_state>: �����ͣ�(U)SIM��״̬��ȡֵ0��4��240��255��
     *             0��(U)SIM��״̬��Ч��
     *             1��(U)SIM��״̬��Ч��
     *             2��(U)SIM��CS����Ч��
     *             3��(U)SIM��PS����Ч��
     *             4��(U)SIM��PS+CS�¾���Ч��
     *             240��ROMSIM�汾��
     *             255��(U)SIM�������ڣ�����(U)SIM��ȷʵ�����ں���Ϊ��CardLock��ס�������������������ʱ��Ӧͨ��<lock_state>��ȡֵ�ж�(U)SIM������ʵ״̬��
     *     <lock_state>: �����ͣ�(U)SIM����LOCK״̬��ȡֵ0��1��
     *             0��(U)SIM��δ��CardLock����������
     *             1��(U)SIM����CardLock����������
     * [ʾ��]:
     *       ��ǰ(U)SIM������λ
     *       ^SIMST: 0,0
     *       ��ǰ(U)SIM����Ч
     *       ^SIMST: 1,0
     */
    { AT_STRING_SIMST, (VOS_UINT8 *)"^SIMST" },
    /*
     * [���]: Э��AT-(U)SIM���
     * [����]: USIMģ���쳣�¼��ϱ�
     * [˵��]: USIMMģ����FEATURE_UE_UICC_MULTI_APP_SUPPORT�򿪣���ʼ������ʱ��������쳣�¼���������ͨ�����AT�����ϱ���Ӧ���쳣ԭ���쳣ԭ�����Ϊ16���ƣ�AP���Ը����쳣ֵ�ж��Ƿ���Ҫ�����Ӧ��CHR��Ϣ��
     *         ������һ�ο����ϱ��������ֵ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^USIMMEX: <Error><CR><LF>
     * [����]:
     *     <Error>: 0x10000001�����������ӿڸ�λ��ʧ�ܣ�
     *             0x10000002�����������ӿڶ�ȡATRʧ�ܣ�
     *             0x10000003����ʼ��ѡ��UICC��MFʧ�ܣ�
     *             0x10000004����ʼ��ѡ��EFDIRʧ�ܣ�
     *             0x10000005����ʼ����ȡEFDIRʧ�ܣ�
     *             0x10000006����ʼ������EFDIRʧ�ܣ�
     *             0x10000007����ʼ��ѡ��ICC��MFʧ�ܣ�
     *             0x10000008�����������ӿ��л�SIM����ѹʧ�ܣ�
     *             0x10000009������Profile Downloadʧ�ܣ�
     *             0x1000000A���л���ѹ֮��ѡ��MFʧ�ܣ�
     *             0x1000000B��NV9090������ҪCSIMӦ�ô��ڵļ��ʧ�ܣ�
     *             0x1000000C��NV9090������ҪCDMAĿ¼���ڵļ��ʧ�ܣ�
     *             0x1000000D����ȡPIN1��Ϣ����
     *             0x1000000E����ʼ��PIN1ʣ�����ʧ�ܣ�
     *             0x1000000F����ʼ��PUK1ʣ�����ʧ�ܣ�
     *             0x10010000����ȡSIM����ѡ�ļ�ʧ�ܣ�ʧ�ܵ��ļ�ID����ڵ�2�ֽ��ϱ�����ͬ��
     *             0x10020000����ȡUSIM���ı�ѡ�ļ�ʧ�ܣ�
     *             0x10030000����ȡISIM���ı�ѡ�ļ�ʧ�ܣ�
     *             0x10040000����ȡUIM����ѡ�ļ�ʧ�ܣ�
     *             0x10050000����ȡCSIM����ѡ�ļ�ʧ�ܣ�
     *             0x10060000����ȡNVʧ�ܣ�
     * [ʾ��]:
     *       ^USIMMEX: 0x0x10000009, 0x10056F22
     */
    { AT_STRING_USIMMEX, (VOS_UINT8 *)"^USIMMEX" },
    /*
     * [���]: Э��AT-(U)SATҵ��
     * [����]: STK�¼�֪ͨ�ϱ�
     * [˵��]: ����������֪ͨTE��SIM���ϱ���һ�����������MS��TE�յ���֪ͨ��Ӧ�·�^STGI�����ȡ�����������ݣ������������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^STIN: <CmdType>,<CmdIndex>,<isTimeOut><CR><LF>
     * [����]:
     * [ʾ��]:
     *       �ޡ�
     */
    { AT_STRING_STIN, (VOS_UINT8 *)"^STIN" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMS EMC PDN״̬�ϱ�
     * [˵��]: ��������IMS EMC PDN����/�Ͽ�ʱ�����ϱ���
     *         ˵��������IMS�������ж�λ���ԡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSEMCSTAT: <stat><CR><LF>
     * [����]:
     *     <stat>: ����ֵ��IMS EMC PDN����״̬��
     *             0��IMS EMC PDN�Ͽ���
     *             1��IMS EMC PDN������
     * [ʾ��]:
     *       IMS EMC PDN����
     *       ^IMSEMCSTAT: 1
     *       IMS EMC PDN�Ͽ�
     *       ^IMSEMCSTAT: 0
     */
    { AT_STRING_IMSEMCSTAT, (VOS_UINT8 *)"^IMSEMCSTAT" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���������λ��Ϣָʾ�����ϱ�
     * [˵��]: Modem ��CS����·�ͷ�ʱ�ϱ�������������GPSоƬ����ĸ�����λ��Ϣ��
     *         ������ͬʱ��^XCPOSR ��^XCPOSRRPT������ơ�
     *         ��GPSоƬ֧�����������λ��Ϣ����ʱ���Ҵ��������ϱ����������λ��Ϣָʾʱ�Ż��ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^XCPOSRRPT: <value><CR><LF>
     * [����]:
     *     <value>: �����ͣ����������λ��Ϣ��ȡֵ1��
     *             1��GPSоƬ���������λ��Ϣ��
     * [ʾ��]:
     *       ���������λ��Ϣָʾ�������ϱ�����֧���Ҵ�ʱModem��CS����·�ͷţ�
     *       ^XCPOSRRPT: 1
     */
    { AT_STRING_XCPOSRRPT, (VOS_UINT8 *)"^XCPOSRRPT" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: Call Control��MO SMS Control����ϱ�
     * [˵��]: ����������Call Control��MO SMS Control��������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CCIN: <Type>,<Result>,<Alpha_id>[,<service_center_address>,<service_center_address_type>,<services_center_address_type>,<destination_address>,<destination_address_type>,<dcs>,<data>]<CR><LF>
     * [����]:
     *     <Type>: ����ֵ��Call/SMS Control�����͡�
     *             0��MO Call Control by USIM��
     *             1��SS Call Control by USIM��
     *             2��USSD Call Control by USIM��
     *             3��SMS Control by USIM��
     *     <Result>: ����ֵ��Call/SMS Control�Ľ����
     *             0������
     *             1��������
     *             2��������������
     *             3��USIMæ��
     *     <Alpha_id>: �ַ������ͣ��ַ���ʶ��
     *     <service_center_address>: �ַ������ͣ��������ĺ��롣
     *     <service_center_address_type>: ����ֵ���������ĺ������͡�
     *     <destination_address>: �ַ������ͣ����պ��롣
     *     <destination_address_type>: ����ֵ�����պ������͡�
     *     <dcs>: ����ֵ�����뷽ʽ��
     *     <data>: �ַ������ͣ�USSD�ִ���
     * [ʾ��]:
     *       MO Call Control����ϱ�
     *       ^CCIN: 0,2,"","010203",145
     */
    { AT_STRING_CCIN, (VOS_UINT8 *)"^CCIN" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �±�״̬�����ϱ�����
     * [˵��]: �������������±�״̬�����仯ʱ�����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^THERM: <value><CR><LF>
     * [����]:
     *     <value>: 0���˳��¶ȱ�����
     *             1�������¶ȱ�����
     * [ʾ��]:
     *       �����¶ȱ���״̬�������ϱ����£�
     *       ^THERM:1
     *       �˳��¶ȱ���״̬�������ϱ����£�
     *       ^THERM:0
     */
    { AT_STRING_THERM, (VOS_UINT8 *)"^THERM" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����ϱ��ϵ�Э��ջ��ʼ�����
     * [˵��]: �����ϱ��ϵ�ʱЭ��ջ��ʼ�������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^PSINIT: <psinit_result><CR><LF>
     * [����]:
     *     <psinit_result>: ����ֵ���ϵ�Э��ջ��ʼ�������
     *             0����ʼ��ʧ�ܣ�
     *             1����ʼ���ɹ���
     *             2��������ʼ��ʧ�ܣ�V8R1ʹ�ã���
     *             3��������ʼ����ʱ
     * [ʾ��]:
     *       �ϵ�Э��ջ��ʼ���ɹ�
     *       ^PSINIT: 1
     *       �ϵ�Э��ջ��ʼ��ʧ��
     *       ^PSINIT: 0
     */
    { AT_STRING_PSINIT, (VOS_UINT8 *)"^PSINIT" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMSע�����л�ָʾ
     * [˵��]: IMSҵ������У�IMSע������л�ָʾ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSRATHO: <ho_status>,<source_rat>,<target_rat>,<cause_code><CR><LF>
     * [����]:
     *     <ho_status>: ����ֵ��IMSע������л�״̬��
     *             0�� RAT handover was successful
     *             1�� RAT handover failed
     *             2�� RAT handover could not be triggered
     *     <source_rat>: ����ֵ��ԭʼ��
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <target_rat>: ����ֵ��Ŀ����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <cause_code>: ����ֵ���л�ʧ��ԭ��
     * [ʾ��]:
     *       IMSע������WIFI�ɹ��л���LTE
     *       ^IMSRATHO: 0,1,0,0
     */
    { AT_STRING_IMSRATHO, (VOS_UINT8 *)"^IMSRATHO" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: IMS����״̬����ָʾ
     * [˵��]: IMS����״̬����ע��״̬�����仯�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^IMSSRVSTATUS: <sms_srv_status>,<sms_srv_rat>,<voip_srv_status>,<voip_srv_rat>,<vt_srv_status>,<vt_srv_rat>,<vs_srv_status>,<vs_srv_rat><CR><LF>
     * [����]:
     *     <sms_srv_status>: ����ֵ��IMS���ŵķ���״̬��
     *             0�� IMS SMS service is not available
     *             1�� IMS SMS is in limited service
     *             2�� IMS SMS is in full service
     *     <sms_srv_rat>: ����ֵ��IMS���ŵķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <voip_srv_status>: ����ֵ��IMS VoIP�ķ���״̬��
     *             0�� IMS VoIP service is not available
     *             1�� IMS VoIP is in limited service
     *             2�� IMS VoIP is in full service
     *     <voip_srv_rat>: ����ֵ��IMS VoIP�ķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <vt_srv_status>: ����ֵ��IMS VT�ķ���״̬��
     *             0�� IMS VT service is not available
     *             1�� IMS VT is in limited service
     *             2�� IMS VT is in full service
     *     <vt_srv_rat>: ����ֵ��IMS VT�ķ���״̬��
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     *     <vs_srv_status>: ����ֵ��IMS VS�ķ���״̬��
     *             0�� IMS VS service is not available
     *             1�� IMS VS is in limited service
     *             2�� IMS VS is in full service
     *     <vs_srv_rat>: ����ֵ��IMS VS�ķ�����
     *             0��IMS service is registered on LTE
     *             1��IMS service is registered on WIFI
     *             2��IMS service is registered onUtran
     *             3��IMS service is registered on Gsm
     *             4��IMS service is registered on NR
     * [ʾ��]:
     *       IMS�ķ���״̬�����仯
     *       ^IMSSRVSTATUS: 2,0,2,0,2,0,2,0
     */
    { AT_STRING_IMSSRVSTATUS, (VOS_UINT8 *)"^IMSSRVSTATUS" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ENDC˫����״̬�仯�����ϱ�
     * [˵��]: ����������LTE��ENDC˫����״̬�仯�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LENDC: <endc_available>,<endc_plmn_avail>,<endc_restricted>,<nr_pscell> <CR><LF>
     * [����]:
     *     <endc_available>: ����ֵ����ǰС���Ƿ�֧��Eutran-NRģʽ��
     *             0����֧�֣�
     *             1��֧�֡�
     *     <endc_plmn_avail>: PLMN LIST���Ƿ���PLMN֧��Eutran-NRģʽ��
     *             0������û��PLMN֧��Eutran-NRģʽ��
     *             1��������PLMN֧��Eutran-NRģʽ��
     *     <endc_restricted>: �������Ƿ�����Eutran-NR������
     *             0�����������û������endc������
     *             1���������������endc������
     *     <nr_pscell>: ����ֵ����ǰPSCell�Ƿ�ΪNR�����Ƿ����ENDC˫����״̬��
     *             0����ENDC״̬��
     *             1��ENDC�Ѿ�����״̬��
     * [ʾ��]:
     *       �����ϱ�ENDC˫����״̬�仯
     *       ^LENDC: 1,1,0,1
     */
    { AT_STRING_LENDC, (VOS_UINT8 *)"^LENDC" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: Modem����״̬�����ϱ�
     * [˵��]: Modem������״̬�����仯ʱ�����ϱ������
     *         ��������ṩ��Modem��Wifi�����߷���ʹ�ã������ϲ��ж�Wifi�����л�MIMO��SISOģʽ��
     *         ��ǰ����״̬�仯�漰��Ҫҵ���������£�CS����ͨ���С�CS��������С�CS�����ҵ��CS�򲹳�ҵ��LAU���̡�ȥע�����̡�RAU���̡�TAU���̡�PS������ҵ���������̡�PS�����ҵ��ȡ�
     *         ��������^CRRCONN����������ơ�
     *         �������ϱ�������״̬������NAS��ҵ����ʼ״̬��������ȫ��ͬ����������̬��
     *         ���������Modem0�������ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: Modem����״̬�����仯ʱ��
     *             <CR><LF>^CRRCONN: <status0>,<status1>,<status2> <CR><LF>
     * [����]:
     *     <status0>: ����ֵ��Modem0������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     *     <status1>: ����ֵ��Modem1������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     *     <status2>: ����ֵ��Modem2������״̬��
     *             0�������ӻ򼴽��˳�����̬��
     *             1�������ӻ򼴽���������̬��
     * [ʾ��]:
     *       Modem0�ɷ�����̬��������̬��Modem1��Modem2��Ϊ������̬
     *       ^CRRCONN: 1,0,0
     */
    { AT_STRING_CRRCONN, (VOS_UINT8 *)"^CRRCONN" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: VoLTE�����Ϣ�����ϱ�
     * [˵��]: AP��LTE�տ�������Ϣ�ϱ����غ�Modem���������ϱ����������֪ͨAP��VoLTE��Ƶͨ���пտ����ʵ�LPDCP�����Ϣ��
     *         ��������^VTRLQUALRPT������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LPDCPINFORPT: <CurrBuffTime>,<CurrBuffPktNum>, <MacUlThrput>,<MaxBuffTime><CR><LF>
     * [����]:
     *     <CurrBuffTime>: ����ֵ���������ݰ�����ʱ������λms��
     *     <CurrBuffPktNum>: ����ֵ����ǰ�������ݰ���������λ����
     *     <MacUlThrput>: ����ֵ��MAC�������ʣ�ֻ������������Ȩ���ʣ��������ش���Ȩ����λbytes/s��
     *     <MaxBuffTime>: ����ֵ�����ݰ���󻺴�ʱ������λms��
     * [ʾ��]:
     *       ����LTE�տ�������Ϣ�ϱ���
     *       ^LPDCPINFORPT: 1200,5,2048000,2000
     */
    { AT_STRING_LPDCPINFORPT, (VOS_UINT8 *)"^LPDCPINFORPT" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: LTE��·���������ϱ�
     * [˵��]: AP��LTE�տ�������Ϣ�ϱ����غ�Modem���������ϱ����������֪ͨAP��ǰLTE����·������Ϣ��
     *         ��������^VTRLQUALRPT������ơ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LTERLQUALINFO: <RSRP>,<RSRQ>,<RSSI>,<BLER> <CR><LF>
     * [����]:
     *     <RSRP>: ����ֵ���ο��źŽ��չ��ʣ���λdBm��
     *     <RSRQ>: ����ֵ���ο��źŽ�����������λdBm��
     *     <RSSI>: ����ֵ���ź�ǿ�ȣ���λdBm��
     *     <BLER>: ����ֵ�������ʡ�
     * [ʾ��]:
     *       ����LTE�տ�������Ϣ�ϱ���
     *       ^LTERLQUALINFO: -95,-15,-89,5
     */
    { AT_STRING_LTERLQUALINFO, (VOS_UINT8 *)"^LTERLQUALINFO" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ���ż�����ϱ�
     * [˵��]: ���ż��״̬�����ϱ���
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^JDINFO: <state>,<rat><CR><LF>
     * [����]:
     *     <state>: ����ֵ�����ż������
     *             0�����Ų�����
     *             1�����Ŵ���
     *     <rat>: ����ֵ��������ʽ��
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     * [ʾ��]:
     *       ���ż�⹦�ܿ����󣬼��GSM��ʽ���Ŵ���
     *       ^JDINFO: 1,0
     */
    { AT_STRING_JDINFO, (VOS_UINT8 *)"^JDINFO" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����ʼ��״̬�ϱ�
     * [˵��]: �����������ϱ���״̬��Ϣ(�������������͡�ICCID��IMSI��)��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CARDSTATUSIND: <value><CR><LF>
     * [����]:
     *     <value>: ��״̬����������
     * [ʾ��]:
     *       �����������ϱ�
     *       ^CARDSTATUSIND��10000000900000000000000010008094601101305756340000000000000000000000000000000000000000000000000000000000000000000000000090000000000000001000007520E8686FC50876100000000000000000000000000000000000000000000000000000000000000000000000009000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010108986111473021185793710000000
     */
    { AT_STRING_CARDSTATUSIND, (VOS_UINT8 *)"^CARDSTATUSIND" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����ϱ�LADN��Ϣ
     * [˵��]: Modem�յ�������·���LADN��Ϣ��������յ�ǰ������λ�����������ϱ���AP��ǰ����֧�ֵ�LADN DNN��Ϣ���͵�ǰ�����ֹʹ�õ�LADN DNN ��Ϣ��UE��λ������������LADN ������DNN����/��������Ϣ�仯��ModemҲʹ�ô������ϱ���AP��
     *         ע�⣺�����ϱ�����Ĭ�Ϲرգ������Ҫ�����ϱ���AP��Ҫʹ��^CLADN��������������ϱ����ܡ�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CLADNU: <allowed_ladn_dnn_num>,<allowed_dnn_list>,<nonallowed_ladn_dnn_num>, <nonallowed_dnn_list><CR><LF>
     * [����]:
     *     <allowed_ladn_num>: ����ֵ����ǰλ�����ڿ�������ʹ�õ�LADN DNN������
     *     <allowed_dnn_list>: �ַ������ͣ���ǰλ�����ڿ�������ʹ�õ�LADN DNN����ʽ���£���dnn1;dnn2;��dnn8��
     *     <nonallowed_ladn_dnn_num>: ����ֵ����ǰλ�����ڲ���������ʹ�õ�LADN DNN������allowed_ladn_num��nonallowed_ladn_num�ĺ���С�ڵ���8��
     *     <nonallowed_dnn_list>: �ַ������ͣ���ǰλ�����ڲ���������ʹ�õ�LADN DNN����ʽ���£���dnn1;dnn2;��dnn8��
     * [ʾ��]:
     *       ����/������LADN DNNͬʱ����ʱ
     *       ^CLADNU: 1,"Ladn.dnn.01",2,"Ladn.dnn.02;Ladn.dnn.03"
     *       NRȥע�ᡢ�������LADN��Ϣʱ
     *       ^CLADNU: 0,"",0,""
     */
    { AT_STRING_CLADNU, (VOS_UINT8 *)"^CLADNU" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ����С����Ϣ�����ϱ�
     * [˵��]: MODEM�жϵ�ǰLRRC��NR��С���Ƿ��б仯�����������ϱ�^ HSRCELLINFO��
     *         ע����������FEATRUE_MBB_HSRCELLINFO����ƣ���Ϊ��Ʒ�߶��ƴ򿪡�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^HSRCELLINFO: <Rat>,<HighSpeedFlg>,<Rsrp>,<CellIdLowBit>[,<CellIdHighBit>]<CR><LF>
     * [����]:
     *     <Rat>: ����ֵ����ǰ�����߽��뼼����
     *             0��GSM
     *             1��WCDMA
     *             2��LTE
     *             3��TD-SCDMA
     *             5��NR
     *             6��UNKNOWN
     *     <HighSpeedFlg>: ����С����ʶ��Ϣ������ֵ��ȡֵ��
     *             0���Ǹ���С��
     *             1������С��
     *     <Rsrp>: ����ֵ���ο��źŽ��չ��ʡ�
     *     <CellIdLowBit>: С��cell id������ֵ
     *             ���ratΪ��LTE�������ʾLTE��С��id��
     *             ���ratΪ��NR������<CellIdHighBit>��������ʾNR��С��id
     *     <CellIdHighBit>: ���ratΪ��NR���� ��<CellIdLowBit>��������ʾNR��С��id
     * [ʾ��]:
     *       �����ϱ�������·LTEС����Ϣ��Ϣ
     *       ^HSRCELLINFO: 2,1,-90,280
     *       �����ϱ�������·NRС����Ϣ��Ϣ
     *       ^HSRCELLINFO: 5,1,-90,280,0
     */
    { AT_STRING_HSRCELLINFO, (VOS_UINT8 *)"^HSRCELLINFO" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����ϱ�����֪ͨAP����������·�ָ��������SCG
     * [˵��]: �������ٻ�5G��NSA������������������SCG���������£�LRRC֪ͨAP��AP����������·�ָ��������SCG�Ĳ�����5G��ʹ�á�
     * [�﷨]:
     *     [����]: -
     *     [���]: ���ؽ����
     *             <CR><LF>^CONNECTRECOVERY <CR><LF>
     * [����]:
     * [ʾ��]:
     *       ������������SCG���������£������ϱ�����
     *       ^CONNECTRECOVERY
     */
    { AT_STRING_CONNECTRECOVERY, (VOS_UINT8 *)"^CONNECTRECOVERY" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ͨ������״̬֪ͨ����
     * [˵��]: �ϱ�ͨ������ҵ��״̬��Ϣ��AP�࣬�磺ͨ����������֧��������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^FUSIONCALLRAWU: <msg_len>��<msg_id>,<msg_context><CR><LF>
     * [����]:
     *     <msg_id>: ��Ӧ����ģ��
     *             0��IMSA
     *             1��IMS SDK
     *     <msg_len>: ��Ϣ����<msg_context>�ĳ��ȣ���Ϣ������󲻳���500
     *     <msg_context>: ģ���ӿ���Ϣ����
     * [ʾ��]:
     *       �ϱ�AP������ͨ������ҵ����
     *       ^FUSIONCALLRAWU: 0,16,"01010000000001000100000000000000"
     */
    { AT_STRING_FUSIONCALLRAWU, (VOS_UINT8 *)"^FUSIONCALLRAWU" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: ��������λ����Ϣ�ϱ�
     * [˵��]: ���������ڵ�һ���޿��������Ѻ󣬽���õ�λ����Ϣ�ϱ���AP�ࡣ��^PSINIT�����ϱ�ǰ�ϱ���
     * [�﷨]:
     *     [����]:
     *     [���]: <CR><LF>^INITLOCINFO: <Mcc>,<Sid><CR><LF>
     * [����]:
     *     <Mcc>: �ֻ����ڵĵ�ǰλ�õĹ����룬ͨ��GUL�����õ�����Чֵ��ȫF
     *     <Sid>: �ֻ����ڵĵ�ǰλ�õ�Sid��ͨ��Xģ�����õ�����Чֵ��-1
     * [ʾ��]:
     *       �ϱ�INITLOCINFO���MCC��SID������Чֵ
     *       ^INITLOCINFO: 460,23
     *       �ϱ�INITLOCINFO���MCC����Чֵ��SID����Чֵ
     *       ^INITLOCINFO: 460,-1
     *       �ϱ�INITLOCINFO���MCC����Чֵ��SID����Чֵ
     *       ^INITLOCINFO: fff,23
     */
    { AT_STRING_INITLOCINFO, (VOS_UINT8 *)"^INITLOCINFO" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: LTE����Ƶ����Ϣ�����ϱ�
     * [˵��]: ��LTE����Ƶ�������˳�WiFiƵ�ʸ��ŷ�Χʱ�������ϱ�^LWURC����֪ͨӦ�ã������WiFiģ���ͬƵ���š�LTE��WiFi��Ƶ�ʸ��ŷ�ΧĬ����(23700-24000)����ͨ��NV�̬������
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LWURC: <state>,<ulfreq>,<ulbw>,<dlfreq>,<dlbw>,<band>,<ant_num>,<ant1_ulfreq>,<ant1_ulbw>,<ant1_dlfreq>,<ant1_dlbw>,<ant2_ulfreq>,<ant2_ulbw>,<ant2_dlfreq>,<ant2_dlbw>,<ant3_ulfreq>,<ant3_ulbw>,<ant3_dlfreq>,<ant3_dlbw>,<ant4_ulfreq>,<ant4_ulbw>,<ant4_dlfreq>,<ant4_dlbw>,<ant_dlmm><ant1_dlmm><ant2_dlmm><ant3_dlmm><ant4_dlmm>,<dl256QAMflag><CR><LF>
     * [����]:
     *     <state>: ��ǰLTEƵ����WIFIƵ�ʳ�ͻ״̬��
     *             0������ͻ����LTE��WIFI������Ƶ�ʸ��ŷ�Χ��
     *             1����ͻ����LTE��WIFI����Ƶ�ʸ��ŷ�Χ��
     *             2����Чֵ����ǰLTE�ǽ���״̬���޹̶�������Ƶ�ʣ���
     *     <ulfreq>: LTE����Ƶ�ʣ���λ100kHz��ȡֵ��Χ��0~4294967295��
     *     <ulbw>: LTE���д���
     *             0��1.4M��
     *             1��3M��
     *             2��5M��
     *             3��10M��
     *             4��15M��
     *             5��20M��
     *     <dlfreq>: LTE����Ƶ�ʣ���λ100kHz��ȡֵ��Χ��0~4294967295��
     *     <dlbw>: LTE���д���
     *             0��1.4M��
     *             1��3M��
     *             2��5M��
     *             3��10M��
     *             4��15M��
     *             5��20M��
     *     <band>: LTEģʽ��Ƶ�κţ�ȡֵ��Χ��1~70��
     *     <ant_num>: ���ߵ�Ƶ���Ƶ�θ�����ȡֵ��Χ0~3��
     *     <dlmm>: UE MIMO����
     *     <dl256QAMflag>: �Ƿ�֧������256QAM��
     *             0����֧�֣�
     *             1��֧�֡�
     */
    { AT_STRING_LWURC, (VOS_UINT8 *)"^LWURC" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: �����ϱ��ź�������Ϣ
     * [˵��]: �����������ϱ��ź�������Ϣ��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^ANLEVEL: <rscp><ecno><rssi><level><rsrp><rsrq><CR><LF>
     * [����]:
     *     <rscp>: ����ֵ��RSCPֵ����λΪ��dBm��
     *     <ecno>: ����ֵ��ECNOֵ����λΪ��dBm��
     *     <rssi>: ����ֵ���ź�ǿ�ȣ���λ��dBm��
     *     <level>: ����ֵ���ź������ȼ���
     *     <rsrp>: ����ֵ���ο��źŽ��չ��ʣ���λ��dBm��
     *     <rsrq>: ����ֵ���ο��źŽ�����������λ��dB��
     * [ʾ��]:
     *       ^ANLEVEL: 0,99,31,4,69,3
     */
    { AT_STRING_ANLEVEL, (VOS_UINT8 *)"^ANLEVEL" },
    { AT_STRING_CARDFETCHIND, (VOS_UINT8 *)"^CARDFETCHIND" },
    /*
     * [���]: Э��AT-LTE���
     * [����]: LTEС��CA״̬�����ϱ�
     * [˵��]: ����Modem�����ϱ�LTEС�����С�����CA����״̬��CA����״̬��
     *         �仯����������
     *         ����CA���ô򿪺͹رա�����CA���ô򿪺͹رա�CA�����ȥ���
     *         �������ϱ���^CURC���ƣ��ɴ򿪻��߹رա�
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^LCACELLURC: "<cell_id> <ul_cfg> <dl_cfg> <act>"[, "<cell_id> <ul_cfg> <dl_cfg> <act>"[,......]]<CR><LF>
     *             <CR><LF>OK<CR><LF>
     * [����]:
     *     <cell_id>: ����ֵ��cell id��0��ʾPcell��������ʾScell��
     *     <ul_cfg>: ����ֵ����cell����CA�Ƿ����ã�0��ʾδ���ã�1��ʾ�����á�
     *     <dl_cfg>: ����ֵ����cell����CA�Ƿ����ã�0��ʾδ���ã�1��ʾ�����á�
     *     <act>: ����ֵ����cell CA�Ƿ񱻼��0��ʾδ���1��ʾ�Ѽ��
     * [ʾ��]:
     *       �����ϱ�LTEС��CA����״̬
     *       ^LCACELLURC: "0 0 0 0", "1 1 1 1", "2 1 1 1", "3 0 0 0", "4 0 0 0", "5 0 0 0", "6 0 0 0", "7 0 0 0"
     */
    { AT_STRING_LCACELLURC, (VOS_UINT8 *)"^LCACELLURC" },
    /*
     * [���]: Э��AT-��AP�Խ�
     * [����]: �����ϱ���λ��Ϣ
     * [˵��]: ����������AP-Modem��̬�£�Modem��AP�����ϱ��������ָʾ����λ��Ϣ��
     *         ����ʹ�ó�����Modem���յ�����Ĳ���ָʾ����λ��Ϣ�������ϱ���
     *         ����ʹ�����ƣ�ֻ�޶���AP-Modem��̬ʹ�á�
     *         ����ʹ��ͨ����ֻ��ͨ����AP�Խӵ�ATͨ����MUXͨ���·���
     *         ���������3GPP TS 27.007Э�顣
     * [�﷨]:
     *     [����]:
     *     [���]: <CR><LF>+CPOSR: <xml><CR><LF>
     * [����]:
     *     <xml>: �ַ������ͣ�һ��XML���ո�ʽ���ݣ������ʽ���3GPP 27007Э��8.56�½�������
     * [ʾ��]:
     *       ��λ��Ϣ�����ϱ�
     *       +CPOSR: <?xml version="1.0" encoding="utf-8"?><pos><pos_meas><RRLP_meas><RRLP_pos_instruct><RRLP_method_type><ms_based><method_accuracy>0</method_accuracy></ms_based></RRLP_method_type><RRLP_method literal="gps"/><resp_time_seconds>7</resp_time_seconds><mult_sets literal="one"/></RRLP_pos_instruct></RRLP_meas></pos_meas></pos>
     */
    { AT_STRING_CPOSR, (VOS_UINT8 *)"+CPOSR" },
    /*
     * [���]: װ��AT-GUCװ��
     * [����]: �����ϱ�CLT�����߱ջ���г��״̬��Ϣ
     * [˵��]: �����ϱ���ǰMT��λCLT״̬��Ϣ(ֻ��WPHY��LPHY�Ż��ϱ�)��
     * [�﷨]:
     *     [����]: -
     *     [���]: <CR><LF>^CLTINTO:  <GammaReal>, <GammaImag>, <GammaAmpUc0>, <GammaAmpUc1>, <GammaAmpUc2>, <GammaAntCoarseTune>, <FomcoarseTune>, <CltAlgState>, <CltDetectCount>, <Dac0>, <Dac1>, <Dac2>, <Dac3> <CR><LF>
     * [����]:
     *     <GammaReal>: �з�������ֵ������ϵ��ʵ��
     *     <GammaImag>: �з�������ֵ������ϵ���鲿
     *     <GammaAmpUc0>: �޷�������ֵ��פ����ⳡ��0����ϵ������
     *     <GammaAmpUc1>: �޷�������ֵ��פ����ⳡ��1����ϵ������
     *     <GammaAmpUc2>: �޷�������ֵ��פ����ⳡ��2����ϵ������
     *     <GammaAntCoarseTune>: �޷�������ֵ���ֵ����λ��
     *     <FomcoarseTune>: �޷�������ֵ���ֵ�FOMֵ
     *     <CltAlgState>: �޷�������ֵ���ջ��㷨����״̬
     *     <CltDetectCount>: �޷�������ֵ���ջ������ܲ���
     *     <Dac0>: �޷�������ֵ��Dac0
     *     <Dac1>: �޷�������ֵ��Dac1
     *     <Dac2>: �޷�������ֵ��Dac2
     *     <Dac3>: �޷�������ֵ��Dac3
     * [ʾ��]:
     *       �����ϱ�
     *       ^CLTINFO: 3150,0,4465,2214,3150,22536,185,17,3,106,11,0,0
     */
    { AT_STRING_CLTINFO, (VOS_UINT8 *)"^CLTINFO" },

    { AT_STRING_BUTT, (TAF_UINT8 *)"\"\"" },
};

VOS_UINT8       g_atCrLf[]        = "\r\n";         /* �س����� */
const TAF_UINT8 g_atCmeErrorStr[] = "+CME ERROR: "; /* ������ʾ�ִ� */
const TAF_UINT8 g_atCmsErrorStr[] = "+CMS ERROR: "; /* ������ʾ�ִ� */

const TAF_UINT8 g_atTooManyParaStr[] = "TOO MANY PARAMETERS"; /* ����̫����ʾ�ִ� */

AT_CMD_FORMAT_TYPE g_atCmdFmtType = AT_CMD_TYPE_BUTT;

AT_SEND_DataBuffer g_atSendDataBuff;                       /* ��������ķ�����Ϣ�洢�� */
AT_SEND_DataBuffer g_atCombineSendData[AT_MAX_CLIENT_NUM]; /* �������ķ�����Ϣ�洢�� */

/* g_atSendDataBuff.aucBuffer�ĵ�0��ΪMUX��֡ͷ��־ */
TAF_UINT8 *g_atSndCrLfAddr    = &g_atSendDataBuff.buffer[AT_SEND_DATA_BUFF_INDEX_1];
TAF_UINT8 *g_atSndCodeAddress = &g_atSendDataBuff.buffer[AT_SEND_DATA_BUFF_INDEX_3];

TAF_LogPrivacyAtCmdUint32 g_logPrivacyAtCmd = TAF_LOG_PRIVACY_AT_CMD_BUTT;

/*
 * ���õ���Ĵ������ݷ��ͽӿ�ʱ���������ָ�����Ϊ4�ֽڶ����ʽ��
 * ��g_atSndCrLfAddr��g_atSndCodeAddress��û������4�ֽڶ��룬������
 * ��һ�ο���������(AT����������Ҫ�󲻸ߣ�������һ�ο����ǿ��е�)
 */
TAF_UINT8 g_atSendDataBuffer[AT_CMD_MAX_LEN];

AT_V_TYPE    g_atVType    = AT_V_ENTIRE_TYPE; /* ָʾ����������� */
AT_CSCS_TYPE g_atCscsType = AT_CSCS_IRA_CODE; /* ָʾTE�������� */

AT_CMD_ECHO_TYPE g_atEType = AT_E_ECHO_CMD;
VOS_UINT32       g_atXType = 0; /* CONNECT <text> result code is given upon entering online data state.
         Dial tone and busy detection are disabled. */
VOS_UINT32 g_atQType = 0;       /* DCE transmits result codes */

MN_OPERATION_ID_T g_opId = 0;

#if (FEATURE_MBB_CUST == FEATURE_ON)
VOS_UINT8 g_modemLoop = 0;
#endif

AT_CSTA_NumTypeUint8 g_atCstaNumType = (0x80 | (MN_CALL_TON_INTERNATIONAL << 4) | (MN_CALL_NPI_ISDN));
AT_PB_Info           g_pbatInfo;


TAF_UINT8 g_atPppIndexTab[AT_MAX_CLIENT_NUM]; /* PppId��Index�Ķ�Ӧ���±���PppId */

/* ��¼�Ƿ��������ʼֵΪδ������ͨ������^DATALOCK�����ɹ����޸ĸñ��� */
VOS_BOOL g_atDataLocked = VOS_TRUE;

#if (FEATURE_SECURITY_SHELL == FEATURE_ON)
AT_SP_WordCtx g_spWordCtx = {0};
#endif

LOCAL AT_RreturnCodeUint32 AT_ProcNetWorkTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcPppOrgTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcAtdIpCallTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcPsCallEndTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcTmodeTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcAbortCmdTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcUnkonwnTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcCpbrTimerOut(VOS_UINT8 indexNum);
LOCAL AT_RreturnCodeUint32 AT_ProcVmsetTimerOut(VOS_UINT8 indexNum);

static const AT_PS_InternalTimeoutResult g_atInterTimeoutResultTab[] = {
    /* Inter Timer Name */ /* ��Ϣ������ */
    { AT_S0_TIMER, AT_RcvTiS0Expired },
    { AT_HOLD_CMD_TIMER, AT_BlockCmdTimeOutProc },
    { AT_SIMLOCKWRITEEX_TIMER, AT_RcvTiSimlockWriteExExpired },
#if (FEATURE_AT_HSUART == FEATURE_ON)
    { AT_VOICE_RI_TIMER, AT_RcvTiVoiceRiExpired },
    { AT_SMS_RI_TIMER, AT_RcvTiSmsRiExpired },
#endif
#if (FEATURE_LTE == FEATURE_ON)
    { AT_SHUTDOWN_TIMER, AT_RcvTiShutDownExpired },
#endif
    { AT_AUTH_PUBKEY_TIMER, AT_RcvTiAuthPubkeyExpired },
};

static const AT_PS_CmdTimeoutResult g_atCmdTimeoutResultTab[] = {
    /* cmd Timer Name */ /* ��Ϣ������ */
    { AT_CMD_COPS_SET_AUTOMATIC, AT_ProcNetWorkTimerOut },
    { AT_CMD_COPS_SET_MANUAL, AT_ProcNetWorkTimerOut },
    { AT_CMD_COPS_SET_DEREGISTER, AT_ProcNetWorkTimerOut },
    { AT_CMD_COPS_SET_MANUAL_AUTOMATIC_MANUAL, AT_ProcNetWorkTimerOut },
    { AT_CMD_CUSD_REQ, AT_ProcNetWorkTimerOut },
    { AT_CMD_PPP_ORG_SET, AT_ProcPppOrgTimerOut },
    { AT_CMD_D_IP_CALL_SET, AT_ProcAtdIpCallTimerOut },
    { AT_CMD_H_PS_SET, AT_ProcPsCallEndTimerOut },
    { AT_CMD_PS_DATA_CALL_END_SET, AT_ProcPsCallEndTimerOut },
#if (FEATURE_LTE == FEATURE_ON)
    { AT_CMD_SET_TMODE, AT_ProcTmodeTimerOut },
#endif
    { AT_CMD_COPS_ABORT_PLMN_LIST, AT_ProcAbortCmdTimerOut },
    { AT_CMD_NETSCAN_ABORT, AT_ProcAbortCmdTimerOut },
    { AT_CMD_CMGS_TEXT_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CMGS_PDU_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CMGC_TEXT_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CMGC_PDU_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CMSS_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CMST_SET, AT_ProcUnkonwnTimerOut },
    { AT_CMD_CPBR2_SET, AT_ProcCpbrTimerOut },
    { AT_CMD_CPBR_SET, AT_ProcCpbrTimerOut },
    { AT_CMD_VMSET_SET, AT_ProcVmsetTimerOut },
};

static const AT_MnOptTypeConvertAtCmdOptType g_atMnOptTypeConvertAtCmdTypeTab[] = {
    { TAF_MMI_CALL_ORIG, AT_CMD_D_CS_VOICE_CALL_SET },
    { TAF_MMI_CALL_CHLD_REQ, AT_CMD_CHLD_SET },
    { TAF_MMI_CHANGE_PIN, AT_CMD_CPIN_UNBLOCK_SET },
    { TAF_MMI_CHANGE_PIN2, AT_CMD_CPIN2_UNBLOCK_SET },
    { TAF_MMI_UNBLOCK_PIN, AT_CMD_CPIN_VERIFY_SET },
    { TAF_MMI_UNBLOCK_PIN2, AT_CMD_CPIN2_VERIFY_SET },
    { TAF_MMI_REGISTER_SS, AT_CMD_SS_REGISTER },
    { TAF_MMI_ERASE_SS, AT_CMD_SS_ERASE },
    { TAF_MMI_ACTIVATE_SS, AT_CMD_SS_ACTIVATE },
    { TAF_MMI_DEACTIVATE_SS, AT_CMD_SS_DEACTIVATE },
    { TAF_MMI_INTERROGATE_SS, AT_CMD_SS_INTERROGATE },
    { TAF_MMI_REGISTER_PASSWD, AT_CMD_SS_REGISTER_PSWD },
    { TAF_MMI_PROCESS_USSD_REQ, AT_CMD_CUSD_REQ },
    { TAF_MMI_SUPPRESS_CLIP, AT_CMD_CLIP_READ },
    { TAF_MMI_INVOKE_CLIP, AT_CMD_CLIP_READ },
    { TAF_MMI_DEACTIVATE_CCBS, AT_CMD_SS_DEACTIVE_CCBS },
    { TAF_MMI_INTERROGATE_CCBS, AT_CMD_SS_INTERROGATE_CCBS },
};

/*
 * 3 ��������������
 */

VOS_UINT8 *AT_GetCrlf(VOS_VOID)
{
    return g_atCrLf;
}
TAF_UINT8 *AT_GetSendDataBuffer(VOS_VOID)
{
    return g_atSendDataBuffer;
}
AT_SEND_DataBuffer *AT_GetSendDataBuff(VOS_VOID)
{
    return &g_atSendDataBuff;
}
TAF_UINT8 *AT_GetSndCodeAddress(VOS_VOID)
{
    return g_atSndCodeAddress;
}

MN_OPERATION_ID_T At_GetOpId(VOS_VOID)
{
    g_opId++;
    g_opId %= MN_OP_ID_BUTT;
    if (g_opId == 0) {
        g_opId++;
    }
    return (g_opId);
}

VOS_VOID AT_StopTimerCmdReady(VOS_UINT8 indexNum)
{
    AT_StopRelTimer(indexNum, &g_atClientTab[indexNum].hTimer);
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_atClientTab[indexNum].opId          = 0;
#if (FEATURE_IOT_ATNLPROXY == FEATURE_ON)
    if (g_current_user_at == indexNum) {
        g_atClientTab[AT_CLIENT_ID_APP].isWaitAts = 0;
        g_current_user_at = -1;
    }
#endif
}

TAF_UINT32 At_ClientIdToUserId(TAF_UINT16 clientId, TAF_UINT8 *indexId)
{
    TAF_UINT8 i;
    *indexId = 0;

    /* �ж��Ƿ�Ϊ�㲥client id */
    /* MODEM 0�Ĺ㲥ClientId */
    if (clientId == AT_BROADCAST_CLIENT_ID_MODEM_0) {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
        return AT_SUCCESS;
    }

    /* MODEM 1�Ĺ㲥ClientId */
    if (clientId == AT_BROADCAST_CLIENT_ID_MODEM_1) {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_1;
        return AT_SUCCESS;
    }

    /* MODEM 2�Ĺ㲥ClientId */
    if (clientId == AT_BROADCAST_CLIENT_ID_MODEM_2) {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_2;
        return AT_SUCCESS;
    }

    /* �����û������ */
    for (i = 0; i < AT_MAX_CLIENT_NUM; i++) {
        if (g_atClientTab[i].used == AT_CLIENT_NULL) {
            continue;
        }
        if (g_atClientTab[i].clientId == clientId) {
            *indexId = i; /* ���ؽ�� */
            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}

TAF_UINT32 At_ClientIdToUserBroadCastId(TAF_UINT16 clientId, TAF_UINT8 *indexId)
{
    VOS_UINT32    rst;
    ModemIdUint16 modemId = MODEM_ID_0;

    rst = AT_GetModemIdFromClient(clientId, &modemId);
    if (rst != VOS_OK) {
        AT_ERR_LOG1("At_ClientIdToUserBroadCastId:Get ModemID From ClientID fail,ClientID=%d", clientId);
        return AT_FAILURE;
    }

    if (modemId == MODEM_ID_0) {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_0;
    }
#if (MULTI_MODEM_NUMBER > 1)
    else if (modemId == MODEM_ID_1) {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_1;
    }
#if (MULTI_MODEM_NUMBER > 2)
    else {
        *indexId = AT_BROADCAST_CLIENT_INDEX_MODEM_2;
    }
#endif
#endif

    return AT_SUCCESS;
}

DMS_PortIdUint16 AT_GetDmsPortIdByClientId(AT_ClientIdUint16 clientId)
{
    return (clientId >= AT_CLIENT_ID_BUTT) ? DMS_PORT_BUTT : g_atClientTab[clientId].portNo;
}

TAF_VOID At_SendReportMsg(TAF_UINT8 type, TAF_UINT8 *data, TAF_UINT16 len)
{
    MN_AT_IndEvt *event = VOS_NULL_PTR;
    VOS_UINT_PTR  tmpAddr;
    errno_t       memResult;

    /* �����Զ����ITEM����4���ֽ� */
    /*lint -save -e516 */
    event = (MN_AT_IndEvt *)TAF_AllocMsgWithoutHeaderLen(WUEPS_PID_AT, (len + AT_MN_INTERFACE_MSG_HEADER_LEN));
    /*lint -restore */
    if (event == TAF_NULL_PTR) {
        AT_ERR_LOG("At_SendCmdMsg:ERROR:Alloc Msg");
        return;
    }

    TAF_CfgMsgHdr((MsgBlock *)event, AT_GetDestPid(((SI_PB_EventInfo *)data)->clientId, I0_WUEPS_PID_TAF), WUEPS_PID_AT,
                  (len + AT_MN_INTERFACE_MSG_HEADER_LEN));
    event->msgName = type;
    event->len     = len;

    tmpAddr = (VOS_UINT_PTR)event->content;
    if (len > 0) {
        memResult = memcpy_s((VOS_UINT8 *)tmpAddr, len, data, len);
        TAF_MEM_CHK_RTN_VAL(memResult, len, len);
    }

    /* ������Ϣ��AT_PID; */
    if (TAF_TraceAndSendMsg(WUEPS_PID_AT, event) != 0) {
        AT_ERR_LOG("At_SendReportMsg:ERROR");
        return;
    }
}

/*
 * Description: ɨ���˸��
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_UINT32 At_ScanDelChar(TAF_UINT8 *data, TAF_UINT16 *len)
{
    TAF_UINT32 chkLen  = 0;
    TAF_UINT16 lenTemp = 0;
    TAF_UINT8 *write   = data;
    TAF_UINT8 *read    = data;

    if (*len == 0) {
        return AT_FAILURE;
    }

    /* ���������� */
    while (chkLen++ < *len) {
        if (g_atS5 == *read) { /* ɾ����һ�ַ� */
            if (lenTemp > 0) {
                write--;
                lenTemp--;
            }
        } else { /* ѡ����Ч�ַ� */
            *write++ = *read;
            lenTemp++;
        }
        read++;
    }
    *len = lenTemp;
    return AT_SUCCESS;
}

/*
 * Description: ɨ����Ʒ�
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_UINT32 At_ScanCtlChar(TAF_UINT8 *data, TAF_UINT16 *len)
{
    TAF_UINT8 *write   = data;
    TAF_UINT8 *read    = data;
    TAF_UINT32 chkLen  = 0;
    TAF_UINT16 lenTemp = 0;

    if (*len == 0) {
        return AT_FAILURE;
    }

    /* ��� */
    while (chkLen++ < *len) {
        /* ѡ����Ч�ַ� */
        if (*read >= 0x20) {
            *write++ = *read;
            lenTemp++;
        }
        read++;
    }
    *len = lenTemp;
    return AT_SUCCESS;
}

TAF_UINT32 At_ScanBlankChar(TAF_UINT8 *data, TAF_UINT16 *len)
{
    TAF_UINT8 *check     = data;
    TAF_UINT8 *write     = data;
    TAF_UINT8 *read      = data;
    TAF_UINT32 chkQout   = 0;
    TAF_UINT32 chkLen    = 0;
    TAF_UINT32 qoutNum   = 0;
    TAF_UINT16 qoutCount = 0;
    TAF_UINT16 lenTemp   = 0;

    if (*len == 0) {
        return AT_FAILURE;
    }

    /* ������� */
    while (qoutNum++ < *len) {
        if (*check++ == '"') {
            chkQout++;
        }
    }
    /* ��ֵֻ��Ϊż�����������ش��� */
    if ((chkQout % 2) == 1) {
        return AT_FAILURE;
    }

    /* ��� */
    while (chkLen++ < *len) {
        /* ��ǰ�ǵڼ�������״̬ */
        if (*read == '\"') {
            qoutCount++;
        }

        /* ȥ��������Ŀո�,��2��Ϊ���ж���ż�� */
        if (((*read != AT_BLANK_ASCII_VALUE) && ((qoutCount % 2) == 0)) || ((qoutCount % 2) == 1)) {
            *write++ = *read;
            lenTemp++;
        }
        read++;
    }
    *len = lenTemp;
    return AT_SUCCESS;
}

VOS_UINT32 AT_ConvertCharToHex(VOS_UINT8 charValue, VOS_UINT8 *hexValue)
{
    if ((charValue >= '0') && (charValue <= '9')) {
        *hexValue = charValue - '0';
    } else if ((charValue >= 'a') && (charValue <= 'f')) {
        /* ת��'a'��'f'Ϊ���� */
        *hexValue = (charValue - 'a') + AT_DECIMAL_BASE_NUM; /* ���ַ���ת����ʮ������ */
    } else if ((charValue >= 'A') && (charValue <= 'F')) {
        /* ת��'a'��'f'Ϊ���� */
        *hexValue = (charValue - 'A') + AT_DECIMAL_BASE_NUM; /* ���ַ���ת����ʮ������ */
    } else {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_ParseSimLockPara(VOS_UINT8 *data, VOS_UINT16 len, AT_ParseParaType *paralist, VOS_UINT32 paraCnt)
{
    VOS_UINT8 *paraStart = VOS_NULL_PTR;
    VOS_UINT32 paraIdx;
    VOS_UINT32 chkRslt;
    VOS_UINT32 i;

    if (paralist == VOS_NULL_PTR) {
        AT_ERR_LOG("At_PareseSimLockPara: pstParalist = VOS_NULL_PTR!");
        return AT_FAILURE;
    }

    paraStart = data;
    paraIdx   = 0;

    /* ��AT^simlock=pucData��pucData��ʼ����  */
    for (i = 0; i < len; i++) {
        if (data[i] == ',') {
            /* ��ǰ�����ĳ��� */
            chkRslt = At_CheckAndParsePara((paralist + paraIdx), paraStart, (data + i));
            if (chkRslt != AT_OK) {
                return AT_FAILURE;
            }

            paraStart = (data + i) + 1;
            ++paraIdx;

            if (paraIdx >= paraCnt) {
                return AT_FAILURE;
            }
        }
    }

    /*  �����һ�β����޸�copy�������б��� */
    chkRslt = At_CheckAndParsePara((paralist + paraIdx), paraStart, (data + len));
    if (chkRslt != AT_OK) {
        return AT_FAILURE;
    }

    ++paraIdx;

    if (paraIdx > paraCnt) {
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_DispatchSimlockCmd(VOS_UINT8 indexNum, VOS_UINT32 paraCnt, AT_ParseParaType *paralist)
{
    VOS_UINT8 simLockOP;

    if ((paraCnt < 1) || (paralist == VOS_NULL_PTR)) {
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_FAILURE;
    }

    if ((paralist[0].paraLen != 1) || (paralist[0].para[0] < '0') || (paralist[0].para[0] > '2')) {
        /* ������� */
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_FAILURE;
    }

    simLockOP = paralist[0].para[0] - '0';

    if (simLockOP == AT_SIMLOCK_OPRT_UNLOCK) {
        At_UnlockSimLock(indexNum, paraCnt, paralist);
    } else if (simLockOP == AT_SIMLOCK_OPRT_SET_PLMN_INFO) {
        At_SetSimLockPlmnInfo(indexNum, paraCnt, paralist);
    } else {
        AT_GetSimLockStatus(indexNum);
    }
    return AT_SUCCESS;
}

/*
 * ��������: ����ַ��������Լ�ͷ������
 */
LOCAL VOS_UINT32 At_CheckSimLockPara(VOS_UINT8 *dataPara, const VOS_UINT8 *data, VOS_UINT16 len, VOS_UINT16 *cmdLen)
{
    VOS_INT8 ret;
    errno_t memResult;

    memResult = memcpy_s(dataPara, len, data, len);
    TAF_MEM_CHK_RTN_VAL(memResult, len, len);

    /* ��������ַ�������С�ڵ���"AT^SIMLOCK"����ֱ�ӷ���AT_FAILURE */
    (*cmdLen) = (VOS_UINT16)VOS_StrLen("AT^SIMLOCK=");

    if (len <= (*cmdLen)) {
        (VOS_VOID)memset_s(dataPara, len, 0x00, len);
        return AT_FAILURE;
    }

    /* ��������ַ���ͷ������"AT^SIMLOCK"ֱ�ӷ���AT_FAILURE */
    ret = VOS_StrNiCmp((VOS_CHAR *)dataPara, "AT^SIMLOCK=", (*cmdLen));

    if (ret != 0) {
        (VOS_VOID)memset_s(dataPara, len, 0x00, len);
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_ProcSimLockPara(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT16        cmdLen = 0;
    VOS_UINT8        *dataPara = TAF_NULL_PTR;
    VOS_UINT32        paraCnt;
    AT_ParseParaType *paralist = VOS_NULL_PTR;

    if (len == 0) {
        return AT_FAILURE;
    }
    /*lint -save -e516 */
    dataPara = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, len);
    /*lint -restore */
    if (dataPara == VOS_NULL_PTR) {
        AT_ERR_LOG("At_ProcSimLockPara: pucDataPara Memory malloc failed!");
        return AT_FAILURE;
    }

    (VOS_VOID)memset_s(dataPara, len, 0x00, len);

    /* ����ַ��������Լ�ͷ������ */
    if (At_CheckSimLockPara(dataPara, data, len, &cmdLen) == AT_FAILURE) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        return AT_FAILURE;
    }

    AT_SaveCmdElementInfo(indexNum, (VOS_UINT8 *)"^SIMLOCK", AT_EXTEND_CMD_TYPE);

    /* ���������� */
    paraCnt = At_GetParaCnt(dataPara, len);

    if (paraCnt < 1) {
        (VOS_VOID)memset_s(dataPara, len, 0x00, len);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_SUCCESS;
    }

    /* ��������ǳ���61������ʧ�� */
    if (paraCnt > AT_SIMLOCK_PLMN_MAX_NUM) {
        (VOS_VOID)memset_s(dataPara, len, 0x00, len);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        At_FormatResultData(indexNum, AT_SIMLOCK_PLMN_NUM_ERR);
        return AT_SUCCESS;
    }

    paralist = (AT_ParseParaType *)PS_MEM_ALLOC(WUEPS_PID_AT, (paraCnt * sizeof(AT_ParseParaType)));

    if (paralist == VOS_NULL_PTR) {
        AT_ERR_LOG("At_ProcSimLockPara: pstParalist Memory malloc failed!");
        (VOS_VOID)memset_s(dataPara, len, 0x00, len);
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_SUCCESS;
    } else {
        (VOS_VOID)memset_s(paralist, paraCnt * sizeof(AT_ParseParaType), 0x00,
            (VOS_SIZE_T)(paraCnt * sizeof(AT_ParseParaType)));
    }

    /* �� At^simlock�Ĳ��������� At��ʽ�Ĳ����б��� */
    if (At_ParseSimLockPara((dataPara + cmdLen), (len - cmdLen), paralist, paraCnt) == AT_SUCCESS) {
        /* ����at^simlock=oprt,paralist�е�oprt�ַ� Simlock������� */
        At_DispatchSimlockCmd(indexNum, paraCnt, paralist);
    } else {
        At_FormatResultData(indexNum, AT_ERROR);
    }

    /*lint -save -e516 */
    (VOS_VOID)memset_s(paralist, paraCnt * sizeof(AT_ParseParaType), 0x00, (VOS_SIZE_T)(paraCnt * sizeof(AT_ParseParaType)));
    (VOS_VOID)memset_s(dataPara, len, 0x00, len);
    PS_MEM_FREE(WUEPS_PID_AT, paralist);
    PS_MEM_FREE(WUEPS_PID_AT, dataPara);
    /*lint -restore */

    return AT_SUCCESS;
}

/*
 * ��������: ��ʼ������
 */
LOCAL VOS_UINT32 AT_HandleFacAuthPubKeyExCmdInit(VOS_UINT8 indexNum, VOS_UINT16 *cmdlen,
    AT_ParseCmdNameType *atCmdName)
{
    VOS_UINT32 tempIndex;
    VOS_UINT32 timerName;

    (*cmdlen) = (VOS_UINT16)VOS_StrLen("AT^FACAUTHPUBKEYEX=");
    tempIndex = (VOS_UINT32)indexNum;
    timerName = AT_AUTH_PUBKEY_TIMER;
    timerName |= AT_INTERNAL_PROCESS_TYPE;
    timerName |= (tempIndex << 12);
    (VOS_VOID)memset_s(atCmdName, sizeof(AT_ParseCmdNameType), 0x00, sizeof(AT_ParseCmdNameType));

    return timerName;
}

/*
 * ��������: ���������Ƿ�ΪAT^FACAUTHPUBKEYEX
 */
LOCAL VOS_INT8 AT_CheckIsFacAuthPubKeyExCmd(VOS_UINT8 *dataPara, VOS_UINT16 cmdlen, VOS_UINT8 *data)
{
    errno_t memResult;
    VOS_INT8 ret;

    (VOS_VOID)memset_s(dataPara, cmdlen, 0x00, cmdlen);

    /* �������������������Ƚ�ʹ�� */
    memResult = memcpy_s(dataPara, cmdlen, data, cmdlen);
    TAF_MEM_CHK_RTN_VAL(memResult, cmdlen, cmdlen);

    /* AT����ͷ�ַ�ת��д */
    At_UpString(dataPara, cmdlen);

    /* ��������ַ���ͷ������"AT^FACAUTHPUBKEYEX="ֱ�ӷ���AT_FAILURE */
    ret = VOS_StrNiCmp((VOS_CHAR *)dataPara, "AT^FACAUTHPUBKEYEX=", cmdlen);

    return ret;
}

/*
 * ��������: ��ȡ����(����������ǰ׺AT)���Ƽ�����
 */
LOCAL VOS_UINT16 AT_GetFacAuthPubKeyCmdParametersLen(VOS_UINT8 indexNum, VOS_UINT8 *data)
{
    AT_ParseCmdNameType atCmdName;
    VOS_UINT16 pos;
    errno_t memResult;

    (VOS_VOID)memset_s(&atCmdName, sizeof(atCmdName), 0x00, sizeof(atCmdName));

    AT_SaveCmdElementInfo(indexNum, (VOS_UINT8 *)"^FACAUTHPUBKEYEX", AT_EXTEND_CMD_TYPE);

    /* ��ȡ����(����������ǰ׺AT)���Ƽ����� */
    pos = (VOS_UINT16)VOS_StrLen("AT");

    atCmdName.cmdNameLen = (VOS_UINT16)VOS_StrLen("^FACAUTHPUBKEYEX");
    memResult            = memcpy_s(atCmdName.cmdName, sizeof(atCmdName.cmdName), (data + pos), atCmdName.cmdNameLen);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(atCmdName.cmdName), atCmdName.cmdNameLen);
    atCmdName.cmdName[atCmdName.cmdNameLen] = '\0';
    pos += atCmdName.cmdNameLen;

    pos += (VOS_UINT16)VOS_StrLen("=");

    return pos;
}

/*
 * ��������: FacAuthPubKey��������ȷ�Ĵ���
 */
LOCAL VOS_VOID AT_ProcFacAuthPubKeyIncorrectPara(VOS_UINT8 indexNum, VOS_UINT32 timerName)
{
    AT_AUTH_PUBKEYEX_CMD_PROC_CTX *authPubKeyCtx = VOS_NULL_PTR;

    authPubKeyCtx = AT_GetAuthPubkeyExCmdCtxAddr();

    At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);
    AT_ClearAuthPubkeyCtx();
    (VOS_VOID)AT_StopRelTimer(timerName, &(authPubKeyCtx->hAuthPubkeyProtectTimer));
}

/*
 * ��������: ��ȡ�����еĶ���λ�ú͸���
 */
LOCAL VOS_UINT16 AT_GetFacAuthPubKeyCmdCommaNumAndPosition(VOS_UINT16 pos, VOS_UINT16 len, VOS_UINT8 *data,
    VOS_UINT16 *firstCommaPos, VOS_UINT16 *secCommaPos)
{
    VOS_UINT16 loop;
    VOS_UINT16 commaCnt = 0;

    /* ��ȡ�����еĶ���λ�ú͸��� */
    for (loop = pos; loop < len; loop++) {
        if (*(data + loop) != ',') {
            continue;
        }

        commaCnt++;
        /* ��¼�µ�һ�����ŵ�λ�� */
        if ((*firstCommaPos) == 0) {
            (*firstCommaPos) = loop + 1;
        } else {
            if ((*secCommaPos) == 0) {
                (*secCommaPos) = loop + 1;
            }
        }
    }

    return commaCnt;
}

VOS_UINT32 AT_HandleFacAuthPubKeyExCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    AT_ParseCmdNameType            atCmdName;
    VOS_UINT8                     *dataPara = VOS_NULL_PTR;
    VOS_UINT32                     result;
    VOS_UINT32                     firstParaVal = 0;
    VOS_UINT32                     secParaVal = 0;
    VOS_UINT32                     timerName;
    VOS_UINT16                     cmdlen;
    VOS_UINT16                     pos = 0;
    VOS_UINT16                     commaCnt = 0;
    VOS_UINT16                     firstCommaPos = 0;
    VOS_UINT16                     secCommaPos = 0;
    VOS_UINT16                     firstParaLen = 0;
    VOS_UINT16                     secondParaLen = 0;
    VOS_UINT16                     thirdParaLen;

    timerName = AT_HandleFacAuthPubKeyExCmdInit(indexNum, &cmdlen, &atCmdName);

    if (len < cmdlen) {
        return AT_FAILURE;
    }

    /*lint -save -e516 */
    dataPara = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, cmdlen);
    /*lint -restore */
    if (dataPara == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HandleFacAuthPubKeyExCmd: pucDataPara Memory malloc failed!");
        return AT_FAILURE;
    }

    if (AT_CheckIsFacAuthPubKeyExCmd(dataPara, cmdlen, data) != 0) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        return AT_FAILURE;
    }

    /* ��ȡ�����в�����ʼ��λ�� */
    pos = AT_GetFacAuthPubKeyCmdParametersLen(indexNum, data);

    /* ��ȡ�����еĶ���λ�ú͸��� */
    commaCnt = AT_GetFacAuthPubKeyCmdCommaNumAndPosition(pos, len, data, &firstCommaPos, &secCommaPos);

    /* �����Ÿ�����Ϊ2����AT����������ʧ�� */
    if (commaCnt != AT_VALID_COMMA_NUM) {
        AT_WARN_LOG("AT_HandleFacAuthPubKeyExCmd: usCommaCnt != 2!");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        AT_ProcFacAuthPubKeyIncorrectPara(indexNum, timerName);
        return AT_SUCCESS;
    }

    /* ��������ĳ��� */
    firstParaLen  = (firstCommaPos - pos) - (VOS_UINT16)VOS_StrLen(",");
    secondParaLen = secCommaPos - firstCommaPos - (VOS_UINT16)VOS_StrLen(",");
    thirdParaLen  = len - secCommaPos;

    /* ��ȡ��һ������ֵ */
    if (atAuc2ul(data + pos, firstParaLen, &firstParaVal) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleFacAuthPubKeyExCmd: ulFirstParaVal value invalid");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        AT_ProcFacAuthPubKeyIncorrectPara(indexNum, timerName);
        return AT_SUCCESS;
    }

    /* ��ȡ�ڶ�������ֵ */
    if (atAuc2ul(data + firstCommaPos, secondParaLen, &secParaVal) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleFacAuthPubKeyExCmd: ulSecParaVal value invalid");
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        AT_ProcFacAuthPubKeyIncorrectPara(indexNum, timerName);
        return AT_SUCCESS;
    }

    result = AT_SetFacAuthPubkeyExPara(indexNum, firstParaVal, secParaVal, thirdParaLen, (data + secCommaPos));
    if (result != AT_WAIT_ASYNC_RETURN) {
        At_FormatResultData(indexNum, result);
    }

    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, dataPara);
    /*lint -restore */
    return AT_SUCCESS;
}

/*
 * ��������: ��ȡ����(����������ǰ׺AT)���Ƽ�����
 */
LOCAL VOS_UINT16 AT_GetAtCmdParametersLen(const VOS_UINT8 *data, const VOS_CHAR *atCmdStr,
    AT_ParseCmdNameType *cmdName)
{
    errno_t memResult;
    VOS_UINT16 pos;

    pos = (VOS_UINT16)VOS_StrLen("AT");

    cmdName->cmdNameLen = (VOS_UINT16)VOS_StrLen(atCmdStr);

    memResult = memcpy_s(cmdName->cmdName, AT_CMD_NAME_LEN + 1, (data + pos), cmdName->cmdNameLen);
    TAF_MEM_CHK_RTN_VAL(memResult, AT_CMD_NAME_LEN + 1, cmdName->cmdNameLen);

    cmdName->cmdName[cmdName->cmdNameLen] = '\0';

    pos += cmdName->cmdNameLen;

    pos += (VOS_UINT16)VOS_StrLen("=");

    return pos;
}

VOS_UINT32 AT_HandleSimLockDataWriteCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    errno_t                     memResult;
    VOS_INT8                    ret;
    VOS_UINT16                  cmdlen;
    VOS_UINT16                  pos;
    VOS_UINT8                  *dataPara         = VOS_NULL_PTR;
    AT_SIMLOCKDATAWRITE_SetReq *simlockDataWrite = VOS_NULL_PTR;
    VOS_UINT32                  result;
    AT_ParseCmdNameType         atCmdName;
    VOS_CHAR                   *atCmdStr = "^SIMLOCKDATAWRITE";

    (VOS_VOID)memset_s(&atCmdName, sizeof(atCmdName), 0x00, sizeof(atCmdName));

    /* Ϊ���AT�������ܣ�����ڴ��ж�������Ƿ�ΪAT^SIMLOCKDATAWRITE��������ĳ��ȣ���������ֱ���˳� */
    if ((VOS_StrLen("AT^SIMLOCKDATAWRITE=") + AT_SIMLOCKDATAWRITE_PARA_LEN) != len) {
        return AT_FAILURE;
    }

    cmdlen = (VOS_UINT16)VOS_StrLen("AT^SIMLOCKDATAWRITE=");
    /*lint -save -e516 */
    dataPara = (VOS_UINT8 *)PS_MEM_ALLOC(WUEPS_PID_AT, cmdlen);
    /*lint -restore */
    if (dataPara == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HandleSimLockDataWriteCmd: pucDataPara Memory malloc failed!");
        return AT_FAILURE;
    }
    (VOS_VOID)memset_s(dataPara, cmdlen, 0x00, cmdlen);

    /* �������������������Ƚ�ʹ�� */
    memResult = memcpy_s(dataPara, cmdlen, data, cmdlen);
    TAF_MEM_CHK_RTN_VAL(memResult, cmdlen, cmdlen);

    /* AT����ͷ�ַ�ת��д */
    At_UpString(dataPara, cmdlen);

    /* ��������ַ���ͷ������"AT^SIMLOCKDATAWRITE="ֱ�ӷ���AT_FAILURE */
    ret = VOS_StrNiCmp((VOS_CHAR *)dataPara, "AT^SIMLOCKDATAWRITE=", cmdlen);
    if (ret != 0) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        return AT_FAILURE;
    }

    AT_SaveCmdElementInfo(indexNum, (VOS_UINT8 *)"^SIMLOCKDATAWRITE", AT_EXTEND_CMD_TYPE);

    /* ��ȡ����(����������ǰ׺AT)���Ƽ����� */
    pos = AT_GetAtCmdParametersLen(data, atCmdStr, &atCmdName);

    /* ���������������ṹ */
    simlockDataWrite = (AT_SIMLOCKDATAWRITE_SetReq *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(AT_SIMLOCKDATAWRITE_SetReq));
    if (simlockDataWrite == VOS_NULL_PTR) {
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, dataPara);
        /*lint -restore */
        At_FormatResultData(indexNum, AT_ERROR);
        return AT_SUCCESS;
    }
    (VOS_VOID)memset_s(simlockDataWrite, sizeof(AT_SIMLOCKDATAWRITE_SetReq), 0x00, sizeof(AT_SIMLOCKDATAWRITE_SetReq));

    /* ������� */
    memResult = memcpy_s(simlockDataWrite->categoryData, AT_SIMLOCKDATAWRITE_PARA_LEN, (data + pos),
                         (VOS_SIZE_T)(len - pos));
    TAF_MEM_CHK_RTN_VAL(memResult, AT_SIMLOCKDATAWRITE_PARA_LEN, (VOS_SIZE_T)(len - pos));

    /* �����������ͣ��������ͺͲ������� */
    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD;
    g_atCmdFmtType          = AT_EXTEND_CMD_TYPE;

    AT_PR_LOGI("enter");

    result = AT_SetSimlockDataWritePara(indexNum, simlockDataWrite);
    if (result != AT_WAIT_ASYNC_RETURN) {
        AT_PR_LOGI("return OK");

        At_FormatResultData(indexNum, result);
    }
    /*lint -save -e516 */
    PS_MEM_FREE(WUEPS_PID_AT, dataPara);
    PS_MEM_FREE(WUEPS_PID_AT, simlockDataWrite);
    /*lint -restore */
    return AT_SUCCESS;
}

/*
 * ��������: ��ȡ����������еĶ���λ�ú͸���
 */
LOCAL VOS_UINT32 AT_GetAtCmdCommaNumAndPosition(VOS_UINT8 indexNum, VOS_UINT16 pos, VOS_UINT16 len,
    const VOS_UINT8 *data, VOS_UINT16 *firstCommaPos)
{
    VOS_UINT16 loop;
    VOS_UINT16 commaCnt = 0;

    for (loop = pos; loop < len; loop++) {
        if (*(data + loop) == ',') {
            commaCnt++;

            /* ��¼�µ�һ�����ŵ�λ�� */
            if ((*firstCommaPos) == 0) {
                (*firstCommaPos) = loop + 1;
            }
        }
    }

    /* �����Ÿ�����Ϊ1����AT����������ʧ�� */
    if (commaCnt != 1) {
        AT_WARN_LOG("AT_GetAtCmdCommaNumAndPosition: usCommaCnt != 1!");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    return AT_FAILURE;
}

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))

VOS_UINT32 AT_HandleApSndApduCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_CHAR *atCmdStr = "^CISA";
    VOS_UINT16 cmdlen;
    VOS_UINT16 pos;
    VOS_UINT16 firstCommaPos = 0;
    VOS_UINT16 firstParaLen;
    VOS_UINT16 secondParaLen;
    VOS_UINT32 lengthValue;
    VOS_UINT32 result;
    VOS_UINT8  headChar;
    VOS_UINT8  tailChar;

    /* CISA����ֻ�ܴ�AP����մ��������˿ڲ����� */
    if (AT_IsApPort(indexNum) != VOS_TRUE) {
        return AT_FAILURE;
    }

    cmdlen = (VOS_UINT16)VOS_StrLen("AT^CISA=");

    /* ��������ַ���ͷ������"AT^CISA="ֱ�ӷ���AT_FAILURE */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CISA=", cmdlen) != 0) {
        return AT_FAILURE;
    }

    /* ��֧��AT^CISA=?���� */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CISA=?", VOS_StrLen("AT^CISA=?")) == 0) {
        At_FormatResultData(indexNum, AT_ERROR);

        return AT_SUCCESS;
    }

    /* ��ȡ����(����������ǰ׺AT)���Ƽ����� */
    pos = AT_GetAtCmdParametersLen(data, atCmdStr, &(g_atParseCmd.cmdName));

    /* ��ȡ����������еĶ���λ�ú͸��� */
    if (AT_GetAtCmdCommaNumAndPosition(indexNum, pos, len, data, &firstCommaPos) == AT_SUCCESS) {
        AT_WARN_LOG("AT_HandleApSndApduCmd: usCommaCnt != 1!");
        return AT_SUCCESS;
    }

    /* �ڶ��������������Ű��������� */
    headChar = *(data + firstCommaPos);
    tailChar = *(data + len - VOS_StrLen("\""));
    if ((headChar != '"') || (tailChar != '"')) {
        AT_WARN_LOG("AT_HandleApSndApduCmd: <command> not a string");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    /* �������������ĳ��� */
    firstParaLen  = (firstCommaPos - pos) - (VOS_UINT16)VOS_StrLen(",");
    secondParaLen = len - firstCommaPos - (VOS_UINT16)(AT_DOUBLE_LENGTH * VOS_StrLen("\""));

    if (atAuc2ul(data + cmdlen, firstParaLen, &lengthValue) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleApSndApduCmd: <length> value invalid");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }
    /* �����������ͣ��������ͺͲ������� */
    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD;
    g_atCmdFmtType          = AT_EXTEND_CMD_TYPE;

    /* ����չ�������ƥ������� */
    result = atMatchCmdName(indexNum, g_atParseCmd.cmdFmtType);

    if (result != ERR_MSP_SUCCESS) {
        At_FormatResultData(indexNum, result);

        return AT_SUCCESS;
    }

    result = AT_SetCISAPara(indexNum, lengthValue, (data + firstCommaPos + VOS_StrLen("\"")), secondParaLen);

    if (result != AT_WAIT_ASYNC_RETURN) {
        At_FormatResultData(indexNum, result);
    }

    return AT_SUCCESS;
}
#endif

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)

VOS_UINT32 AT_HandleApXsmsSndCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT16 cmdlen;
    VOS_UINT16 pos;
    VOS_UINT16 firstCommaPos = 0;
    VOS_UINT16 firstParaLen;
    VOS_UINT16 secondParaLen;
    VOS_UINT32 lengthValue;
    VOS_UINT32 result;
    VOS_UINT8  headChar;
    VOS_UINT8  tailChar;
    VOS_CHAR *atCmdStr = "^CCMGS";

    cmdlen = (VOS_UINT16)VOS_StrLen("AT^CCMGS=");

    /* ��������ַ���ͷ������"AT^CCMGS="ֱ�ӷ���AT_FAILURE */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CCMGS=", cmdlen) != 0) {
        return AT_FAILURE;
    }

    /* ��֧��AT^CCMGS=?���� */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CCMGS=?", VOS_StrLen("AT^CCMGS=?")) == 0) {
        At_FormatResultData(indexNum, AT_OK);

        return AT_SUCCESS;
    }

    /* ��ȡ����(����������ǰ׺AT)���Ƽ����� */
    pos = AT_GetAtCmdParametersLen(data, atCmdStr, &(g_atParseCmd.cmdName));

    /* ��ȡ����������еĶ���λ�ú͸��� */
    if (AT_GetAtCmdCommaNumAndPosition(indexNum, pos, len, data, &firstCommaPos) == AT_SUCCESS) {
        AT_WARN_LOG("AT_HandleApXsmsSndCmd: usCommaCnt != 1!");
        return AT_SUCCESS;
    }

    /* �ڶ��������������Ű��������� */
    headChar = *(data + firstCommaPos);
    tailChar = *(data + len - VOS_StrLen("\""));
    if ((headChar != '"') || (tailChar != '"')) {
        AT_WARN_LOG("AT_HandleApXsmsSndCmd: <PDU> not a string");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    /* �������������ĳ��� */
    firstParaLen  = (firstCommaPos - pos) - (VOS_UINT16)VOS_StrLen(",");
    secondParaLen = len - firstCommaPos - (VOS_UINT16)(AT_DOUBLE_LENGTH * VOS_StrLen("\""));

    if (atAuc2ul(data + cmdlen, firstParaLen, &lengthValue) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleApXsmsSndCmd: <length> value invalid");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }
    /* �����������ͣ��������ͺͲ������� */
    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD;
    g_atCmdFmtType          = AT_EXTEND_CMD_TYPE;

    /* ����չ�������ƥ������� */
    result = atMatchCmdName(indexNum, g_atParseCmd.cmdFmtType);

    if (result != ERR_MSP_SUCCESS) {
        At_FormatResultData(indexNum, result);

        return AT_SUCCESS;
    }

    result = AT_SetCcmgsPara(indexNum, lengthValue, (data + firstCommaPos + VOS_StrLen("\"")), secondParaLen);

    if (result != AT_WAIT_ASYNC_RETURN) {
        At_FormatResultData(indexNum, result);
    }

    return AT_SUCCESS;
}

/*
 * ��������: ��ȡ���Ÿ�����λ��
 */
LOCAL VOS_UINT16 AT_GetApXsmsWriteCmdCommaNumAndPosition(VOS_UINT16 pos, VOS_UINT16 len, VOS_UINT8 *data, VOS_UINT16 *firstCommaPos, VOS_UINT16 *secondCommaPos)
{
    VOS_UINT16 commaCnt = 0;
    VOS_UINT16 loop;

    for (loop = pos; loop < len; loop++) {
        if (*(data + loop) == ',') {
            commaCnt++;

            /* ��¼�µ�һ�����ŵ�λ�� */
            if ((*firstCommaPos) == 0) {
                (*firstCommaPos) = loop + 1;
            }

            /* ��¼�µڶ������ŵ�λ�� */
            if (((*secondCommaPos) == 0) && (commaCnt == AT_VALID_COMMA_NUM)) {
                (*secondCommaPos) = loop + 1;
            }
        }
    }

    return commaCnt;
}

/*
 * ��������: ���ڶ����͵���������
 */
LOCAL VOS_UINT32 AT_CheckApXsmsWriteCmdPara(VOS_UINT8 indexNum, VOS_UINT16 commaCnt, VOS_UINT16 len, VOS_UINT16 secondCommaPos, VOS_UINT8 *data)
{
    VOS_UINT8  headChar;
    VOS_UINT8  tailChar;

    /* �����Ÿ�����Ϊ2����AT����������ʧ�� */
    if (commaCnt != AT_VALID_COMMA_NUM) {
        AT_WARN_LOG("AT_CheckApXsmsWriteCmdPara: usCommaCnt != 1!");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    /* �����������������Ű��������� */
    headChar = *(data + secondCommaPos);
    tailChar = *(data + len - VOS_StrLen("\""));
    if ((headChar != '"') || (tailChar != '"')) {
        AT_WARN_LOG("AT_CheckApXsmsWriteCmdPara: <PDU> not a string");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    return AT_FAILURE;
}

VOS_UINT32 AT_HandleApXsmsWriteCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT16 cmdlen;
    VOS_UINT16 pos;
    VOS_UINT16 commaCnt = 0;
    VOS_UINT16 firstCommaPos = 0;
    VOS_UINT16 secondCommaPos = 0;
    VOS_UINT16 firstParaLen;
    VOS_UINT16 secondParaLen;
    VOS_UINT16 thirdParaLen;
    VOS_UINT32 lengthValue;
    VOS_UINT32 statValue;
    VOS_UINT32 result;
    VOS_CHAR *atCmdStr = "^CCMGW";

    cmdlen = (VOS_UINT16)VOS_StrLen("AT^CCMGW=");

    /* ��������ַ���ͷ������"AT^CCMGW="ֱ�ӷ���AT_FAILURE */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CCMGW=", cmdlen) != 0) {
        return AT_FAILURE;
    }

    /* ��֧��AT^CCMGS=?���� */
    if (VOS_StrNiCmp((VOS_CHAR *)data, "AT^CCMGW=?", VOS_StrLen("AT^CCMGW=?")) == 0) {
        At_FormatResultData(indexNum, AT_OK);

        return AT_SUCCESS;
    }

    /* ��ȡ����(����������ǰ׺AT)���Ƽ����� */
    pos = AT_GetAtCmdParametersLen(data, atCmdStr, &(g_atParseCmd.cmdName));

    /* ��ȡ�����еĶ���λ�ú͸��� */
    commaCnt = AT_GetApXsmsWriteCmdCommaNumAndPosition(pos, len, data, &firstCommaPos, &secondCommaPos);

    if (AT_CheckApXsmsWriteCmdPara(indexNum, commaCnt, len, secondCommaPos, data) == AT_SUCCESS) {
        return AT_SUCCESS;
    }
    /* �������������ĳ��� */
    firstParaLen  = (firstCommaPos - pos) - (VOS_UINT16)VOS_StrLen(",");
    secondParaLen = secondCommaPos - firstCommaPos - (VOS_UINT16)VOS_StrLen(",");
    thirdParaLen  = len - secondCommaPos - (VOS_UINT16)(AT_DOUBLE_LENGTH * VOS_StrLen("\""));

    /* ��������1<len>��ֵ */
    if (atAuc2ul(data + cmdlen, firstParaLen, &lengthValue) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleApXsmsSndCmd: <length> value invalid");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    /* ��������2<stat>��ֵ */
    if (atAuc2ul(data + firstCommaPos, secondParaLen, &statValue) == AT_FAILURE) {
        AT_WARN_LOG("AT_HandleApXsmsSndCmd: <length> value invalid");

        At_FormatResultData(indexNum, AT_CME_INCORRECT_PARAMETERS);

        return AT_SUCCESS;
    }

    /* �����������ͣ��������ͺͲ������� */
    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD;
    g_atCmdFmtType          = AT_EXTEND_CMD_TYPE;

    /* ����չ�������ƥ������� */
    result = atMatchCmdName(indexNum, g_atParseCmd.cmdFmtType);

    if (result != ERR_MSP_SUCCESS) {
        At_FormatResultData(indexNum, result);

        return AT_SUCCESS;
    }

    result = AT_SetCcmgwPara(indexNum, lengthValue, statValue, (data + secondCommaPos + VOS_StrLen("\"")),
                             thirdParaLen);

    if (result != AT_WAIT_ASYNC_RETURN) {
        At_FormatResultData(indexNum, result);
    }

    return AT_SUCCESS;
}
#endif

VOS_UINT32 At_HandleApModemSpecialCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    /* ��ȡ��ǰ��Ʒ��̬ */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    if ((*systemAppConfig != SYSTEM_APP_ANDROID)
#if (FEATURE_PHONE_SC == FEATURE_ON)
        && (*systemAppConfig != SYSTEM_APP_MP)
#endif
        ) {
        return AT_FAILURE;
    }

    /* ����AT^FACAUTHPUBKEYEX=<index>,<total>,<pubkey>��������(����<pubkey>����) */
    if (AT_HandleFacAuthPubKeyExCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    /* ����AT^IDENTIFYEND=<Rsa>��������(����<Rsa>����) */
    if (AT_HandleApModemSpecialCmdProc(indexNum, data, len, "^IDENTIFYEND") == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    /* ����AT^IDENTIFYOTAEND=<OtaRsa>��������(����<OtaRsa>����) */
    if (AT_HandleApModemSpecialCmdProc(indexNum, data, len, "^IDENTIFYOTAEND") == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    /* ����AT^PHONEPHYNUM=<type>,<PhynumRsa>,<Hmac>��������(����<PhynumRsa>����) */
    if (AT_HandleApModemSpecialCmdProc(indexNum, data, len, "^PHONEPHYNUM") == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    /* ����AT^SIMLOCKDATAWRITEEX=<layer>,<total>,<index>,<simlock_data>,[hmac]��������(����<simlock_data>����) */
    if (AT_HandleSimLockDataWriteExCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    /* ����AT^SIMLOCKOTADATAWRITE=<layer>,<total>,<index>,<simlock_data>,[hmac]��������(����<simlock_data>����) */
    if (AT_HandleSimLockOtaDataWriteCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }

#if (FEATURE_SC_NETWORK_UPDATE == FEATURE_ON)
    /* ����AT^SIMLOCKNWDATAWRITE=<layer>,<total>,<index>,<simlock_data>,[hmac]��������(����<simlock_data>����) */
    if (AT_HandleSimLockNWDataWriteCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }
#endif

    /* ����AT^SIMLOCKDATAWRITE=<simlock_data_write>��������(����<simlock_data_write>����) */
    if (AT_HandleSimLockDataWriteCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }

#if (defined(FEATURE_PHONE_ENG_AT_CMD) && (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON))
    /* ����AT^CISA=<length>,<command> */
    if (AT_HandleApSndApduCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }
#endif

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (AT_HandleApXsmsSndCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }

    if (AT_HandleApXsmsWriteCmd(indexNum, data, len) == AT_SUCCESS) {
        return AT_SUCCESS;
    }
#endif

    /* ɾ����^EOPLMN���������������� */

    return AT_FAILURE;
}

TAF_UINT32 At_CheckUsimStatus(TAF_UINT8 *cmdName, VOS_UINT8 indexNum)
{
    TAF_UINT32       rst;
    ModemIdUint16    modemId;
    VOS_UINT32       getModemIdRslt;
    AT_USIM_InfoCtx *usimInfoCtx = VOS_NULL_PTR;
    AT_ModemSmsCtx  *smsCtx      = VOS_NULL_PTR;

    NAS_NVIM_FollowonOpenspeedFlag quickStartFlg;

    quickStartFlg.quickStartSta = AT_QUICK_START_DISABLE;
    modemId                     = MODEM_ID_0;

    getModemIdRslt = AT_GetModemIdFromClient(indexNum, &modemId);

    if (getModemIdRslt != VOS_OK) {
        AT_ERR_LOG("At_CheckUsimStatus:Get Modem Id fail!");
        return AT_ERROR;
    }

    smsCtx      = AT_GetModemSmsCtxAddrFromModemId(modemId);
    usimInfoCtx = AT_GetUsimInfoCtxFromModemId(modemId);


    /* ���ٿ���ģʽ������� PIN ״̬ */
    if (TAF_ACORE_NV_READ(modemId, NV_ITEM_FOLLOW_ON_OPEN_SPEED_FLAG, (VOS_UINT8 *)(&quickStartFlg),
                          sizeof(NAS_NVIM_FollowonOpenspeedFlag)) != NV_OK) {
        quickStartFlg.quickStartSta = AT_QUICK_START_DISABLE;
    }

    if (quickStartFlg.quickStartSta == AT_QUICK_START_ENABLE) {
        return AT_SUCCESS;
    }

    /* SMS����������������Ҫ��ҵ��ģ���ڲ������˴�������CMGL����������Ĵ����ȷ�� */
    if (VOS_StrCmp((TAF_CHAR *)cmdName, "+CMGL") == 0) {
        if (smsCtx->cpmsInfo.usimStorage.totalRec != 0) {
            return AT_SUCCESS;
        }
    }

    switch (usimInfoCtx->cardStatus) {
        case USIMM_CARDAPP_SERVIC_AVAILABLE:
            rst = AT_SUCCESS;
            break;
        case USIMM_CARDAPP_SERVIC_SIM_PIN:
            rst = AT_CME_SIM_PIN_REQUIRED;
            break;
        case USIMM_CARDAPP_SERVIC_SIM_PUK:
            rst = AT_CME_SIM_PUK_REQUIRED;
            break;
        case USIMM_CARDAPP_SERVIC_UNAVAILABLE:
        case USIMM_CARDAPP_SERVIC_NET_LCOK:
        case USIMM_CARDAPP_SERVICE_IMSI_LOCK:
            rst = AT_CME_SIM_FAILURE;
            break;
        case USIMM_CARDAPP_SERVIC_ABSENT:
            rst = AT_CME_SIM_NOT_INSERTED;
            break;

        case USIMM_CARDAPP_SERVIC_BUTT:

            rst = AT_CME_SIM_BUSY;
            break;

        default:
            rst = AT_ERROR;
            break;
    }

    return rst;
}

VOS_VOID At_SetCmdSubMode(AT_ClientIdUint16 clientId, AT_CmdSubModeUint8 subMode)
{
    DMS_PortIdUint16        portId;
    DMS_PortModeUint8       mode;

    portId  = g_atClientTab[clientId].portNo;
    mode    = DMS_PORT_GetMode(portId);

    if  (mode != DMS_PORT_MODE_CMD &&
         mode != DMS_PORT_MODE_ONLINE_CMD) {
        AT_ERR_LOG1("At_SetCmdSubMode mode abnormal", mode);
        return;
    }

    g_parseContext[clientId].mode = subMode;
}

AT_CmdSubModeUint8 At_GetCmdSubMode(AT_ClientIdUint16 clientId)
{
    return g_parseContext[clientId].mode;
}

TAF_UINT32 AT_CountDigit(TAF_UINT8 *data, TAF_UINT32 len, TAF_UINT8 Char, TAF_UINT32 indexNum)
{
    TAF_UINT8 *tmp    = TAF_NULL_PTR;
    TAF_UINT8 *check  = TAF_NULL_PTR;
    TAF_UINT32 count  = 0;
    TAF_UINT32 chkLen = 0;
    TAF_UINT32 tmpLen = 0;

    tmp = data;

    /* ���������� */
    if ((*tmp == Char) || (*((tmp + len) - 1) == Char)) {
        return 0;
    }

    check = data;
    while (tmpLen++ < len) {
        /* �ж��Ƿ������ֻ������ض��ַ� */
        if (((*check >= '0') && (*check <= '9')) || ((*check == Char) && (*(check + 1) != Char))) {
            check++;
        } else {
            return 0;
        }
    }

    while (chkLen++ < len) {
        if (*tmp == Char) {
            count++;
            if (count == indexNum) {
                return chkLen;
            }
        }
        tmp++;
    }

    return 0;
}

/*
 * ��������: COPS�������ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcNetWorkTimerOut(VOS_UINT8 indexNum)
{
    return AT_CME_NETWORK_TIMEOUT;
}

/*
 * ��������: PC����PPP�������ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcPppOrgTimerOut(VOS_UINT8 indexNum)
{
    AT_SendRelPppReq(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_REQ);
    /* ��������ģʽ */
    DMS_PORT_ResumeCmdMode(g_atClientTab[indexNum].portNo);

    /* ��NO CARRIER��Ϣ */
    return AT_NO_CARRIER;
}

/*
 * ��������: ATD����PPP�������ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcAtdIpCallTimerOut(VOS_UINT8 indexNum)
{
    AT_SendRelPppReq(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);
    return AT_ERROR;
}

/*
 * ��������: ����ҵ�񲦺ŶϿ����ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcPsCallEndTimerOut(VOS_UINT8 indexNum)
{
    /* ��������ģʽ */
    DMS_PORT_ResumeCmdMode(g_atClientTab[indexNum].portNo);
    return AT_ERROR;
}

/*
 * ��������: ����TMODE���ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcTmodeTimerOut(VOS_UINT8 indexNum)
{
    return AT_ERROR;
}

/*
 * ��������: ABORT���ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcAbortCmdTimerOut(VOS_UINT8 indexNum)
{
    return AT_ABORT;
}

/*
 * ��������: δ֪����ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcUnkonwnTimerOut(VOS_UINT8 indexNum)
{
    return AT_CMS_UNKNOWN_ERROR;
}

/*
 * ��������: ���绰����ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcCpbrTimerOut(VOS_UINT8 indexNum)
{
    AT_UartCtx          *uartCtx = VOS_NULL_PTR;

    uartCtx = AT_GetUartCtxAddr();

    if (AT_IsHsuartBaseUser(indexNum) == VOS_TRUE) {
        uartCtx->wmLowFunc = VOS_NULL_PTR;
    }

    return AT_ERROR;
}

/*
 * ��������: ����ģʽ���ó�ʱ����
 */
LOCAL AT_RreturnCodeUint32 AT_ProcVmsetTimerOut(VOS_UINT8 indexNum)
{
    AT_InitVmSetCtx();
    return AT_ERROR;
}

/*
 * ��������: ��������صļ�ʱ������ʱ��������
 */
LOCAL AT_PS_CMD_TIMEOUT_FUNC AT_GetCmdTimerOutResultFunc(AT_CmdCurOptUint32 cmdCurrentOpt)
{
    const AT_PS_CmdTimeoutResult  *cmdTimeoutFuncTblPtr = VOS_NULL_PTR;
    AT_PS_CMD_TIMEOUT_FUNC         cmdTimeoutFunc       = VOS_NULL_PTR;
    VOS_UINT32                          cnt;

    cmdTimeoutFuncTblPtr = AT_GET_CMD_TIMEOUT_RSLT_FUNC_TBL_PTR();

    for (cnt = 0; cnt < AT_GET_CMD_TIMEOUT_RSLT_FUNC_TBL_SIZE(); cnt++) {
        if (cmdCurrentOpt == cmdTimeoutFuncTblPtr[cnt].cmdCurrentOpt) {
            cmdTimeoutFunc = cmdTimeoutFuncTblPtr[cnt].cmdTimeoutFunc;
            break;
        }
    }

    return cmdTimeoutFunc;
}

VOS_VOID At_InterTimerOutProc(VOS_UINT8 indexNum)
{
    AT_PS_CMD_TIMEOUT_FUNC cmdTimeoutFunc = VOS_NULL_PTR;
    AT_RreturnCodeUint32 result;

    result  = AT_FAILURE;

    cmdTimeoutFunc = AT_GetCmdTimerOutResultFunc(g_atClientTab[indexNum].cmdCurrentOpt);

    /* ������������������ */
    if (cmdTimeoutFunc != VOS_NULL_PTR) {
        result = cmdTimeoutFunc(indexNum);
    } else {
        AT_ERR_LOG1("At_InterTimerOutProc: not find func, cmdCurrentOpt is ", g_atClientTab[indexNum].cmdCurrentOpt);
        result = AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;
#if (FEATURE_IOT_ATNLPROXY == FEATURE_ON)
    /* usMagic С�� 1024���� */
    g_atClientTab[indexNum].magic = 0;
    g_atClientTab[indexNum].canAbort = 0;
    if (indexNum == g_current_user_at) {
        g_atClientTab[AT_CLIENT_ID_APP].isWaitAts = 0;
        g_current_user_at = VOS_NULL_PARA;
    }
#endif
    At_FormatResultData(indexNum, result);
}

VOS_UINT32 AT_DockHandleCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    errno_t    memResult;
    VOS_UINT16 length = 0;
    VOS_UINT8  cmdTmp[AT_CMDTMP_MAX_LEN + 1] = {0};
    VOS_UINT8  i;

    if (len > AT_CMDTMP_MAX_LEN) {
        return AT_FAILURE;
    }

    /* ����AT�����Ӧ��ת����E5 */
    for (i = 0; i < AT_MAX_CLIENT_NUM; i++) {
        if (g_atClientTab[i].userType == AT_APP_USER) {
            (VOS_VOID)memset_s(cmdTmp, sizeof(cmdTmp), 0x00, sizeof(cmdTmp));
            if (len > 0) {
                memResult = memcpy_s(cmdTmp, sizeof(cmdTmp), data, len);
                TAF_MEM_CHK_RTN_VAL(memResult, sizeof(cmdTmp), len);
            }

            length += (VOS_UINT16)AT_FormatReportString(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                (VOS_CHAR *)g_atSndCodeAddress + length, "%s%s%s", g_atCrLf, cmdTmp, g_atCrLf);
            At_SendResultData(i, g_atSndCodeAddress, length);
            return AT_SUCCESS;
        }
    }

    return AT_FAILURE;
}

/*
 * Description: ������ƥ��
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_UINT32 At_MatchCmdName(TAF_UINT16 indexNum, TAF_UINT32 cmdType)
{
    /*   */
    return AT_FAILURE;
}

VOS_UINT32 AT_HandleDockSetCmd(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT32 result;

    result = AT_ParseSetDockCmd(data, len);
    if (result != AT_SUCCESS) {
        return AT_FAILURE;
    }

    if (At_MatchCmdName(indexNum, g_atCmdFmtType) == AT_FAILURE) {
        At_FormatResultData(indexNum, AT_CMD_NOT_SUPPORT);
        return AT_SUCCESS;
    }

    AT_SaveCmdElementInfo(indexNum, (VOS_UINT8 *)"^DOCK", AT_EXTEND_CMD_TYPE);

    result = At_SetParaCmd(indexNum);

    At_FormatResultData(indexNum, result);

    return AT_SUCCESS;
}

VOS_VOID AT_RcvTiS0Expired(REL_TimerMsgBlock *msg)
{
    AT_ModemCcCtx    *ccCtx = VOS_NULL_PTR;
    MN_CALL_SupsParam callMgmtParam;
    TAF_Ctrl          ctrl;
    VOS_UINT8         clientIndex;
    VOS_UINT16        reqClientId;
    ModemIdUint16     modemId;

    /* ����timer name��ȡ�����ĸ�index */
    clientIndex = (VOS_UINT8)(msg->name >> 12);

    ccCtx = AT_GetModemCcCtxAddrFromClientId(clientIndex);

    (VOS_VOID)memset_s(&callMgmtParam, sizeof(callMgmtParam), 0x00, sizeof(callMgmtParam));

    /* Ŀǰֻ��voice֧���Զ��������ܣ�calltype �̶���Ϊvoice */
    callMgmtParam.callType    = MN_CALL_TYPE_VOICE;
    callMgmtParam.callSupsCmd = MN_CALL_SUPS_CMD_HOLD_ACT_ACPT_OTH;

    callMgmtParam.callId = (MN_CALL_ID_T)msg->para;

    /*
     * ����ClientIdΪ�㲥ClientId 0x3fff����0x7fff��
     * AT�ڴ���incoming�¼�ʱ���ж��������Զ�������������ʱ��ʱulName�д�����Client Index��0x3f����0x7f
     * ���Դ˴�����MN_CALL_Supsʱ��Ҫ��ClientId
     */
    if (clientIndex == AT_BROADCAST_CLIENT_INDEX_MODEM_0) {
        reqClientId = AT_BROADCAST_CLIENT_ID_MODEM_0;
    } else if (clientIndex == AT_BROADCAST_CLIENT_INDEX_MODEM_1) {
        reqClientId = AT_BROADCAST_CLIENT_ID_MODEM_1;
    } else {
        reqClientId = AT_BROADCAST_CLIENT_ID_MODEM_2;
    }

    (VOS_VOID)memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

    ctrl.moduleId = WUEPS_PID_AT;
    ctrl.clientId = reqClientId;
    ctrl.opId     = 0;

    if (AT_GetModemIdFromClient(reqClientId, &modemId) != VOS_OK) {
        return;
    }

    /* AT��CCM���Ͳ���ҵ������ */
    if (TAF_CCM_CallCommonReq(&ctrl, &callMgmtParam, ID_TAF_CCM_CALL_SUPS_CMD_REQ, sizeof(callMgmtParam), modemId) !=
        VOS_OK) {
        AT_WARN_LOG("AT_ProcTimerS0: S0 Answer Fail");
    }
    AT_PR_LOGH("AT_RcvTiS0Expired: CallId = %d", callMgmtParam.callId);
    ccCtx->s0TimeInfo.timerStart = TAF_FALSE;
    ccCtx->s0TimeInfo.timerName  = 0;
}

VOS_VOID AT_RcvTiShutDownExpired(REL_TimerMsgBlock *tmrMsg)
{
#if (VOS_WIN32 == VOS_OS_VER || OSA_CPU_CCPU == VOS_OSA_CPU)
    AT_SysbootShutdown((sysboot_shutdown_reason_e)0);
#else
    AT_SysbootShutdown();
#endif
}

AT_PS_INTERNAL_TIMEOUT_FUNC AT_GetInternalTimeOutResultFunc(VOS_UINT32 interTimerName)
{
    const AT_PS_InternalTimeoutResult  *interTimeoutFuncTblPtr = VOS_NULL_PTR;
    AT_PS_INTERNAL_TIMEOUT_FUNC         interTimeoutFunc       = VOS_NULL_PTR;
    VOS_UINT32                          cnt;

    interTimeoutFuncTblPtr = AT_GET_INTERNAL_TIMEOUT_RSLT_FUNC_TBL_PTR();

    for (cnt = 0; cnt < AT_GET_INTERNAL_TIMEOUT_RSLT_FUNC_TBL_SIZE(); cnt++) {
        if (interTimerName == interTimeoutFuncTblPtr[cnt].interTimerName) {
            interTimeoutFunc = interTimeoutFuncTblPtr[cnt].internalTimeoutFunc;
            break;
        }
    }

    return interTimeoutFunc;
}

TAF_VOID At_TimeOutProc(REL_TimerMsgBlock *msg)
{
    /* �ֲ�������������ʼ�� */
    AT_PS_INTERNAL_TIMEOUT_FUNC interTimeoutFunc = VOS_NULL_PTR;
    VOS_UINT8                   indexNum;

    if (msg == VOS_NULL_PTR) {
        AT_WARN_LOG("At_TimeOutProc pMsg is NULL");
        return;
    }

    indexNum = (VOS_UINT8)msg->name;
    if ((msg->name & 0x00000f00) == AT_INTERNAL_PROCESS_TYPE) {
        interTimeoutFunc = AT_GetInternalTimeOutResultFunc(msg->name & 0x000000ff);

        /* ������������������ */
        if (interTimeoutFunc != VOS_NULL_PTR) {
            interTimeoutFunc(msg);
        } else {
            AT_ERR_LOG1("At_TimeOutProc: not find timeout func, Msg is ", msg->name);
        }
        return;
    } else {
        indexNum = (VOS_UINT8)msg->name;
        if (indexNum >= AT_MAX_CLIENT_NUM) {
            AT_WARN_LOG("At_TimeOutProc FAILURE");
            return;
        }

        if (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_CURRENT_OPT_BUTT) {
            AT_WARN_LOG("At_TimeOutProc AT_CMD_CURRENT_OPT_BUTT");
            return;
        }

        AT_LOG1("At_TimeOutProc ucIndex:", indexNum);
        AT_LOG1("At_TimeOutProc g_atClientTab[ucIndex].CmdCurrentOpt:", g_atClientTab[indexNum].cmdCurrentOpt);

        At_InterTimerOutProc(indexNum);
    }
}
/*
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_UINT8* At_GetStrContent(TAF_UINT32 type)
{
    if (type < AT_STRING_BUTT) {
        return g_atStringTab[type].text;
    } else {
        return g_atStringTab[AT_STRING_BUTT].text;
    }
}

/* PC������AT��A���Ƶ�C��, At_sprintf���ظ�����,���ڴ˴������������� */

/*
 * ��������: �����κϷ���
 */
LOCAL VOS_INT32 AT_CheckFormatReportString(VOS_CHAR *headAddr, VOS_CHAR *currAddr)
{
    if ((headAddr == VOS_NULL_PTR) || (currAddr == VOS_NULL_PTR)) {
        AT_ERR_LOG("AT_CheckFormatReportString: Parameter of the function is null.");
        return VOS_FALSE;
    }

    if (currAddr < headAddr) {
        AT_ERR_LOG("AT_CheckFormatReportString: addr error.");
        return VOS_FALSE;
    }
    return VOS_TRUE;
}

/*
 * ��������: ����ʽ���ַ���������
 */
LOCAL VOS_VOID AT_ProcFormatReportType(VOS_CHAR **data, VOS_UINT32 *signFlag, VOS_UINT32 *len1, VOS_UINT32 *len2)
{
    (*len1) = 0;
    (*len2) = 0;
    (*data)++;

    if ((**data) == '+' && *(*data + 1) == 'd') {
        (*data)++;
        (*signFlag) = VOS_TRUE;
        (*len1)     = 0; /* �з������ʹ�ӡ���� */
    }

    if ((**data) == '0') {
        (*data)++;
        (*len1) = 0; /* ָ���Ŀ�ȣ������ */
    }

    /* Ŀǰ��ֻ֧��С�ڵ���9λ�Ŀ�� */
    if (((**data) >= '0') && ((**data) <= '9')) {
        (*len1) = (VOS_UINT32)(VOS_INT32)((**data) - '0'); /* ָ���Ŀ�� */
        (*data)++;
    }
}

/*
 * ��������: �ַ������͵Ĵ���
 */
LOCAL VOS_VOID AT_ProcFormatTypeForString(VOS_CHAR *s , VOS_INT32 *length)
{
    VOS_CHAR  *sc       = NULL;

    if (s == NULL) {
        (*length) += (VOS_INT32)strlen("(null)");
        return;
    }

    for (sc = s; *sc != '\0'; ++sc) {
        (*length)++;
    }
    return;
}

/*
 * ��������: �������͵Ĵ���
 */
LOCAL VOS_VOID AT_ProcFormatTypeForInteger(VOS_INT32 numSrc, VOS_UINT32 len1, VOS_INT32 *length,
    VOS_UINT32 *len2, VOS_UINT32 *signFlag)
{
    VOS_INT32  numTemp  = numSrc; /* ת��������� */

    (*length)++;
    (*len2)++; /* ʵ�ʵĿ�� */

    /* �з�����ʮ�����������ǰ���ִ���9��С��-9ʱ����Ҫͨ����10�������ֿ�� */
    while ((numTemp > AT_DEC_MAX_NUM) || (numTemp < AT_DEC_SIGNED_MIN_NUM)) {
        numTemp = numTemp / 10;
        (*length)++;
        (*len2)++; /* ʵ�ʵĿ�� */
    }

    if ((*len2) < len1) {
        (*length) += (VOS_INT32)(len1 - (*len2));
    }
    /* ���ulLen2 >= ulLen1,��lNumSrc<0����Ҫ���ϸ��ŵ�һ���ռ� */
    else if (numSrc < 0 || (*signFlag) == VOS_TRUE) {
        (*length)++;
        (*signFlag) = VOS_FALSE;
    } else {
    }
}

/*
 * ��������: �޷����������͵Ĵ���
 */
LOCAL VOS_VOID AT_ProcFormatTypeForUnInteger(VOS_UINT32 num, VOS_UINT32 len1, VOS_INT32 *length, VOS_UINT32 *len2)
{
    (*length)++;
    (*len2)++; /* ʵ�ʵĿ�� */
    /* �޷�����ʮ�����������ǰ���ִ���9ʱ����Ҫͨ����10�������ֿ�� */
    while (num > AT_DEC_MAX_NUM) {
        num = num / 10;
        (*length)++;
        (*len2)++; /* ʵ�ʵĿ�� */
    }

    if ((*len2) < len1) {
        (*length) += (VOS_INT32)(len1 - (*len2));
    }
}

/*
 * ��������: ʮ���������͵Ĵ���
 */
LOCAL VOS_VOID AT_ProcFormatTypeForHex(VOS_UINT32 num, VOS_UINT32 len1, VOS_INT32 *length, VOS_UINT32 *len2)
{
    (*length)++;
    (*len2)++; /* ʵ�ʵĿ�� */

    while (num > 0x0f) {
        /* ʮ�������������ǰ���ִ���0x0fʱ����Ҫͨ����16�������ֿ�� */
        num = num / 16;
        (*length)++;
        (*len2)++; /* ʵ�ʵĿ�� */
    }

    if ((*len2) < len1) {
        (*length) += (VOS_INT32)(len1 - (*len2));
    }
}

/*lint -e713 -e507 -e530*/
VOS_INT32 AT_FormatReportString(VOS_INT32 maxLength, VOS_CHAR *headAddr, VOS_CHAR *currAddr, const VOS_CHAR *fmt, ...)
{
    VOS_INT32  length   = 0;
    VOS_UINT32 num      = 0;
    VOS_INT32  numSrc   = 0; /* ԭʼ���� */
    VOS_UINT32 signFlag = VOS_FALSE;
    VOS_CHAR  *data     = NULL;
    VOS_CHAR  *s        = NULL;
    va_list    args;
    VOS_UINT32 len1 = 0; /* ulLen1:%�����õĳ���; */
    VOS_UINT32 len2 = 0; /* ulLen2:ʵ�ʶ�ȡ�ĳ��� */

    if (AT_CheckFormatReportString(headAddr, currAddr) != VOS_TRUE) {
        return 0;
    }

    va_start(args, fmt);
    for (data = (VOS_CHAR *)fmt; *data != '\0'; ++data) {
        if (*data != '%') {
            length++; /* ��1 */
            continue;
        }

        AT_ProcFormatReportType(&data, &signFlag, &len1, &len2);

        /* Ŀǰ��ֻ���õ�c,d,s,X,���֣�����һ����Ϊ���� */
        switch (*data) {
            case 'c':
                length++;
                break;

            case 's':
                s = va_arg(args, VOS_CHAR *);
                AT_ProcFormatTypeForString(s, &length);
                break;

            case 'd':
                numSrc  = va_arg(args, VOS_INT32);
                AT_ProcFormatTypeForInteger(numSrc, len1, &length, &len2, &signFlag);
                break;

            case 'u':
                num = va_arg(args, VOS_UINT32);
                AT_ProcFormatTypeForUnInteger(num, len1, &length, &len2);
                break;

            case 'X':
            case 'x':
                num = va_arg(args, VOS_UINT32);
                AT_ProcFormatTypeForHex(num, len1, &length, &len2);
                break;

            default:
                va_end(args);
                return 0;
        }
    }
    va_end(args);

    if (currAddr - headAddr + length >= (VOS_INT32)(maxLength)) {
        return 0;
    }

    va_start(args, fmt);

    if (length != vsnprintf_s(currAddr, (VOS_SIZE_T)(maxLength - (currAddr - headAddr)),
                              (VOS_SIZE_T)(maxLength - (currAddr - headAddr) - 1), fmt, args)) {
        va_end(args);
        return 0;
    }

    va_end(args);

    return length;
}

/*lint +e713 +e507 +e530*/

TAF_VOID AT_MapCallTypeModeToClccMode(MN_CALL_TypeUint8 callType, VOS_UINT8 *clccMode)
{
    switch (callType) {
        case MN_CALL_TYPE_VOICE:
        case MN_CALL_TYPE_EMERGENCY:
        case MN_CALL_TYPE_MIEC:
        case MN_CALL_TYPE_AIEC:
        case MN_CALL_TYPE_TEST:
        case MN_CALL_TYPE_RECFGURATION:
        case MN_CALL_TYPE_PSAP_ECALL:
            *clccMode = AT_CLCC_PARA_MODE_VOICE;
            break;
        case MN_CALL_TYPE_FAX:
            *clccMode = AT_CLCC_PARA_MODE_FAX;
            break;
        default:
            *clccMode = AT_CLCC_PARA_MODE_DATA;
            break;
    }
}

AT_CmdCurOptUint32 At_GetMnOptType(MN_MMI_OperationTypeUint8 mnOptType)
{
    AT_CmdCurOptUint32  cmdCurrentOpt = AT_CMD_INVALID;
    VOS_UINT32          loop;
    VOS_UINT32          itemsNum;

    itemsNum = sizeof(g_atMnOptTypeConvertAtCmdTypeTab) / sizeof(AT_MnOptTypeConvertAtCmdOptType);

    for (loop = 0; loop < itemsNum; loop++) {
        if ((VOS_UINT8)g_atMnOptTypeConvertAtCmdTypeTab[loop].mnOptType == mnOptType) {
            cmdCurrentOpt = (AT_CmdCurOptUint32)g_atMnOptTypeConvertAtCmdTypeTab[loop].atCmdType;
            break;
        }
    }

    return cmdCurrentOpt;
}

VOS_UINT32 At_UnlockSimLock(VOS_UINT8 indexNum, VOS_UINT32 paraCnt, AT_ParseParaType *paralist)
{
    AT_ParseParaType       *pwdPara = VOS_NULL_PTR;
    DRV_AGENT_SimlockSetReq simlockReq;
    errno_t                 memResult;

    simlockReq.pwdLen = 0;

    if (paraCnt == AT_SIMLOCK_VALID_PARA_NUM) {
        pwdPara = (AT_ParseParaType *)(paralist + 1);

        if (pwdPara->paraLen == TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX) {
            if (At_CheckNumString(pwdPara->para, TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX) != AT_FAILURE) {
                simlockReq.pwdLen = TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX;

                memResult = memcpy_s(simlockReq.pwd, sizeof(simlockReq.pwd), pwdPara->para,
                                     TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);
                TAF_MEM_CHK_RTN_VAL(memResult, sizeof(simlockReq.pwd), TAF_PH_ME_PERSONALISATION_PWD_LEN_MAX);
                simlockReq.pwd[simlockReq.pwdLen] = 0;
            }
        }
    }

    if (AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                               DRV_AGENT_SIMLOCK_SET_REQ, (VOS_UINT8 *)&simlockReq, sizeof(simlockReq),
                               I0_WUEPS_PID_DRV_AGENT) == TAF_SUCCESS) {
        /* ���õ�ǰ����ģʽ */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_DRV_AGENT_SIMLOCK_SET_REQ;
        (VOS_VOID)memset_s(simlockReq.pwd, sizeof(simlockReq.pwd), 0x00, sizeof(simlockReq.pwd));
        /* ����SIMLOCK���⴦�����Ｔʹ���صȴ��첽��Ϣ������Ҳ������ʱ�� */
        if (At_StartTimer(AT_SET_PARA_TIME, indexNum) != AT_SUCCESS) {
            At_FormatResultData(indexNum, AT_ERROR);
            return VOS_ERR;
        }

        g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_PEND;

        return VOS_OK;
    }

    (VOS_VOID)memset_s(simlockReq.pwd, sizeof(simlockReq.pwd), 0x00, sizeof(simlockReq.pwd));
    At_FormatResultData(indexNum, AT_ERROR);
    return VOS_ERR;
}

/*
 * ��������: ��ȡxml�Ĳ�������
 */
LOCAL VOS_UINT32 At_GetXmlProcType(const TAF_UINT8 *data, VOS_UINT16 *tmpLen, AT_XmlProcTypeUint8 *xmlProcType, MTA_AT_CposOperateTypeUint32 *cposOpType)
{
    /* �յ�"Ctrl-Z"�ַ������ͱ�����Ϣ */
    if (data[(*tmpLen) - 1] == AT_CTRL_Z) {
        (*tmpLen) -= 1;
        (*xmlProcType) = AT_XML_PROC_TYPE_FINISH;
        (*cposOpType)  = MTA_AT_CPOS_SEND;
    }
    /* �յ�"CR"�ַ��������ȴ����� */
    else if (data[(*tmpLen) - 1] == AT_CR) {
        (*xmlProcType) = AT_XML_PROC_TYPE_CONTINUE;
    }
    /* �յ�"ESC"�ַ���ȡ��������Ϣ���� */
    else if (data[(*tmpLen) - 1] == AT_ESC) {
        (*tmpLen) -= 1;
        (*xmlProcType) = AT_XML_PROC_TYPE_FINISH;
        (*cposOpType)  = MTA_AT_CPOS_CANCEL;
    } else {
        return AT_ERROR;
    }

    return AT_SUCCESS;
}

VOS_UINT32 At_ProcXmlText(TAF_UINT8 indexNum, TAF_UINT8 *data, TAF_UINT16 len)
{
    errno_t                      memResult;
    VOS_UINT16                   tmpLen;
    AT_XmlProcTypeUint8          xmlProcType;
    AT_MTA_CposReq              *cposSetReq = VOS_NULL_PTR;
    VOS_UINT32                   result;
    MTA_AT_CposOperateTypeUint32 cposOpType;
    AT_ModemAgpsCtx             *agpsCtx = VOS_NULL_PTR;

    agpsCtx = AT_GetModemAgpsCtxAddrFromClientId(indexNum);

    /* �ֲ�������ʼ�� */
    tmpLen      = len;
    xmlProcType = AT_XML_PROC_TYPE_BUTT;
    cposOpType  = MTA_AT_CPOS_BUTT;

    if (At_GetXmlProcType(data, &tmpLen, &xmlProcType, &cposOpType) == AT_ERROR) {
        return AT_ERROR;
    }

    /* �ѻ��������ַ��ӵ�XML������ */
    if (cposOpType != MTA_AT_CPOS_CANCEL) {
        /* XML���������ж� */
        if (((agpsCtx->xml.xmlTextCur - agpsCtx->xml.xmlTextHead) + tmpLen) > AT_XML_MAX_LEN) {
            return AT_ERROR;
        }

        if (tmpLen > 0) {
            memResult = memcpy_s(agpsCtx->xml.xmlTextCur,
                                 AT_XML_MAX_LEN - (unsigned int)(agpsCtx->xml.xmlTextCur - agpsCtx->xml.xmlTextHead),
                                 data, tmpLen);
            TAF_MEM_CHK_RTN_VAL(memResult,
                                AT_XML_MAX_LEN - (unsigned int)(agpsCtx->xml.xmlTextCur - agpsCtx->xml.xmlTextHead),
                                tmpLen);
        }

        agpsCtx->xml.xmlTextCur += tmpLen;
    }

    /* �·�XML����������ȴ����� */
    if (xmlProcType == AT_XML_PROC_TYPE_FINISH) {
        cposSetReq = (AT_MTA_CposReq *)PS_MEM_ALLOC(WUEPS_PID_AT, sizeof(AT_MTA_CposReq));
        if (cposSetReq == VOS_NULL_PTR) {
            AT_ERR_LOG("At_ProcXmlText: Memory malloc failed!");
            return AT_ERROR;
        }
        (VOS_VOID)memset_s(cposSetReq, sizeof(AT_MTA_CposReq), 0x00, sizeof(AT_MTA_CposReq));

        /* ��д��Ϣ�ṹ */
        cposSetReq->cposOpType = cposOpType;

        /* ���û�������Ctrl-Z�Ž���������MTA */
        if (cposOpType == MTA_AT_CPOS_SEND) {
            cposSetReq->xmlLength = (VOS_UINT32)(agpsCtx->xml.xmlTextCur - agpsCtx->xml.xmlTextHead);
            if (cposSetReq->xmlLength > 0) {
                memResult = memcpy_s(cposSetReq->xmlText, MTA_CPOS_XML_MAX_LEN, agpsCtx->xml.xmlTextHead,
                                     cposSetReq->xmlLength);
                TAF_MEM_CHK_RTN_VAL(memResult, MTA_CPOS_XML_MAX_LEN, cposSetReq->xmlLength);
            }
        }

        /* ���͵�MTAģ�� */
        result = AT_FillAndSndAppReqMsg(g_atClientTab[indexNum].clientId, g_atClientTab[indexNum].opId,
                                        ID_AT_MTA_CPOS_SET_REQ, (VOS_UINT8 *)cposSetReq, sizeof(AT_MTA_CposReq),
                                        I0_UEPS_PID_MTA);

        /* �ͷ�XML�����ṹ */
        /*lint -save -e516 */
        PS_MEM_FREE(WUEPS_PID_AT, cposSetReq);
        /*lint -restore */

        if (result != TAF_SUCCESS) {
            AT_ERR_LOG("At_ProcXmlText: AT_FillAndSndAppReqMsg fail.");
            return AT_ERROR;
        }

        /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOS_SET;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_WAIT_XML_INPUT;
}

/* ɾ��^EOPLMN��������Ĳ�������������ʹ��ͨ��AT�������������� */
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)

TAF_UINT32 At_CheckCurrRatModeIsCL(VOS_UINT8 indexNum)
{
    ModemIdUint16 modemId = MODEM_ID_0;
    VOS_UINT32    getModemIdRslt;
    VOS_UINT8    *isCLMode = VOS_NULL_PTR;

    getModemIdRslt = AT_GetModemIdFromClient(indexNum, &modemId);

    /* ��ȡmodemidʧ�ܣ�Ĭ�ϲ�������Ӧ��AT���� */
    if (getModemIdRslt != VOS_OK) {
        AT_ERR_LOG("At_CurrRatModeIsCL:Get Modem Id fail!");
        return VOS_FALSE;
    }

    isCLMode = AT_GetModemCLModeCtxAddrFromModemId(modemId);

    if (*isCLMode == VOS_TRUE) {
        return VOS_TRUE;
    } else {
        return VOS_FALSE;
    }
}
#endif

VOS_UINT32 AT_ConvertMtaResult(MTA_AT_ResultUint32 result)
{
    AT_RreturnCodeUint32 returnCode;

    switch (result) {
        case MTA_AT_RESULT_NO_ERROR:
            returnCode = AT_OK;
            break;
        case MTA_AT_RESULT_INCORRECT_PARAMETERS:
            returnCode = AT_CME_INCORRECT_PARAMETERS;
            break;
        case MTA_AT_RESULT_NETWORK_TIMEOUT:
            returnCode = AT_CME_NETWORK_TIMEOUT;
            break;
        case MTA_AT_RESULT_ERROR:
        default:
            returnCode = AT_ERROR;
            break;
    }

    return returnCode;
}

VOS_UINT32 AT_GetSeconds(VOS_VOID)
{
    VOS_UINT32 ulsecond;
    VOS_UINT32 ultimelow    = 0;
    VOS_UINT32 ultimehigh   = 0;
    VOS_UINT64 ulltimeCount = 0;

    if (AT_GetTimerAccuracyTimestamp(&ultimehigh, &ultimelow) == ERR_MSP_SUCCESS) {
        ulltimeCount = ((VOS_UINT64)ultimehigh << 32) | ((VOS_UINT64)ultimelow);
        ulltimeCount = ulltimeCount / SLICE_TO_SECOND_UINT;
    }

    /* ��U64��sliceת��Ϊ��֮��ֻ��Ҫȡ��32λ���ɡ���32λ������ͳ�Ƶ�ʱ���Ѿ��ﵽ136�� */
    ulsecond = (VOS_UINT32)(ulltimeCount & SLICE_TO_MAX_SECOND);

    return ulsecond;
}

VOS_VOID At_AdjustLocalDate(TIME_ZONE_Time *uinversalTime, VOS_INT8 adjustValue, TIME_ZONE_Time *localTime)
{
    VOS_UINT8 day;

    /* �������� */
    day = (VOS_UINT8)(uinversalTime->day + adjustValue);

    if (day == 0) {
        /* �·ݼ�һ */
        if (uinversalTime->month == 1) {
            /* ����Ϊ��һ���12�·�,��ݼ�һ */
            localTime->month = 12;

            if (uinversalTime->year == 0) {
                /* �����2000�꣬����Ϊ1999�� */
                localTime->year = 99;
            } else {
                localTime->year = uinversalTime->year - 1;
            }
        } else {
            localTime->month = uinversalTime->month - 1;
            localTime->year  = uinversalTime->year;
        }

        /* ���ڵ���Ϊ�ϸ��µ����һ��, */
        localTime->day = At_GetDaysForEachMonth(localTime->year, localTime->month);
    } else if (day > At_GetDaysForEachMonth(uinversalTime->year, uinversalTime->month)) {
        /* ���ڵ���Ϊ�¸���һ�� */
        localTime->day = 1;

        /* �·ݼ�һ ��ǰ�·�Ϊ12 */
        if (uinversalTime->month == 12) {
            /* ����Ϊ��һ���1�·�,��ݼ�һ */
            localTime->month = 1;
            localTime->year  = uinversalTime->year + 1;
        } else {
            localTime->month = uinversalTime->month + 1;
            localTime->year  = uinversalTime->year;
        }
    } else {
        localTime->day   = day;
        localTime->month = uinversalTime->month;
        localTime->year  = uinversalTime->year;
    }
}

VOS_VOID AT_GetLiveTime(NAS_MM_InfoInd *aTtime, NAS_MM_InfoInd *newTime, VOS_UINT32 nwSecond)
{
    TIME_ZONE_Time newTimeInfo = {0};
    VOS_UINT32     nowSecond;
    VOS_UINT32     seconds;
    VOS_UINT32     adjustDate;
    VOS_UINT32     indexNum;
    errno_t        memResult = 0;

    /* ȫ�ֱ����б�����ʱ����Ϣ */
    if ((aTtime->ieFlg & NAS_MM_INFO_IE_UTLTZ) == NAS_MM_INFO_IE_UTLTZ) {
        memResult = memcpy_s(newTime, sizeof(NAS_MM_InfoInd), aTtime, sizeof(NAS_MM_InfoInd));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(NAS_MM_InfoInd), sizeof(NAS_MM_InfoInd));

        if (nwSecond == 0) {
            return;
        }

        nowSecond = AT_GetSeconds();

        /* ���ulNowSecond ������slice��ʾ�����ֵ����ᷴת����ת֮����Ҫ�������ֵ */
        if (nowSecond >= nwSecond) {
            seconds = nowSecond - nwSecond;
        } else {
            /* �ο������������ʵ��, +1��ԭ���Ǿ���0�ļ��� */
            seconds = SLICE_TO_MAX_SECOND - nwSecond + nowSecond + 1;
        }

        if (seconds != 0) {
            memResult = memcpy_s(&newTimeInfo, sizeof(TIME_ZONE_Time), &aTtime->universalTimeandLocalTimeZone,
                                 sizeof(TIME_ZONE_Time));
            TAF_MEM_CHK_RTN_VAL(memResult, sizeof(TIME_ZONE_Time), sizeof(TIME_ZONE_Time));

            newTimeInfo.second += (VOS_UINT8)(seconds % AT_MINUTE_TO_SECONDS);                          /* �� */
            newTimeInfo.minute += (VOS_UINT8)((seconds / AT_MINUTE_TO_SECONDS) % AT_MINUTE_TO_SECONDS); /* �� */
            newTimeInfo.hour +=
                (VOS_UINT8)((seconds / (AT_MINUTE_TO_SECONDS * AT_HOUR_TO_MINUTES)) % AT_DAY_TO_HOURS); /* ʱ */
            adjustDate = (seconds / (AT_MINUTE_TO_SECONDS * AT_HOUR_TO_MINUTES * AT_DAY_TO_HOURS));     /* �� */

            if (newTimeInfo.second >= AT_MINUTE_TO_SECONDS) {
                newTimeInfo.minute += newTimeInfo.second / AT_MINUTE_TO_SECONDS; /* �� */
                newTimeInfo.second = newTimeInfo.second % AT_MINUTE_TO_SECONDS;  /* ʵ�ʵ����� */
            }

            if (newTimeInfo.minute >= AT_HOUR_TO_MINUTES) {
                newTimeInfo.hour += newTimeInfo.minute / AT_HOUR_TO_MINUTES;  /* Сʱ */
                newTimeInfo.minute = newTimeInfo.minute % AT_HOUR_TO_MINUTES; /* ʵ�ʵķ��� */
            }

            if ((adjustDate > 0) || (newTimeInfo.hour >= AT_DAY_TO_HOURS)) {
                adjustDate += (newTimeInfo.hour / AT_DAY_TO_HOURS);    /* ���� */
                newTimeInfo.hour = newTimeInfo.hour % AT_DAY_TO_HOURS; /* ʵ�ʵ�Сʱ�� */

                memResult = memcpy_s(&newTime->universalTimeandLocalTimeZone, sizeof(TIME_ZONE_Time), &newTimeInfo,
                                     sizeof(TIME_ZONE_Time));
                TAF_MEM_CHK_RTN_VAL(memResult, sizeof(TIME_ZONE_Time), sizeof(TIME_ZONE_Time));

                for (indexNum = 0; indexNum < adjustDate; indexNum++) {
                    At_AdjustLocalDate(&newTimeInfo, 1, &newTime->universalTimeandLocalTimeZone);
                    memResult = memcpy_s(&newTimeInfo, sizeof(TIME_ZONE_Time), &newTime->universalTimeandLocalTimeZone,
                                         sizeof(TIME_ZONE_Time));
                    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(TIME_ZONE_Time), sizeof(TIME_ZONE_Time));
                }
            } else {
                memResult = memcpy_s(&newTime->universalTimeandLocalTimeZone, sizeof(TIME_ZONE_Time), &newTimeInfo,
                                     sizeof(TIME_ZONE_Time));
                TAF_MEM_CHK_RTN_VAL(memResult, sizeof(TIME_ZONE_Time), sizeof(TIME_ZONE_Time));
            }
        }
    }
}

#if (FEATURE_MBB_CUST == FEATURE_ON)
VOS_UINT8 AT_IsModemLoopOn(VOS_VOID)
{
    return g_modemLoop;
}

VOS_VOID AT_WriteModemLoop(VOS_UINT8 value)
{
    g_modemLoop = value;
}
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
/*
 * ��������: �յ�"Ctrl-Z"�ַ���������Ϣ����
 */
LOCAL VOS_UINT32 At_ProcStreamTypeFinish(VOS_UINT8 clientId, AT_ModemUePolicyCtx *uePolicyCtx)
{
    TAF_PS_5GUePolicyInfo *uePolicyInfo   = VOS_NULL_PTR;
    errno_t                memResult;
    VOS_UINT32             allocLen;

    allocLen     = sizeof(TAF_PS_5GUePolicyInfo) + uePolicyCtx->uePolicyLen / 2;
    uePolicyInfo = (TAF_PS_5GUePolicyInfo *)PS_MEM_ALLOC(WUEPS_PID_AT, allocLen);

    if (uePolicyInfo == VOS_NULL_PTR) {
        AT_ERR_LOG("At_ProcStreamTypeFinish: Memory malloc failed!");
        return AT_ERROR;
    }
    (VOS_VOID)memset_s(uePolicyInfo, allocLen, 0x00, allocLen);

    /* ��д��Ϣ�ṹ */
    uePolicyInfo->protocalVer     = uePolicyCtx->protocalVer;
    uePolicyInfo->pti             = uePolicyCtx->pti;
    uePolicyInfo->uePolicyMsgType = uePolicyCtx->uePolicyMsgType;
    uePolicyInfo->classMarkLen    = uePolicyCtx->classMarkLen;
    memResult = memcpy_s(uePolicyInfo->classMark, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN, uePolicyCtx->classMark,
                         TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN);

    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN, TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN);

    uePolicyInfo->ueOsIdLen = uePolicyCtx->ueOsIdLen;
    memResult = memcpy_s(uePolicyInfo->ueOsIdInfo, TAF_PS_UE_OS_ID_INFO_MAX_LEN, uePolicyCtx->ueOsIdInfo,
                         TAF_PS_UE_OS_ID_INFO_MAX_LEN);

    TAF_MEM_CHK_RTN_VAL(memResult, TAF_PS_UE_OS_ID_INFO_MAX_LEN, TAF_PS_UE_OS_ID_INFO_MAX_LEN);

    uePolicyInfo->infoLen = uePolicyCtx->uePolicyLen / 2;
    if (At_AsciiString2HexSimple(uePolicyInfo->content, uePolicyCtx->uePolicyHead,
                                 (VOS_UINT16)uePolicyCtx->uePolicyLen) != AT_SUCCESS) {
        PS_MEM_FREE(WUEPS_PID_AT, uePolicyInfo);
        return AT_ERROR;
    }

    /* ����DSM�ṩ�����ýӿ� */
    if (TAF_PS_SetCsUePolicy(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[clientId].clientId), 0,
                             uePolicyInfo) != VOS_OK) {
        PS_MEM_FREE(WUEPS_PID_AT, uePolicyInfo);
        return AT_ERROR;
    }

    PS_MEM_FREE(WUEPS_PID_AT, uePolicyInfo);

    g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_CSUEPOLICY_SET;
    return AT_WAIT_ASYNC_RETURN;
}

LOCAL VOS_UINT32 At_ProcUePolicyInfo(VOS_UINT8 clientId, VOS_UINT8 *data, VOS_UINT16 length)
{
    errno_t                memResult;
    VOS_UINT16             tmpLen         = length - 1;
    AT_StreamProcTypeUint8 streamProcType = AT_STREAM_PROC_TYPE_BUTT;
    AT_ModemUePolicyCtx   *uePolicyCtx    = VOS_NULL_PTR;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);

    /* �յ�"Ctrl-Z"�ַ������ͱ�����Ϣ */
    if (data[tmpLen] == AT_CTRL_Z) {
        streamProcType = AT_STREAM_PROC_TYPE_FINISH;
    }
    /* �յ�"CR"�ַ��������ȴ����� */
    else if (data[tmpLen] == AT_CR) {
        streamProcType = AT_STREAM_PROC_TYPE_CONTINUE;
    }
    /* �յ�"ESC"�ַ���ȡ��������Ϣ���� */
    else if (data[tmpLen] == AT_ESC) {
        streamProcType = AT_STREAM_PROC_TYPE_CANCEL;
    } else {
        return AT_ERROR;
    }

    /* �ѻ��������ַ��ӵ������� */
    if (streamProcType != AT_STREAM_PROC_TYPE_CANCEL) {
        /* ���������ж� */
        if ((VOS_UINT32)((uePolicyCtx->uePolicyCur - uePolicyCtx->uePolicyHead) + tmpLen) > uePolicyCtx->uePolicyLen) {
            AT_ERR_LOG("At_ProcUePolicyInfo: UE POLICY stream too long!");
            return AT_ERROR;
        }

        if (tmpLen > 0) {
            memResult = memcpy_s(uePolicyCtx->uePolicyCur,
                                 uePolicyCtx->uePolicyLen -
                                     (unsigned int)(uePolicyCtx->uePolicyCur - uePolicyCtx->uePolicyHead),
                                 data, tmpLen);
            TAF_MEM_CHK_RTN_VAL(memResult,
                                uePolicyCtx->uePolicyLen -
                                    (unsigned int)(uePolicyCtx->uePolicyCur - uePolicyCtx->uePolicyHead),
                                tmpLen);
        }

        uePolicyCtx->uePolicyCur += tmpLen;
    }

    /* �·�����������ȴ����� */
    if (streamProcType == AT_STREAM_PROC_TYPE_FINISH) {
        if (At_ProcStreamTypeFinish(clientId, uePolicyCtx) == AT_ERROR) {
            return AT_ERROR;
        } else {
            return AT_WAIT_ASYNC_RETURN;
        }
    } else if (streamProcType == AT_STREAM_PROC_TYPE_CANCEL) {
        return AT_OK;
    } else {
        return AT_WAIT_UE_POLICY_INPUT;
    }
}

LOCAL VOS_UINT32 At_ProcSendImsUrsp(VOS_UINT8 clientId)
{
    AT_ModemUePolicyCtx   *uePolicyCtx  = VOS_NULL_PTR;
    AT_IMSA_ImsUrspSetReq *urspSetReq   = VOS_NULL_PTR;
    VOS_UINT32             allocLen;
    VOS_UINT32             rst;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);

    allocLen   = sizeof(AT_IMSA_ImsUrspSetReq) + uePolicyCtx->imsUrspLen / 2;
    urspSetReq = (AT_IMSA_ImsUrspSetReq *)PS_MEM_ALLOC(WUEPS_PID_AT, allocLen);

    if (urspSetReq == VOS_NULL_PTR) {
        AT_ERR_LOG("At_ProcSendImsUrsp: Memory malloc failed!");
        return AT_ERROR;
    }
    (VOS_VOID)memset_s(urspSetReq, allocLen, 0x00, allocLen);

    /* ��д��Ϣ�ṹ */
    urspSetReq->version = (VOS_UINT16)uePolicyCtx->imsUrspVer;

    urspSetReq->length = (VOS_UINT16)(uePolicyCtx->imsUrspLen / 2);
    if (At_AsciiString2HexSimple(urspSetReq->data, uePolicyCtx->imsUrspHead,
                                 (VOS_UINT16)uePolicyCtx->imsUrspLen) != AT_SUCCESS) {
        PS_MEM_FREE(WUEPS_PID_AT, urspSetReq);
        return AT_ERROR;
    }

    rst = AT_FillAndSndAppReqMsg(g_atClientTab[clientId].clientId, g_atClientTab[clientId].opId,
              ID_AT_IMSA_IMS_URSP_SET_REQ, (VOS_UINT8 *)(&urspSetReq->version),
              (VOS_UINT32)(sizeof(urspSetReq->version) + sizeof(urspSetReq->length) + urspSetReq->length),
              PS_PID_IMSA);

    if (rst != TAF_SUCCESS) {
        PS_MEM_FREE(WUEPS_PID_AT, urspSetReq);
        AT_ERR_LOG("At_ProcSendImsUrsp: AT_FillAndSndAppReqMsg FAIL");
        return AT_ERROR;
    }

    PS_MEM_FREE(WUEPS_PID_AT, urspSetReq);

    g_atClientTab[clientId].cmdCurrentOpt = AT_CMD_IMSURSP_SET;

    return AT_WAIT_ASYNC_RETURN;
}

LOCAL VOS_UINT32 At_ProcImsUrspInfo(VOS_UINT8 clientId, VOS_UINT8 *data, VOS_UINT16 length)
{
    AT_ModemUePolicyCtx   *uePolicyCtx  = VOS_NULL_PTR;
    errno_t                memRslt;
    VOS_UINT16             tmpLen         = length - 1;
    AT_StreamProcTypeUint8 streamProcType = AT_STREAM_PROC_TYPE_BUTT;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);

    /* �յ�"Ctrl-Z"�ַ������ͱ�����Ϣ */
    if (data[tmpLen] == AT_CTRL_Z) {
        streamProcType = AT_STREAM_PROC_TYPE_FINISH;
    }
    /* �յ�"CR"�ַ��������ȴ����� */
    else if (data[tmpLen] == AT_CR) {
        streamProcType = AT_STREAM_PROC_TYPE_CONTINUE;
    }
    /* �յ�"ESC"�ַ���ȡ��������Ϣ���� */
    else if (data[tmpLen] == AT_ESC) {
        return AT_OK;
    } else {
        return AT_ERROR;
    }

    /* �ѻ��������ַ��ӵ������� */
    if ((VOS_UINT32)((uePolicyCtx->imsUrspCur - uePolicyCtx->imsUrspHead) + tmpLen) > uePolicyCtx->imsUrspLen) {
        AT_ERR_LOG("At_ProcImsUrspInfo: ims ursp stream too long!");
        return AT_ERROR;
    }

    if (tmpLen > 0) {
        memRslt = memcpy_s(uePolicyCtx->imsUrspCur,
                      uePolicyCtx->imsUrspLen - (unsigned int)(uePolicyCtx->imsUrspCur - uePolicyCtx->imsUrspHead),
                      data, tmpLen);
        TAF_MEM_CHK_RTN_VAL(memRslt,
           uePolicyCtx->imsUrspLen - (unsigned int)(uePolicyCtx->imsUrspCur - uePolicyCtx->imsUrspHead), tmpLen);
    }

    uePolicyCtx->imsUrspCur += tmpLen;

    /* �·�����������ȴ����� */
    if (streamProcType == AT_STREAM_PROC_TYPE_FINISH) {
        /* �������� */
        return At_ProcSendImsUrsp(clientId);
    } else {
        return AT_WAIT_IMSA_URSP_INPUT;
    }
}

VOS_VOID At_ProcUePolicyInfoStream(VOS_UINT8 clientId, VOS_UINT8 *data, VOS_UINT16 length, AT_ParseContext *clientCxt)
{
    VOS_UINT32           result;
    AT_ModemUePolicyCtx *uePolicyCtx = VOS_NULL_PTR;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);
    result      = At_ProcUePolicyInfo(clientId, data, length);

    /* ����ɹ��������ʾ��">" */
    if (result == AT_WAIT_UE_POLICY_INPUT) {
        At_FormatResultData(clientId, result);
        return;
    }

    /* ��ջ����� */
    PS_MEM_FREE(WUEPS_PID_AT, uePolicyCtx->uePolicyHead); /* ��� */
    (VOS_VOID)memset_s(uePolicyCtx, sizeof(AT_ModemUePolicyCtx), 0x00, sizeof(AT_ModemUePolicyCtx));

    At_SetCmdSubMode(clientId, AT_NORMAL_MODE); /* �û�����״̬ */

    if (result == AT_WAIT_ASYNC_RETURN) {
        if (clientCxt->cmdElement != NULL) {
            if (At_StartTimer(clientCxt->cmdElement->setTimeOut, clientId) != AT_SUCCESS) {
                AT_ERR_LOG("At_ProcImsUrspStream():ERROR:Start Timer Failed");
            }

            clientCxt->clientStatus = AT_FW_CLIENT_STATUS_PEND;
        }
        return;
    }

    At_FormatResultData(clientId, result);
}

VOS_VOID At_ProcImsUrspStream(VOS_UINT8 clientId, VOS_UINT8 *data, VOS_UINT16 length, AT_ParseContext *clientCxt)
{
    VOS_UINT32           result;
    AT_ModemUePolicyCtx *uePolicyCtx = VOS_NULL_PTR;

    uePolicyCtx = AT_GetModemUePolicyCtxAddrFromClientId(clientId);

    result = At_ProcImsUrspInfo(clientId, data, length);

    /* ����ɹ��������ʾ��">" */
    if (result == AT_WAIT_IMSA_URSP_INPUT) {
        At_FormatResultData(clientId, result);
        return;
    }

    /* ��ջ����� */
    PS_MEM_FREE(WUEPS_PID_AT, uePolicyCtx->imsUrspHead); /* ��� */
    (VOS_VOID)memset_s(uePolicyCtx, sizeof(AT_ModemUePolicyCtx), 0x00, sizeof(AT_ModemUePolicyCtx));

    At_SetCmdSubMode(clientId, AT_NORMAL_MODE); /* �û�����״̬ */

    if (result == AT_WAIT_ASYNC_RETURN) {
        if (clientCxt->cmdElement != NULL) {
            if (At_StartTimer(clientCxt->cmdElement->setTimeOut, clientId) != AT_SUCCESS) {
                AT_ERR_LOG("At_ProcImsUrspStream():ERROR:Start Timer Failed");
            }

            clientCxt->clientStatus = AT_FW_CLIENT_STATUS_PEND;
        }
        return;
    }

    At_FormatResultData(clientId, result);
}

#endif

