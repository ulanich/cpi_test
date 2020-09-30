
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "aes.h"
#include "ec_params.h"
#include "curves.h"
#include "nn.h"
#include "rand.h"
#include "print.h"
#include "nn_rand.h"
#include "prj_pt_monty.h"
#include "hmac_sha2.h"
#include "sha2.h"

#define DH_KEY_SIZE (68)
#define DH_KEYS_COUNT (3)
#define uswap_32(x) ({\
    uint32_t _x = (x);\
    (uint32_t)(\
        ((_x & 0xff000000UL) >> 24) | \
        ((_x & 0x00ff0000UL) >>  8) | \
        ((_x & 0x0000ff00UL) <<  8) | \
        ((_x & 0x000000ffUL) << 24)); \
})

uint16_t crc16_ccitt(const uint8_t* data_p, uint32_t length)
{
  uint8_t x;
  uint16_t crc = 0xFFFF;
  while (length--){
    x = crc >> 8 ^ *data_p++;
    x ^= x>>4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
  }
  return crc;
}

typedef struct  __attribute__((packed))
{
	int32_t time;
	uint16_t cmd;
	union
	{
		uint8_t args[8];
		uint32_t argsint[2];
	};
}tsCpiCmd;

typedef struct  __attribute__((packed))
{
	tsCpiCmd cmd[3];
}tsCpi01;

typedef struct __attribute__((packed))
{
	uint16_t id:6;
	uint16_t addinfo:5;
	uint16_t wc:5;
} tsRCDheader;

typedef struct __attribute__((packed))
{
	uint8_t MAC[8];
	uint8_t IV[4];
}tsRCDfooter;

typedef struct __attribute__((packed))
{
	uint32_t obts; ///<БШВ 2
	uint32_t heap_avail; ///<Доступно байт в куче 6
	uint8_t idleCPUprcnt; //<Нахождение процессора в простое в процентах от целой секунды 10
	uint8_t activeCyclogramCount;//<Количество активных циклограмм 11
	uint16_t events_count;//<Количество сформированных сообщений 12
	uint16_t errors_count;//<Количество сформированных ошибок 14
	uint32_t lastmilstatus;///признаки обмена с абонентом. 1 означает что последний обмен завершился неудачно. 16
	uint8_t  scsecretpublicformed:1;//<Сформирован общий секрет 20
	uint8_t  sessionMode:1;//<Сеанс Связи 20
	uint8_t reserved:6; //20
	uint8_t milA;//счетчик успешно переданных пакетов по МКОА 21
	uint8_t milB;//счетчик успешно переданных пакетов по МКОБ 22
	uint8_t milAerrors;//счетчик ошибок при передаче пакетов по МКОА 23
	uint8_t milBerrors;//счетчик ошибок при передаче пакетов по МКОБ 24
	uint8_t cycle; //номер цикла работы БВМ5-БКУ  25
	uint8_t cyclesSinceTimeSync;// количество циклов прошедших с момента последней синхронизации времени 26
	uint8_t cyclesSinceSyncPulse;// количество циклов прошедших с момента получения импульса секундной метки 27
	uint8_t CAN1recv;//счетчик принятых пакетов по CAN1 28
	uint8_t CAN2recv;//счетчик принятых пакетов по CAN2 29
	uint8_t PPS[2]; //счетчик принятых СМ  30 31
	uint8_t edacRegNumber; //32
	uint32_t EDACstatusregister; // 33
	uint32_t lastcrc32; //значение последнего посчитанного при помощи CRC32 участка памяти 37
	uint8_t mcuT;//температура МК
	uint8_t _pad[6];
}tsRCD8;

typedef struct __attribute__((packed))
{
	tsRCDheader header;
	union
	{
		uint8_t b[50];
		uint16_t w[25];
		tsRCD8 rcd8;
	};
	tsRCDfooter footer;
}tsRCD;

