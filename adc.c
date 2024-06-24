 #include "adc.h"
 #include "delay.h"
#include "lcd.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//////////////////////////////////////////////////////////////////////////////////
u16 VAL_MAX=0;
u16 VAL_MIN=0;
u16 VAL_JUDGE=1300;
u16 count=0;
void  GET_Range(void)
{ int i=0;
	int j=0;
	int t=0;
	u16 adcx;
	u16 MAX[10]={100,100,100,100,100,100,100,100,100,100};//第一个最小
	u16 MIN[10]={5000,5000,5000,5000,5000,5000,5000,5000,5000,5000};//第一个最大
	for(i=0;i<100;i++)
	{
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		if(adcx>=(MAX[7]+MAX[8]+MAX[9])/3-50)
		{
			MAX[0]=adcx;
			for(j=0;j<9;j++)
			{
				if(MAX[j]>MAX[j+1]) //前面的要小
					{t=MAX[j];
					 MAX[j]=MAX[j+1];
					 MAX[j+1]=t;
					}
			}
				  //
		}
		if(adcx<=(MIN[7]+MIN[8]+MIN[9])/3+50)
		{ MIN[0]=adcx;
			for(j=0;j<9;j++)
			{
				if(MIN[j]<MIN[j+1]) //前面的要大
					{
						t=MIN[j];
						MIN[j]=MIN[j+1];
						MIN[j+1]=t;
					}
			}

		}
	}
	VAL_MAX=(MAX[3]+MAX[4]+MAX[5]+MAX[6]+MAX[7])/5;
	VAL_MIN=(MIN[3]+MIN[4]+MIN[5]+MIN[6]+MIN[7])/5;
	//VAL_JUDGE=(VAL_MAX+VAL_MIN)/2;
	VAL_JUDGE=((VAL_MAX+VAL_MIN)/2-VAL_MIN)*0.9+VAL_MIN;//略微偏下
	if(VAL_MAX-VAL_MIN<400)//200MV左右 防止检测的都是一
	{ VAL_MIN=VAL_MIN-500;//有待商榷数值
		VAL_JUDGE=(VAL_MAX+VAL_MIN)/2;
	}

}
u8 GET_CODE(void)  //采集   现在只判断一和零 没有中间 正弦波时候清空  一定要特殊标记 延迟五秒关闭
{ 	u16 adcx;
	  adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//显示ADC的值
	if(adcx>VAL_JUDGE)  //阈值需要修改
	{	count++;
		return 1;
	}
	else
	{
		count=0;
		return 0;
	}
}

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3
void  Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟


	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1

	ADC_ResetCalibration(ADC1);	//使能复位校准

	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

	ADC_StartCalibration(ADC1);	 //开启AD校准

	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
}



























