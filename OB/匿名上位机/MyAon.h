/**
 *********************************************************************************************************
 * @file                
 * @Author          chen
 * @Version         V1.0
 * @Date             2021-xx-xx
 * @Libsupports   STM32F4xx_DFP(x.x.x)
 * @Brief              
 *********************************************************************************************************
**/
#ifndef __MYANO_H
#define __MYANO_H



/* Includes Files ---------------------------------------------------------------------------*/
#include "include.h"


/* Global Nacros ----------------------------------------------------------------------------*/
/**宏定义**/
#define USER_DMA_USART	0		//使用DMA发送接收数据（0为不使用DMA）

#define usart huart6			//使用串口一

#define Color_Black  	0		
#define Color_Red  		1
#define Color_Green  	2	//Log打印颜色

/* Global Defines ---------------------------------------------------------------------------*/
/**全局变量声明**/
extern _ano MyAno;		//声明外部变量
extern _Para MPara;		//参数

/* Global Functions prototypes --------------------------------------------------------------*/
/**全局函数声明**/
/****** 用户不可调用函数 ******/
static void SendParaData(uint8_t Id,int32_t para);		//发送ID对应的参数
static void SendCheckAnalysis(uint8_t id,uint8_t *sumcheck,uint8_t *addcheck);		//发送数据校验帧0x00
static uint8_t Receive_CheckData(uint8_t *_da);		//接收数据校验检测
static uint8_t Send_CheckData(_ano *ano);			//发送数据和校验&附加校验计算

/****** 用户可调用函数 ******/
void Ano_Init(void); 												//参数初始化
void Ano_Send_Data(uint8_t id, void *Data, uint8_t lenth);			//发送数据函数
void Ano_Set_Mdata(uint8_t id,void *data,uint8_t len,uint8_t num);	//多数据配置
void Ano_SendMdata(void);											//多数据发送
void Ano_DataAnalysis(uint8_t *_da);								//接收数据解析(放串口回调函数里面)
void Ano_SendString(const char *str,uint8_t color);					//发送字符串
void Ano_SendStringVal(const char *str,int32_t Val);				//发送字符串+数据值

void Show_Test(void);	//测试显示函数

/****** 用户可调用&重写函数 ******/
__weak void ControlOrder(uint8_t _Ord);					//控制指令
__weak void ParaOfReturn_Set(uint16_t _id);				//参数回传设置
__weak void ParaRead_Set(uint16_t _id,int32_t _val);	//参数数据读取设置


#endif
/* File Of End ------------------------------------------------------------------------------*/