typedef struct
{
  union
  {
    struct
    {
      uint16_t id : 6;
      uint16_t addinfo:5;
      uint16_t wc : 5;
    };
    uint16_t word;
  };
}tsCpiHeader;
typedef struct
{
  tsCpiHeader header;
  union
  {
    uint8_t b[50];
    uint16_t w[25];
  }data;
  uint8_t MAC[8];
  uint8_t IV[4];

}tsCpiMainStruct;
typedef struct
{
	int32_t time;
	//uint16_t msec;
	uint16_t dest;///< Адрес задачи получателя команды.
	uint16_t cmd;
	union
	{
		uint8_t args[68]; //для возможного хранения разового командного сообщения в лице командного слова и 32 слов данных
		uint16_t words[34];
		uint32_t ints[17];
	};

}ts_cmdStruct;

typedef struct sCmd
{
	uint16_t task_node;
	uint16_t cmd;
}sCmd_t;

typedef enum
{
  CPI_DECODE_OK = 0, ///<Успешное декодирование КПИ
  CPI_UNKNOWN_ID = 1, ///<Неподдерживаемая КПИ
  CPI_NOT_FULL = 2, ///<Неполная последовательность КПИ
  CPI_PHRASE_OVERFLOW = 3, ///<Неверное количество фраз в КПИ
  CPI_UNFINISHED = 4, ///<Перерыв в последовательности
  CPI_DECODE_OVERFLOW = 5, ///<Ошибка распределения памяти
  CPI_BAD_ARG = 6, ///<Неверный аргумент в КПИ
  CPI_BLOCK_ACCESS = 7, ///<КПИ не разрешена
  CPI_EXCHANGE_RT_ERROR = 8, ///<Не удался обмен по МКО с ОУ при выполнении КПИ
  CPI_OBTS_NOT_FOUND = 9, ///</Не удалось найти в очереди по БШВ задание для его отмены
  CPI_INVALID_CRC = 10, ///<Неверная контрольная сумма КПИ
}teCpiDecodeStatus;
enum eCryptoTaskCmd
{
	CMD_CRYPTO_TASK_CALC_SC_Q=1,
	CMD_CRYPTO_TASK_CALC_GCM,
	CMD_CRYPTO_TASK_CALC_PUBLIC,
	CMD_CRYPTO_TASK_SEN_RCI,
	CMD_CRYPTO_TASK_CHANGE_K,
	CMD_CRYPTO_TASK_CALC_NEW_KEY,
	CMD_CRYPTO_TASK_CHANGE_CUR_KEY,
	CMD_CRYPTO_TASK_CHANGE_K0
};

tsRCD rcd = {0};
uint8_t flag_k0; // флаг для того чтобы невозможно было поменять К0 без процедуры рассчета
/********************************************
 ****************CPI DECODE******************
 ********************************************/

uint32_t SFTY_decodeCPI06(tsCpiMainStruct *bytemsg)
{

    ts_cmdStruct cmd = {0,};
    cmd.time = bytemsg->data.w[0];
    uint16_t code = bytemsg->data.w[2];
    if (code == 1)
    {
      cmd.cmd = CMD_CRYPTO_TASK_CALC_PUBLIC;
      CRYPTO_ProcessCommand(&cmd);
    }
    else if (code == 2)
    {
      cmd.cmd = CMD_CRYPTO_TASK_SEN_RCI;
      CRYPTO_ProcessCommand(&cmd);
    }
    else
    {
      cmd.cmd = code;
      memcpy(&cmd.args, &bytemsg->data.w[3], 8);
      //SFTY_ScheduleCMD(&cmd);
    }

  return CPI_DECODE_OK;
}

