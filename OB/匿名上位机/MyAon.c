/**
 *********************************************************************************************************
 * @file
 * @Author       chen
 * @Version      V1.0
 * @Date         2021-xx-xx
 * @Libsupports  STM32F4xx_DFP(x.x.x)
 * @Brief
 *********************************************************************************************************
**/
/* Global Defines ---------------------------------------------------------------------------*/
/* Includes Files ---------------------------------------------------------------------------*/
/**头文件**/
#include "MyAon.h"

/* Defines ----------------------------------------------------------------------------------*/
/**全局变量定义**/
_ano MyAno = {0};			//发送的数据
_Para MPara = {0};			//参数

/* Local Defines ---------------------------------------------------------------------------*/
/**局部变量声明**/


/* Local Functions Prototypes ---------------------------------------------------------------*/
/**内部函数声明**/


/* Global Functions -------------------------------------------------------------------------*/
/**全局函数定义**/
 /*
 *******************************************************************************
 * @brief   
 * @param   
 * @retval  
 *******************************************************************************
 */
/** 	
 *	函数名:	参数初始化
 *	形	参:	无
 *	返回值:	无
**/
void Ano_Init(void) 
{
	MyAno.Head = 0xAA;
	MyAno.Addr = 0xFF;
	MyAno.Lenth = 0;
}

/**	
 * 函数名:	发送ID对应的参数
 * 形	参:	Id - 参数ID
 * 			para - 参数数据
 *	返回值:	无
**/
static void SendParaData(uint8_t Id,int32_t para)
{
	Ano_Set_Mdata(0xE2,(uint16_t *)&Id,2,1);	//加载ID
	Ano_Set_Mdata(0xE2,(int32_t *)&para,4,2);	//加载参数数据
	Ano_SendMdata();	//发送数据
}

/**
 * 函数名:	发送数据校验帧0x00
 * 形	参:	id - 帧的ID	
 * 			sumcheck 和校验数据
 * 			addcheck 附加校验数据
 * 返回值：无
**/
static void SendCheckAnalysis(uint8_t id,uint8_t *sumcheck,uint8_t *addcheck)
{
	Ano_Set_Mdata(0x00,(uint8_t *)&id,1,1);
	Ano_Set_Mdata(0x00,(uint8_t *)sumcheck,1,2);
	Ano_Set_Mdata(0x00,(uint8_t *)addcheck,1,3);
	Ano_SendMdata();
}

/**	
 *	函数名:	接收数据校验检测
 *	形	参:	_da数据
 *	返回值:	1校验成功	0校验失败 
**/
static uint8_t Receive_CheckData(uint8_t *_da)
{
	uint8_t i = 0;
	uint8_t sumcheck = 0,addcheck = 0;
	for(i = 0;i < _da[3] + 4;i++)
	{
		sumcheck += _da[i];
		addcheck += sumcheck;
	}	
	if((sumcheck == _da[_da[3] + 4]) && (addcheck == _da[_da[3] + 5]))		//校验通过
		return 1;
	else
		return 0;
}

/**	
 *	函数名:	发送数据和校验&附加校验计算
 *	形	参:	ano结构体
 *	返回值:	1->校验成功 0->校验失败
**/
static uint8_t Send_CheckData(_ano *ano)	
{
	uint8_t i = 0;
	uint8_t sumcheck = 0,addcheck = 0;
	for(i = 0;i < ano->Lenth + 4;i++)
	{
		sumcheck += ano->SendBuff[i];
		addcheck += sumcheck;
	}	
	memcpy(ano->SendBuff + 4 + ano->Lenth,(uint8_t*)&sumcheck,sizeof(sumcheck));
	memcpy(ano->SendBuff + 5 + ano->Lenth,(uint8_t*)&addcheck,sizeof(addcheck));
	/* 其中 ano->SendBuff[3] 表示数据长度 */
	if(sumcheck == ano->SendBuff[ano->SendBuff[3] + 4] && addcheck == ano->SendBuff[ano->SendBuff[3] + 5])
		return 1;
	else
		return 0;
}

/**
 * 函数名：	控制指令
 * 形 参：	_Ord命令
 * 返回值：	无
**/
__weak void ControlOrder(uint8_t _Ord)
{
	switch (_Ord)	//命令
	{
		case Stop:			//程序停止
			Ano_SendString("程序停止运行!\r\n",Color_Red);
		break;

		case Operation:		//运行程序
			Ano_SendString("程序开始运行!\r\n",Color_Green);
		break;

		case Store:			//储存参数
			Ano_SendString("参数储存成功!\r\n",Color_Green);
		break;
	}
}

