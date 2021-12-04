#include "yaogan.h"
#include "usart.h"

volatile unsigned short ADCConvertedValue[4]={0};//用来存放ADC转换结果
         
void ADC_DMA_Init(void)
{

    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能时钟

    DMA_DeInit(DMA1_Channel1);    //将通道一寄存器设为默认值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);//该参数用以定义DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;//该参数用以定义DMA内存基地址(转换结果保存的地址)
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//该参数规定了外设是作为数据传输的目的地还是来源，此处是作为来源
    DMA_InitStructure.DMA_BufferSize = 4;//定义指定DMA通道的DMA缓存的大小,单位为数据单位。这里也就是ADCConvertedValue的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设定外设地址寄存器递增与否,此处设为不变 Disable
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//用来设定内存地址寄存器递增与否,此处设为递增，Enable
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度为16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道拥有高优先级 分别4个等级 低、中、高、非常高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//使能DMA通道的内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);//根据DMA_InitStruct中指定的参数初始化DMA的通道

    DMA_Cmd(DMA1_Channel1, ENABLE);//启动DMA通道一
}


void Yaogan_ADC_Init(void)
{	
	ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA,ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
	
	
    /*4个IO口的配置（PA1、PA2、PA3 ,PA6）*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 4;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_239Cycles5);//PA1
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_239Cycles5);//PA2
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,3,ADC_SampleTime_239Cycles5);//PA3
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,4,ADC_SampleTime_239Cycles5);//PA6

	  
    ADC_DMACmd(ADC1, ENABLE);//开启ADC的DMA支持
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

}				  

void Yaogan_Init(void)
{
	ADC_DMA_Init();
	Yaogan_ADC_Init();
	
}

volatile u16 AD_N_Value[4]={0};

//获得ADC值
//ch:通道值 0~3
void Get_ADC_Data(void)   
{
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结
	AD_N_Value[0]=1000*((float)ADCConvertedValue[0]/4096.0);
	AD_N_Value[1]=1000*((float)ADCConvertedValue[1]/4096.0);
	AD_N_Value[2]=1000*((float)ADCConvertedValue[2]/4096.0);
	AD_N_Value[3]=1000*((float)ADCConvertedValue[3]/4096.0);
	
}


unsigned char Yaogan_Get_Left(void)
{
	Get_ADC_Data();
	printf("%d ,%d\r\n",AD_N_Value[1],AD_N_Value[2]);
	if(AD_N_Value[1]>700||AD_N_Value[2]>700)
		return 0;
	else
		return 1;
}

unsigned char Yaogan_Get_Right(void)
{
	Get_ADC_Data();
	if(AD_N_Value[1]<300||AD_N_Value[2]<300)
		return 0;
	else
		return 1;
}


unsigned char Yaogan_scan(void) 
{
	unsigned char out=0;
	Get_ADC_Data();
	
	//printf("%d,%d,%d,%d \r\n",AD_N_Value[0],AD_N_Value[1],AD_N_Value[2],AD_N_Value[3]);
	
	if(AD_N_Value[2]>700) out|=0x01;  //
	if(AD_N_Value[2]<300) out|=0x02;  //
	
	if(AD_N_Value[3]>700) out|=0x04;  //up
	if(AD_N_Value[3]<300) out|=0x08;  //down

	return out;
	
}