typedef enum
{
	STATUS_OK=0,
	STATUS_FAIL,
	STATUS_INVALID_MESSAGE,
	STATUS_TIMEDOUT,
	STATUS_RT_BUSY,
	STATUS_INVALID_MIL_LINE,
	STATUS_INVALID_ARGUMENTS,
	STATUS_CYCLOGRAM_ALREADY_STARTED,
	STATUS_CYCLOGRAM_CORRUPTED,
	STATUS_NODE_ID_INCORRECT,
	STATUS_NULL_POINTER,
	STATUS_INCORRECT_SIZE,
	STATUS_MIL_RT_FORBIDEN,
	STATUS_RT_BUSY_SEND_RETRY,
	STATUS_NOT_FOUND,
	STATUS_NO_FREE_SPACE,
	STATUS_INCORRECT_CRC,
	STATUS_UNKNOWN_COMMAND,
	STATUS_OPERATION_ALREADY_IN_PROGRESS,
}teFunctionStatus;

typedef enum
{
	NO_TASK_NODE = 0,
	/**
	 * Задача ведения БШВ
	 */
	OBTS_TASK_NODE =1,
	/**
	 * Задача работы с МКО
	 */
	MIL_1553_DAEMON_TASK_NODE=2,
	/**
	 * Задача сбора ТМИ
	 */
	TLM_TASK_NODE=3,
	/**
	 * Задача обеспечения работы БКУ
	 */
	SFTY_TASK_NODE=4,
	/**
	 * Задача работы с CAN
	 */
	CAN_DAEMON_TASK_NODE=5,
	/**
	 * Задача работы с циклограммами
	 */
	CYC_TASK_NODE=6,
	/**
	 * Задача работы с полезной нагрузкой
	 */

	PLD_TASK_NODE=7,
	/**
	 * Задача обеспечения шифрования
	 */
	CRYPTO_TASK_NODE=8,
	/**
	 * Задча работы с АСН
	 */
	NAV_TASK_NODE=9,
	/**
	 * Задача работы СОС
	 */
	ADCS_OBC_TASK_NODE=10,
	ADCS_TASK_NODE=ADCS_OBC_TASK_NODE,
	/**
	 * Задача работы с ЗД
	 */
	STR_TASK_NODE=11,
	/**
	 * Задача работы с памятью
	 */
	MEM_TASK_NODE=12,
	/**
	 * Задача работы с ДС
	 */
	SUN_SENSOR_TASK_NODE=13,
	/**
	 * Задача работы с КДУ
	 */
	VPS_TASK_NODE=14,
	TASKS_COUNT,
}eTaskNodes;

#define CMD_TABLE_LEN	(128)

sCmd_t commands_table[CMD_TABLE_LEN]={

		[65] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CALC_SC_Q},
		[66] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CALC_GCM},
		[67] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CALC_PUBLIC},
		[68] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_SEN_RCI},
		[69] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CHANGE_K},
		[70] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CALC_NEW_KEY},
		[71] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CHANGE_CUR_KEY},
		[72] = {CRYPTO_TASK_NODE, CMD_CRYPTO_TASK_CHANGE_K0},
};

teFunctionStatus CmdGet(uint32_t cmd, sCmd_t *c)
{
	if(cmd<CMD_TABLE_LEN)
	{
		if (c!=NULL)
			memcpy(c, &commands_table[cmd], sizeof(sCmd_t));
		if (c->cmd==0 && c->task_node==0)
			return STATUS_NOT_FOUND;
		return STATUS_OK;
	}
	return STATUS_INVALID_ARGUMENTS;
}