/**
 * 函数名：	参数回传设置
 * 形 参：	_id参数的ID
 * 返回值：	无
**/
__weak void ParaOfReturn_Set(uint16_t _id)
{
	switch (_id)
	{
		case 1:		SendParaData((uint8_t )_id,(int32_t)HWTYPE);	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_1_P:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par1_P);	break;	
		case PID_1_I:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par1_I);	break;	
		case PID_1_D:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par1_D);	break;	//参数回传（第一组PID）

		case PID_2_P:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par2_P);	break;	
		case PID_2_I:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par2_I);	break;	
		case PID_2_D:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par2_D);	break;	//参数回传（第二组PID）

		case PID_3_P:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par3_P);	break;	
		case PID_3_I:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par3_I);	break;	
		case PID_3_D:	SendParaData((uint8_t )_id,(int32_t)MPara.PID_Par3_D);	break;	//参数回传（第三组PID）

		default: SendParaData((uint8_t )_id,(int32_t)0x00);	break;	//若为其他参数则返回0	
	}
}

/**
 * 函数名：	参数数据读取设置
 * 形 参：	_id参数的ID
 * 			_val参数数据
 * 返回值：	无
**/
__weak void ParaRead_Set(uint16_t _id,int32_t _val)
{
	switch (_id)
	{
		case PID_1_P:	MPara.PID_Par1_P = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_1_I:	MPara.PID_Par1_I = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_1_D:	MPara.PID_Par1_D = _val;	break;	//参数回传硬件版本（此处借用拓空者）

		case PID_2_P:	MPara.PID_Par2_P = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_2_I:	MPara.PID_Par2_I = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_2_D:	MPara.PID_Par2_D = _val;	break;	//参数回传硬件版本（此处借用拓空者）

		case PID_3_P:	MPara.PID_Par3_P = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_3_I:	MPara.PID_Par3_I = _val;	break;	//参数回传硬件版本（此处借用拓空者）
		case PID_3_D:	MPara.PID_Par3_D = _val;	break;	//参数回传硬件版本（此处借用拓空者）
	}
}

/**	
 *	函数名:	接收数据解析
 *	形	参:	_da数据(需为完整的一帧数据)
 *	返回值:	无
**/
void Ano_DataAnalysis(uint8_t *_da)
{
	uint16_t HeadID = 0;
	int32_t DataVal = 0;
	MPara.OrderState = 1;	//在接收命令

	if(_da[0] == 0xAA && (_da[1] == 0xFF || _da[1] == 0x05))	//判断帧头、和目标地址
	{
		if(_da[2] == 0xE0)	//判断功能码（命令帧）
		{
			if(_da[4] == 0x10 && _da[5] == 0x00)		//判断CID和CMD0
				ControlOrder(_da[6]);	//控制指令（通过CMD1来控制程序）
			SendCheckAnalysis((uint8_t )0xE0,(uint8_t *)&(_da[_da[3] + 4]),(uint8_t *)&(_da[_da[3] + 5]));	//回传数据校验帧
			MPara.OrderState = 0;	//命令回传结束
		}
		else if(_da[2] == 0xE1)	//判断功能码（读参数）
		{
			if(Receive_CheckData(_da))
			{
				HeadID = (uint8_t)(_da[4] + (_da[5] << 8));
				ParaOfReturn_Set(HeadID);	//参数回传
				MPara.OrderState = 0;	//命令回传结束
			}
		}
		else if(_da[2] == 0xE2)	//判断功能码（写参数）
		{
			if(Receive_CheckData(_da))
			{
				HeadID = (uint8_t)(_da[4] + (_da[5] << 8));		//(*(uint16_t*)(&_da[4]))
				DataVal = *(int32_t*)(&_da[6]);
				ParaRead_Set(HeadID,DataVal);	//参数数据读取设置
				SendCheckAnalysis((uint8_t )0xE2,(uint8_t *)&(_da[_da[3] + 4]),(uint8_t *)&(_da[_da[3] + 5]));	//回传数据校验帧
				MPara.OrderState = 0;	//命令回传结束
			}
		}
	}	
}

/**	
 *	函数名:	多数据配置函数（结合Ano_SendMdata多数据发送函数使用）
 *	形	参:	id->功能码（0xF1-0xFA） *data->发送的数据 len->数据长度（sizeof） num->用于指示第几个数据（可去除）
 *	返回值:	无
**/
void Ano_Set_Mdata(uint8_t id,void *data,uint8_t len,uint8_t num)
{
	MyAno.ID = id;
	memcpy(MyAno.SendBuff,(uint8_t*)&MyAno,3);
	memcpy(MyAno.SendBuff + 4 + MyAno.Lenth,(uint8_t*)data,len);
	MyAno.Lenth += len;
	memcpy(MyAno.SendBuff + 3,(uint8_t*)&MyAno.Lenth,1);
}

