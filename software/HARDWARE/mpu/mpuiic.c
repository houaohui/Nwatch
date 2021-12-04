#include "mpuiic.h"
#include "delay.h"

 //MPU IIC 延时函数
void MPU_IIC_Delay(void)
{
    delay_us(2);
}

//初始化IIC
void MPU_IIC_Init(void)
{
     GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}
//产生IIC起始信号
void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //sda线输出
	MPU_IIC_SDA_H;
	MPU_IIC_SCL_H;
	MPU_IIC_Delay();
 	MPU_IIC_SDA_L;//START:when CLK is high,DATA change form high to low
	MPU_IIC_Delay();
	MPU_IIC_SCL_L;//钳住I2C总线，准备发送或接收数据
}	  
//产生IIC停止信号
void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();//sda线输出
	MPU_IIC_SCL_L;
	MPU_IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	MPU_IIC_Delay();
	MPU_IIC_SCL_H;
	MPU_IIC_SDA_H;//发送I2C总线结束信号
	MPU_IIC_Delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MPU_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MPU_SDA_IN();      //SDA设置为输入  
	MPU_IIC_SDA_H;
	MPU_IIC_Delay();
	MPU_IIC_SCL_H;
	MPU_IIC_Delay();
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL_L;//时钟输出0
	return 0;  
} 
//产生ACK应答
void MPU_IIC_Ack(void)
{
	MPU_IIC_SCL_L;
	MPU_SDA_OUT();
	MPU_IIC_Delay();
	MPU_IIC_SDA_L;
	MPU_IIC_Delay();
	MPU_IIC_SCL_H;
	MPU_IIC_Delay();
	MPU_IIC_SCL_L;
}
//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
	MPU_IIC_SCL_L;
	MPU_SDA_OUT();
	MPU_IIC_SDA_H;
	MPU_IIC_Delay();
	MPU_IIC_SCL_H;
	MPU_IIC_Delay();
	MPU_IIC_SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MPU_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MPU_SDA_OUT(); 	    
    MPU_IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //MPU_IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7 == 1)
        {
            MPU_IIC_SDA_H;
        }
        else
        {
            MPU_IIC_SDA_L;
        }
            txd<<=1;
		    MPU_IIC_SCL_H;
		    MPU_IIC_Delay(); 
		    MPU_IIC_SCL_L;
		    MPU_IIC_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        MPU_IIC_SCL_L;
        MPU_IIC_Delay();
		MPU_IIC_SCL_H;
        receive<<=1;
        if(MPU_READ_SDA)receive++;   
		MPU_IIC_Delay(); 
    }					 
    if (!ack)
        MPU_IIC_NAck();//发送nACK
    else
        MPU_IIC_Ack(); //发送ACK   
    return receive;
}