teFunctionStatus externalCmd2InternalCmd(uint16_t extcmd, sCmd_t *outcmd)
{

	teFunctionStatus res = STATUS_OK;
	if (extcmd<256)
	{
		res =  CmdGet(extcmd, outcmd);
	}
	else if (extcmd>=256 && extcmd<512)//CAN КИР1
	{
		outcmd->task_node = CAN_DAEMON_TASK_NODE;
		outcmd->cmd = extcmd;
	}
	else if (extcmd>=512 && extcmd<768)//CAN КИР2
	{
		outcmd->task_node = CAN_DAEMON_TASK_NODE;
		outcmd->cmd = extcmd;
	}
	else if (extcmd>=768 && extcmd<1024)//CAN КСП
	{
		outcmd->task_node = CAN_DAEMON_TASK_NODE;
		outcmd->cmd = extcmd;
	}
	else if (extcmd>=1024 && extcmd<1280)//CAN БКП
	{
		outcmd->task_node = CAN_DAEMON_TASK_NODE;
		outcmd->cmd = extcmd;
	}
	else if (extcmd>=1280 && extcmd<1536)//CAN КСО
	{
		outcmd->task_node = CAN_DAEMON_TASK_NODE;
		outcmd->cmd = extcmd;
	}
	else if (extcmd & 0xf000)//команды для задач СПО
	{
		eTaskNodes task = extcmd>>12;
		uint16_t cmd = extcmd & 0x0fff;
		outcmd->task_node = task;
		outcmd->cmd = cmd;
	}
	else
	{
		res = STATUS_INVALID_ARGUMENTS;
	}
	return res;
}

uint32_t SFTY_decodeCPI01(tsCpiMainStruct *cpi)//Команды обрезано для КПИ6
{
	uint32_t remove = cpi->header.addinfo == 0x1f?1:0;
	int cmdcount = cpi->header.wc/7;
	tsCpi01 *cpi01 = (tsCpi01 *)cpi->data.b;

	tsCpiCmd *cpicmd = &cpi01->cmd;
	sCmd_t cmd;
	teFunctionStatus res =  externalCmd2InternalCmd(cpicmd->cmd, &cmd);
	if (res == STATUS_OK)
	{
		ts_cmdStruct cmdstruct = {0};
		cmdstruct.time = cpicmd->time;
		cmdstruct.dest = cmd.task_node;
		cmdstruct.cmd  = cmd.cmd;
		memcpy(cmdstruct.args,cpicmd->args, 8);

		CRYPTO_ProcessCommand(&cmd);
	}

	return CPI_DECODE_OK;
}

uint32_t SFTY_decodeCPI07(tsCpiMainStruct *bytemsg)
{
  extern uint8_t DH_stationQNst[DH_KEYS_COUNT*DH_KEY_SIZE+16+2];
  if (bytemsg->header.addinfo < 4)
  {
    memcpy(DH_stationQNst+60*(bytemsg->header.addinfo - 1),bytemsg->data.b,60);
  }

  else
  {
    memcpy(DH_stationQNst+60*(bytemsg->header.addinfo - 1),bytemsg->data.b,(DH_KEYS_COUNT*DH_KEY_SIZE+2+16)-60*(bytemsg->header.addinfo - 1));

    uint16_t crc = DH_stationQNst[(DH_KEYS_COUNT*DH_KEY_SIZE+2+16)-1]<<8|DH_stationQNst[(DH_KEYS_COUNT*DH_KEY_SIZE+2+16)-2];
    uint16_t crcc = crc16_ccitt(DH_stationQNst,(DH_KEYS_COUNT*DH_KEY_SIZE+2+16)-2);

    if (crc == crcc)
    {
      ts_cmdStruct cmd;
      cmd.cmd = CMD_CRYPTO_TASK_CALC_SC_Q;
      CRYPTO_ProcessCommand(&cmd);
    }
    else
    {
      return  CPI_INVALID_CRC;
    }
  }
  if (bytemsg->header.addinfo > 4)
  {
    return CPI_PHRASE_OVERFLOW;
  }
  return CPI_DECODE_OK;
}

uint32_t SFTY_decodeCPI08(tsCpiMainStruct *bytemsg)
{
  extern uint8_t cryp_data[64];

  memcpy(cryp_data,bytemsg,64);

  ts_cmdStruct cmd;
  cmd.cmd = CMD_CRYPTO_TASK_CALC_GCM;
  CRYPTO_ProcessCommand(&cmd);

  return CPI_DECODE_OK;
}