/**	
 *	函数名:	多数据发送（结合Ano_Set_Mdata多数据配置函数使用）
 *	形	参:	id->功能码（0xF1-0xFA） *data->发送的数据 len->数据长度（sizeof） num->用于指示第几个数据（可去除）
 *	返回值:	无
**/
void Ano_SendMdata(void)
{
	uint8_t check;
	check = Send_CheckData(&MyAno);
	if(check)
	{
		#if USER_DMA_USART	//使用DMA形式
			HAL_UART_Transmit_DMA(&usart,(uint8_t*)&MyAno.SendBuff,MyAno.Lenth + 6);	//使用DMA形式
		#else	//未使用DMA形式
			HAL_UART_Transmit(&usart,MyAno.SendBuff,MyAno.Lenth + 6,0xFFFF);		//未使用DMA形式USART
			memset(MyAno.SendBuff,0,sizeof(MyAno.SendBuff));
			MyAno.Lenth = 0;
		#endif
	}
	else
	{
		memset(MyAno.SendBuff,0,sizeof(MyAno.SendBuff));
		MyAno.Lenth = 0;
	}
}

/**	
 *	函数名:	发送数据函数
 * 	形	参:	id->功能码（0xF1-0xFA） *Data->发送的数据 lenth->数据长度（sizeof）
 *	返回值:	无
**/
void Ano_Send_Data(uint8_t id, void *Data, uint8_t lenth)	//发送函数
{
	static uint8_t check;
	MyAno.ID = id;
	MyAno.Lenth = lenth;
	
	uint16_t *data = Data;
	
	if(*data>10000)
	{
		*data=0;
	}

	memcpy(MyAno.SendBuff,(uint8_t*)&MyAno,4);
	memcpy(MyAno.SendBuff + 4,data,lenth);
	
	check = Send_CheckData(&MyAno);
	if(check)	//如果校验成功则发送数据，校验失败就丢弃此包
	{
		#if USER_DMA_USART	//使用DMA形式
			HAL_UART_Transmit_DMA(&usart,MyAno.SendBuff,MyAno.Lenth + 6);	//使用DMA形式
		#else	//未使用DMA形式
			HAL_UART_Transmit(&usart,MyAno.SendBuff,MyAno.Lenth + 6,0xFFFF);	//未使用DMA形式USART
		#endif
	}
}
		
/**
 * 函数名：	发送LOGO信息
 * 形 参：	字符串\数字
 * 返回值：	无
**/
void Ano_SendString(const char *str,uint8_t color)
{
	uint8_t i = 0;
	Ano_Set_Mdata(0xA0,(uint8_t*)&color,1,1);	//加载颜色数据
	while (*(str + i) != '\0')		//判断结束位
	{
		Ano_Set_Mdata(0xA0,(uint8_t*)(str+i++),1,2);	//加载字符串数据
	}
	Ano_SendMdata();	//发送数据
}

/**
 * 函数名：	发送LOGO信息+数据信息
 * 形 参：	字符串\数字
 * 返回值：	无
**/
void Ano_SendStringVal(const char *str,int32_t Val)
{
	uint8_t i = 0;
	Ano_Set_Mdata(0xA0,(int32_t*)&Val,4,1);	//加载数据信息
	while (*(str + i) != '\0')		//判断结束位
	{
		Ano_Set_Mdata(0xA1,(uint8_t*)(str+i++),1,2);	//加载字符串数据
	}
	Ano_SendMdata();	//发送数据
}

int16_t Ano_Show_Sin(float x)	//SIN函数
{
	return 100*sin(2*x);
}

float Ano_Show_Cos(float x)	//COS函数
{
	return 100*cos(2*x);
}

float Ano_Show_SQU(float x)	//方波
{
	if(x > 0 && x < 3.14f)
		return 10;
	else
		return -10;
}
	
void Show_Test(void)	//测试显示
{	
	int16_t y1,y2,y3;
	uint8_t nul = 0;
	static float x = 0;
	x += 0.01f;
	if(x > 3.14f)
		x = -3.14f;
	
	y1 = Ano_Show_Sin(x);		//数据*10发送
	y2 = Ano_Show_Cos(x);		//数据*10发送
	y3 = Ano_Show_SQU(x);
	
	Ano_Set_Mdata(0xF1,(int16_t*)&y1,sizeof(y1),1);
	Ano_Set_Mdata(0xF1,(int16_t*)&y2,sizeof(y2),2);
	Ano_Set_Mdata(0xF1,(int16_t*)&y3,sizeof(y3),3);
	Ano_Set_Mdata(0xF1,(uint8_t*)&nul,sizeof(nul),4);		//加载数据到对应的数据位
	Ano_SendMdata();	//发送数据
}

#if USER_DMA_USART	//使用DMA形式
//开启发送中断，等待发送结束，发送结束后把数组进行清零
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == usart.Instance)	//判断是串口几产生的发送中断
	{
		memset(MyAno.SendBuff,0,sizeof(MyAno.SendBuff));	//清零数组
		MyAno.Lenth = 0;
	}
}
#endif


/* Local Functions --------------------------------------------------------------------------*/
/**内部函数定义**/


/* File Of End ------------------------------------------------------------------------------*/