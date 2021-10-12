基于匿名上位机单V7版本的波形图
单数据发送
.h
```C
typedef struct
{
	uint8_t Head;
	uint8_t Addr;
	uint8_t ID;
	uint8_t Lenth;
	uint8_t SendBuff[1024]; //发送缓存数组
	uint8_t ReceiveBuf[10]; //接收缓存数组
}_ano;

extern _ano MyAno; //声明外部变量

void Ano_Init(void);
void Ano_Send_Data(uint8_t id, void *Data, uint8_t lenth); //发送数据函数

```
.c
```C
static void SendParaData(uint8_t Id,int32_t para); //发送ID对应的参数

_ano MyAno = {0};

/** 
 *  函数名:    参数初始化
 *  形   参:  无
 *  返回值:    无
**/
void Ano_Init(void)
{
	MyAno.Head = 0xAA;
	MyAno.Addr = 0xFF;
	MyAno.Lenth = 0;
}

/**
 *  函数名:    发送数据函数
 *  形   参:  id->功能码（0xF1-0xFA） *Data->发送的数据 lenth->数据长度（sizeof）
 *  返回值:    无
**/
void Ano_Send_Data(uint8_t id, void *Data, uint8_t lenth) //发送函数
{
	static uint8_t check;
	MyAno.ID = id;
	MyAno.Lenth = lenth;
	uint16_t *data = Data;
	
	memcpy(MyAno.SendBuff,(uint8_t*)&MyAno,4);
	memcpy(MyAno.SendBuff + 4,data,lenth);
	check = Send_CheckData(&MyAno);

	if(check) //如果校验成功则发送数据，校验失败就丢弃此包
	{
		#if USER_DMA_USART//使用DMA形式
			HAL_UART_Transmit_DMA(&usart,MyAno.SendBuff,MyAno.Lenth + 6); //使用DMA形式
		#else //未使用DMA形式
			HAL_UART_Transmit(&usart,MyAno.SendBuff,MyAno.Lenth + 6,0xFFFF); //未使用DMA形式USART
		#endif
	}
}

/**
 *  函数名:    发送数据和校验&附加校验计算
 *  形   参:  ano结构体
 *  返回值:    1->校验成功 0->校验失败
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
```