uint32_t SFTY_decodeCPI09(tsCpiMainStruct *cpi)
{
	tsCpi01 *cpi01 = (tsCpi01 *)cpi->data.b;
	int cmdcount = cpi->header.wc/7;
	tsCpiCmd *cpicmd = &cpi01->cmd[0];
	sCmd_t scmd;
	if (cpicmd->cmd == 72 && cpi->header.id == 9)
	{
		teFunctionStatus res =  externalCmd2InternalCmd(cpicmd->cmd, &scmd);
		ts_cmdStruct cmdstruct = {0};
		cmdstruct.dest = scmd.task_node;
		cmdstruct.cmd  = scmd.cmd;
		memcpy(cmdstruct.args,cpicmd->args, 8);
		CRYPTO_ProcessCommand(&scmd);
	}
	if (cpicmd->cmd == 71 && cpi->header.id == 12)
	{
		teFunctionStatus res =  externalCmd2InternalCmd(cpicmd->cmd, &scmd);
		ts_cmdStruct cmdstruct = {0};
		cmdstruct.dest = scmd.task_node;
		cmdstruct.cmd  = scmd.cmd;
		memcpy(cmdstruct.args,cpicmd->args, 8);
		CRYPTO_ProcessCommand(&scmd);
	}
	return CPI_DECODE_OK;
}

uint32_t SFTY_decodeCPI10(tsCpiMainStruct *bytemsg)
{
	extern uint8_t data_Nst[16];
	memcpy(data_Nst,bytemsg->data.b,16);
	ts_cmdStruct cmd;
	cmd.cmd = CMD_CRYPTO_TASK_CALC_NEW_KEY;
	CRYPTO_ProcessCommand(&cmd);

	return CPI_DECODE_OK;
}

uint32_t SFTY_decodeCPI11(tsCpiMainStruct *bytemsg)
{
	extern uint8_t cryp_data[64];

	memcpy(cryp_data,bytemsg,64);
	ts_cmdStruct cmd;
	cmd.cmd = CMD_CRYPTO_TASK_CALC_GCM;
	CRYPTO_ProcessCommand(&cmd);

	return CPI_DECODE_OK;
}

