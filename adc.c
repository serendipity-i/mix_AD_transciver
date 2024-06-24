 #include "adc.h"
 #include "delay.h"
#include "lcd.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
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
	u16 MAX[10]={100,100,100,100,100,100,100,100,100,100};//��һ����С
	u16 MIN[10]={5000,5000,5000,5000,5000,5000,5000,5000,5000,5000};//��һ�����
	for(i=0;i<100;i++)
	{
		adcx=Get_Adc_Average(ADC_Channel_1,10);
		if(adcx>=(MAX[7]+MAX[8]+MAX[9])/3-50)
		{
			MAX[0]=adcx;
			for(j=0;j<9;j++)
			{
				if(MAX[j]>MAX[j+1]) //ǰ���ҪС
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
				if(MIN[j]<MIN[j+1]) //ǰ���Ҫ��
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
	VAL_JUDGE=((VAL_MAX+VAL_MIN)/2-VAL_MIN)*0.9+VAL_MIN;//��΢ƫ��
	if(VAL_MAX-VAL_MIN<400)//200MV���� ��ֹ���Ķ���һ
	{ VAL_MIN=VAL_MIN-500;//�д���ȶ��ֵ
		VAL_JUDGE=(VAL_MAX+VAL_MIN)/2;
	}

}
u8 GET_CODE(void)  //�ɼ�   ����ֻ�ж�һ���� û���м� ���Ҳ�ʱ�����  һ��Ҫ������ �ӳ�����ر�
{ 	u16 adcx;
	  adcx=Get_Adc_Average(ADC_Channel_1,10);
		LCD_ShowxNum(156,130,adcx,4,16,0);//��ʾADC��ֵ
	if(adcx>VAL_JUDGE)  //��ֵ��Ҫ�޸�
	{	count++;
		return 1;
	}
	else
	{
		count=0;
		return 0;
	}
}

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3
void  Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��


	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼

	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

	ADC_StartCalibration(ADC1);	 //����ADУ׼

	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
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



