int SFTY_ProcessCPI(uint8_t *_64bytes)
{
	tsCpiMainStruct *cpi = _64bytes;
	#define IV_LEN__ (12)
	uint8_t iv[IV_LEN__] = {0};
	memcpy(iv,cpi->IV,4);
	uint8_t currentkey[32];
	uint8_t decrypt[50];
	uint32_t identifier = cpi->header.id;

	if (identifier != 9)
	{
		flag_k0 = 0;
	}

	if(identifier == 7 || identifier == 10)
	{
		uint8_t stream[62] = {0};
		memcpy(stream, _64bytes, 2);
		memcpy(stream+2, cpi->data.b, 60);
		uint16_t crcc = crc16_ccitt(stream,62);
		uint16_t crc = cpi->IV[3]<<8|cpi->IV[2];

		if (crcc != crc)
		{
			return CPI_INVALID_CRC;
		}
	}

	else if(identifier != 8 && identifier != 11)
	{
		if (identifier == 6)		CryptoGetK0AESKey(currentkey);
		else if(identifier == 9 || identifier == 12) 	CryptoGetKnewAESKey(currentkey);
		else						CryptoGetCurrentAESKey(currentkey);

		int res=aes_gcm_ad(currentkey,32, //KEY
				(uint8_t*)iv,IV_LEN__,//IV
				(uint8_t*)cpi->data.b,50,//Cypher text
				(uint8_t*)&cpi->header,2, //AAD
				(uint8_t *)cpi->MAC, 8,//MAC
				decrypt);

		if (res!=0)
		{
			return CPI_INVALID_CRC;
		}

		memcpy(cpi->data.b,decrypt,50);
	}

	if (identifier == 1 || identifier == 6) // 6 кпи аналогично 1 - му, поэтому решили сократить количество
	{
		return SFTY_decodeCPI01(cpi);
	}
	else if (identifier == 7)
	{
	  return SFTY_decodeCPI07(cpi);
	}
	else if (identifier == 8)
	{
	  return SFTY_decodeCPI08(cpi);
	}
	else if (identifier == 9 || identifier == 12) // аналогично с 9 и 12
	{
		return SFTY_decodeCPI09(cpi);
	}
	else if (identifier == 10)
	{
		return SFTY_decodeCPI10(cpi);
	}
	else if (identifier == 11)
	{
		return SFTY_decodeCPI11(cpi);
	}
	return CPI_UNKNOWN_ID;
}
  /********************************************
   ****************CRYPTO**********************
   ********************************************/

  prj_pt Q_sat;
  aff_pt Q_aff_sat;
  aff_pt Q_aff_sat_rev;

  prj_pt Q_st;
  aff_pt Q_aff_st;
  aff_pt Q_aff_st_rev;

  uint8_t Nsat[16];
  uint8_t Nst[16];

  nn_t d_sat;

  ec_params curve_params;
  nn d_satellite;
  ec_str_params *the_curve_const_parameters;

  uint8_t DH_stationQNst[DH_KEY_SIZE*DH_KEYS_COUNT+16+2];
  uint8_t cryp_data[64];
  uint8_t data_Nst[16];

  uint8_t currentAESKEY[32]="verywowpasswordsuchbigverystrong";
  void CryptoGetCurrentAESKey(uint8_t *keybuffer)
  {
  	memcpy(keybuffer,currentAESKEY,32);
  }
  uint8_t K0[32]={0};
  void CryptoGetK0AESKey(uint8_t *keybuffer)
  {
  	memcpy(keybuffer,K0,32);
  }
  uint8_t Knew[32] = {0};
  void CryptoGetKnewAESKey(uint8_t *keybuffer)
  {
  	memcpy(keybuffer,Knew,32);
  }
  uint8_t K0_old[32]={0};
  void CryptoGetK0_oldAESKey(uint8_t *keybuffer)
  {
  	memcpy(keybuffer,K0_old,32);
  }
  uint8_t Kt_old[32]={0};
  void CryptoGetKt_oldAESKey(uint8_t *keybuffer)
  {
  	memcpy(keybuffer,Kt_old,32);
  }
  int CRYPTO_ProcessCommand(ts_cmdStruct *cmd)
  {

  	if (cmd->cmd == CMD_CRYPTO_TASK_CALC_PUBLIC)
  	{
  		makedQ(); // формируем открытый ключ
  		get_random(Nsat, 16); // криптоустойчивый рандомизатор
  	}

  	if (cmd->cmd == CMD_CRYPTO_TASK_CALC_SC_Q)
  	{
  		prj_pt_init(&Q_st,&(curve_params.ec_curve));
  		uint32_t *dhix = (uint32_t *) (DH_stationQNst);
  		uint32_t *dhiy = (uint32_t *) (DH_stationQNst+68);
  		uint32_t *dhiz = (uint32_t *) (DH_stationQNst+136);
  		memcpy(Nst,DH_stationQNst+204,16);

  		for (int i =0;i<17;i++)
  		{
  			Q_st.X.fp_val.val[16-i] = uswap_32(dhix[i]);
  			Q_st.Y.fp_val.val[16-i] = uswap_32(dhiy[i]);
  			Q_st.Z.fp_val.val[16-i] = uswap_32(dhiz[i]);
  		}
  		makeSharedSecred();

  	}

	if (cmd->cmd == CMD_CRYPTO_TASK_SEN_RCI)
	{
		uint8_t message[104] = {0};
		message[3] = 0x01;
		memcpy(message+4,Q_aff_st_rev.x.fp_val.val,68);
		memcpy(message+4+68,Nst,16);
		memcpy(message+4+68+16,Nsat,16);
		hmac_sha256(K0, 32, message, 104, Knew, SHA256_DIGEST_SIZE);

		uint8_t Kmac[32] = {0};
		message[3] = 0x02;
		memcpy(message+4,Q_aff_st_rev.y.fp_val.val,68);
		memcpy(message+4+68,Nsat,16);
		memcpy(message+4+68+16,Nst,16);
		hmac_sha256(K0, 32, message, 104, Kmac, SHA256_DIGEST_SIZE);

		uint8_t message_mac[32] = {0};
		uint8_t hmac[32] = {0};
		memcpy(message_mac,Nst,16);
		memcpy(message_mac+16,Nsat,16);
		hmac_sha256(Kmac, 32, message_mac, 32, hmac, SHA256_DIGEST_SIZE);

		uint8_t downstream[68+68+16+32] = {0};
		uint8_t crcstream[64+64+62] = {0};

		memcpy(downstream,Q_aff_sat_rev.x.fp_val.val,68);
		memcpy(downstream+68,Q_aff_sat_rev.y.fp_val.val,68);
		memcpy(downstream+68*2,Nsat,16);
		memcpy(downstream+68*2+16,hmac,32);

		for (int i =0;i<3;i++)
		{
			rcd.header.wc = 31;
			rcd.header.id= 43;
			rcd.header.addinfo= i;
			if (i==2)
			{
				memcpy(crcstream+64*i, &rcd.header, 2);
				memcpy(crcstream+2+64*i, downstream+i*62, 60);
				uint16_t crc =  crc16_ccitt(crcstream,190);
				memcpy(rcd.b,downstream+i*62,60);
				memcpy(rcd.b+60,&crc,2);
			}
			else
			{
				memcpy(crcstream+64*i, &rcd.header, 2);
				memcpy(crcstream+2+64*i, downstream+i*62, 62);
				memcpy(rcd.b,downstream+i*62,62);
			}
		}
	}

	if (cmd->cmd == CMD_CRYPTO_TASK_CALC_GCM)
		{
			uint8_t downstream[62] = {0};
			volatile uint8_t iv[4] = {0};
			volatile uint8_t data[50] = {0};
			volatile uint8_t tag[16] = {0};
			volatile uint8_t aad [2] = {0};

			unsigned char crypt_buf[50] = {0,};
			memcpy(aad,cryp_data,2);
			memcpy(data,cryp_data+2,50);
			memcpy(tag,cryp_data+50+2,8);
			memcpy(iv,cryp_data+60,4);
			int err = 0;

			unsigned char plain_buf[50] = {0,};

			err = aes_gcm_ad(Knew, 32, iv, 4, data, 50, aad, 2, tag, 8, plain_buf);

			if (err != 0)
			{
				return err;
			}

			if (memcmp(Nsat, plain_buf,16) == 0)
			{
				volatile uint8_t plain_text[50] = {0,};
				memcpy(plain_text,Nst,16);
				rcd.header.wc = 31;
				rcd.header.id= 47;
				rcd.header.addinfo= 0;
				if ((aad[0] & 0x3f) == 8) rcd.header.id= 44;

				memcpy(aad,&rcd.header,2);
				memcpy(iv,downstream+58,4);
				err = aes_gcm_ae(Knew, 32, iv, 4, plain_text, 50, aad, 2, crypt_buf, tag);
				if (err != 0)
				{
					return err;
				}

				memcpy(downstream,crypt_buf,50);
				memcpy(downstream+50,tag,8);
				memcpy(rcd.b,downstream,62);
			}
		}

  	if (cmd->cmd == CMD_CRYPTO_TASK_CHANGE_K0)
  	{
		memcpy(K0_old,K0,32);
		memcpy(K0,Knew,32);
  	}

	if (cmd->cmd == CMD_CRYPTO_TASK_CALC_NEW_KEY)
	{
		get_random(Nsat, 16);
		uint8_t message[36] = {0};
		message[3] = 0x01;
		memcpy(Nst,data_Nst,16);
		memcpy(message+4,Nst,16);
		memcpy(message+4+16,Nsat,16);
		hmac_sha256(K0, 32, message, 36, Knew, SHA256_DIGEST_SIZE);

		uint8_t Kmac[32] = {0};
		message[3] = 0x02;
		memcpy(message+4,Nsat,16);
		memcpy(message+4+16,Nst,16);
		hmac_sha256(K0, 32, message, 36, Kmac, SHA256_DIGEST_SIZE);

		uint8_t message_mac[32] = {0};
		uint8_t hmac[32] = {0};
		memcpy(message_mac,Nst,16);
		memcpy(message_mac+16,Nsat,16);
		hmac_sha256(Kmac, 32, message_mac, 32, hmac, SHA256_DIGEST_SIZE);

		uint8_t downstream[64] = {0};
		memcpy(downstream+2,Nsat,16);
		memcpy(downstream+16+2,hmac,32);

		rcd.header.wc = 24;
		rcd.header.id= 46;
		rcd.header.addinfo= 0;
		memcpy(downstream, &rcd.header, 2);
		uint16_t crc =  crc16_ccitt(downstream, 62);
		memcpy(downstream+62, &crc, 2);
		memcpy(rcd.b,downstream+2,62);

	}

  	if (cmd->cmd == CMD_CRYPTO_TASK_CHANGE_CUR_KEY)
  	{
		memcpy(Kt_old,currentAESKEY,32);
		memcpy(currentAESKEY,Knew,32);
  	}

  	return 0;

  }


  void makeSharedSecred()
  {
  	prj_pt_mul_monty(&Q_st, d_sat, &Q_st);
  	prj_pt_to_aff(&Q_aff_st, &Q_st);

  	aff_pt_init(&Q_aff_st_rev, &(curve_params.ec_curve));
  	for (int i =0;i<17;i++)
  	{
  		Q_aff_st_rev.x.fp_val.val[16-i] = uswap_32(Q_aff_st.x.fp_val.val[i]);
  		Q_aff_st_rev.y.fp_val.val[16-i] = uswap_32(Q_aff_st.y.fp_val.val[i]);
  	}

  }

  void makedQ()
  {
  	u8 curve_name[MAX_CURVE_NAME_LEN] = { 0 };

  	ec_get_curve_name_by_type(ec_maps[5].type, curve_name,
  			sizeof(curve_name));

  	d_sat = &d_satellite;

  	ext_printf("[+] ECDH on curve %s\n", curve_name);

  	the_curve_const_parameters =
  			ec_get_curve_params_by_name(curve_name,
  					(u8)local_strnlen((const char *)
  							curve_name,
  							MAX_CURVE_NAME_LEN)
  							+ 1);
  	import_params(&curve_params, the_curve_const_parameters);

  	//satellite
  	prj_pt_init(&Q_sat, &(curve_params.ec_curve));
  	aff_pt_init(&Q_aff_sat_rev, &(curve_params.ec_curve));
  	nn_init(d_sat, 0);
  	nn_get_random_mod(d_sat, &(curve_params.ec_gen_order));
  	prj_pt_mul_monty(&Q_sat, d_sat, &(curve_params.ec_gen)); //
  	prj_pt_to_aff(&Q_aff_sat, &Q_sat);

  	for (int i =0;i<17;i++)
  	{
  		Q_aff_sat_rev.x.fp_val.val[16-i] = uswap_32(Q_aff_sat.x.fp_val.val[i]);
  		Q_aff_sat_rev.y.fp_val.val[16-i] = uswap_32(Q_aff_sat.y.fp_val.val[i]);
  	}

  }
  int main()
  {
	  uint8_t pytes[64] = {0};
	  SFTY_ProcessCPI(pytes);

	  return 0;
  }
