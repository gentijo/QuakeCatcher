/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : L3G4200D.c
* Author             : MSH Application Team
* Author             : Andrea Labombarda
* Version            : $Revision:$
* Date               : $Date:$
* Description        : L3G4200D driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 22/03/2011         |	Initial Revision                |	Andrea Labombarda

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "l3g4200D_driver.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name		: ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*					: I2C or SPI reading functions					
* Input				: Register Address
* Output			: Data REad
* Return			: None
*******************************************************************************/
u8_t ReadReg(u8_t Reg, u8_t* Data) {
  
  //To be completed with either I2c or SPI reading function
  //i.e.: *Data = SPI_Mems_Read_Reg( Reg );
  
  return 1;
}

/*******************************************************************************
* Function Name		: WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*					: I2C or SPI writing function
* Input				: Register Address, Data to be written
* Output			: None
* Return			: None
*******************************************************************************/
u8_t WriteReg(u8_t Reg, u8_t Data) {
    
  //To be completed with either I2c or SPI writing function
  //i.e.: //SPI_Mems_Write_Reg(Reg, Data);
  
  return 1;
}
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SetODR
* Description    : Sets L3G4200D Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetODR(ODR_t ov){
  u8_t value;

  if( !ReadReg(CTRL_REG1, &value) )
    return MEMS_ERROR;

  value &= 0x0f;
  value |= ov<<4;

  if( !WriteReg(CTRL_REG1, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetMode
* Description    : Sets L3G4200D Operating Mode
* Input          : Modality (NORMAL, SLEEP, POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetMode(Mode_t md) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG1, &value) )
    return MEMS_ERROR;
                  
  switch(md) {
  
  case POWER_DOWN:		
    value &= 0xF7;
    value |= (MEMS_RESET<<PD);
    break;
          
  case NORMAL:
    value &= 0xF7;
    value |= (MEMS_SET<<PD);
    break;
          
  case SLEEP:		
    value &= 0xF0;
    value |= ( (MEMS_SET<<PD) | (MEMS_RESET<<ZEN) | (MEMS_RESET<<YEN) | (MEMS_RESET<<XEN) );
    break;
          
  default:
    return MEMS_ERROR;
  }
  
  if( !WriteReg(CTRL_REG1, value) )
    return MEMS_ERROR;
                  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetAxis
* Description    : Enable/Disable L3G4200D Axis
* Input          : X_ENABLE/X_DISABLE | Y_ENABLE/Y_DISABLE | Z_ENABLE/Z_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetAxis(Axis_t axis) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG1, &value) )
    return MEMS_ERROR;
    
  value &= 0xf8;
  value |= axis;
  
  if( !WriteReg(CTRL_REG1, value) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetFullScale
* Description    : Sets the L3G4200D FullScale
* Input          : FS_250/FS_500/FS_2000
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetFullScale(Fullscale_t fs) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;	
  value |= (fs<<FS);
  
  if( !WriteReg(CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetBDU(State_t bdu) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG4, &value) )
    return MEMS_ERROR;
 
  value &= 0x7F;
  value |= (bdu<<BDU);

  if( !WriteReg(CTRL_REG4, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetBLE(Endianess_t ble) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;	
  value |= (ble<<BLE);
  
  if( !WriteReg(CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetSelfTest
* Description    : Set Self Test Modality
* Input          : ST_NORMAL_MODE/ST_0/ST_1
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetSelfTest(SelfTest_t st) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF9;
  value |= (st<<SELF_TEST);
  
  if( !WriteReg(CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : HPFEnable
* Description    : Enable/Disable High Pass Filter
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t HPFEnable(State_t hpf) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG5, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (hpf<<HPEN);
  
  if( !WriteReg(CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : HPM_NORMAL_MODE_RES/HPM_REF_SIGNAL/HPM_NORMAL_MODE/HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetHPFMode(HPFMode_t hpf) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;
  value |= (hpf<<HPM);
  
  if( !WriteReg(CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF_0,HPFCF_1,HPFCF_2... See Table 27 of the datasheet
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetHPFCutOFF(HPFCutOffFreq_t hpf) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= (hpf<<HPFC0);
  
  if( !WriteReg(CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : SetIntPinMode
* Description    : Set Interrupt Pin Modality (push pull or Open drain)
* Input          : PUSH_PULL/OPEN_DRAIN
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetIntPinMode(IntPinMode_t pm) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (pm<<PP_OD);
  
  if( !WriteReg(CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          : I1_ON_PIN_INT1_ENABLE | I1_BOOT_ON_INT1 | INT1_ACTIVE_HIGH
* example        : SetInt1Pin(I1_ON_PIN_INT1_ENABLE | I1_BOOT_ON_INT1_ENABLE | INT1_ACTIVE_LOW) 
* to enable Interrupt 1 or Bootsatus on interrupt 1 pin
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetInt1Pin(Int1PinConf_t pinConf) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0x1F;
  value |= pinConf;
  
  if( !WriteReg(CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : I2_DRDY_ON_INT2_ENABLE/DISABLE | 
                   WTM_ON_INT2_ENABLE/DISABLE | 
                   OVERRUN_ON_INT2_ENABLE/DISABLE | 
                   EMPTY_ON_INT2_ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetInt2Pin(Int2PinConf_t pinConf) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= pinConf;
  
  if( !WriteReg(CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t Int1LatchEnable(State_t latch) {
  u8_t value;
  
  if( !ReadReg(INT1_CFG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;
  value |= latch<<LIR;
  
  if( !WriteReg(INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t ResetInt1Latch(void) {
  u8_t value;
  
  if( !ReadReg(INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetIntConfiguration
* Description    : Interrupt 1 Configuration
* Input          : AND/OR, INT1_LIR ZHIE_ENABLE/DISABLE | INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetIntConfiguration(Int1Conf_t ic) {
  u8_t value;
  
  value = ic;

  if( !WriteReg(INT1_CFG, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetInt1Threshold(IntThsAxis axis, u16_t ths) {
  u8_t value;
  
  switch (axis) {
    
    case THS_X:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !WriteReg(INT1_THS_XL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !WriteReg(INT1_THS_XH, value) )
        return MEMS_ERROR;
      
      break;
      
    case THS_Y:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !WriteReg(INT1_THS_YL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !WriteReg(INT1_THS_YH, value) )
        return MEMS_ERROR;
      
      break;
      
    case THS_Z:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !WriteReg(INT1_THS_ZL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !WriteReg(INT1_THS_ZH, value) )
        return MEMS_ERROR;
      
      break;     

        
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetInt1Duration(Int1Conf_t id) {
 
  if (id > 127)
    return MEMS_ERROR;

  if( !WriteReg(INT1_DURATION, id) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : 
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t FIFOModeEnable(FifoMode_t fm) {
  u8_t value;  
  
  if(fm == FIFO_DISABLE) {
    
    if( !ReadReg(CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;    
    
    if( !WriteReg(CTRL_REG5, value) )
      return MEMS_ERROR;
    
  }
  else {
    
    if( !ReadReg(CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;
    value |= MEMS_SET<<FIFO_EN;
    
    if( !WriteReg(CTRL_REG5, value) )
      return MEMS_ERROR;
    
    
    if( !ReadReg(FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<FM0);
    
    if( !WriteReg(FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetWaterMark(u8_t wtm) {
  u8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !ReadReg(FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xE0;
  value |= wtm; 
  
  if( !WriteReg(FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : GetSatusReg
* Description    : Read the status register
* Input          : None
* Output         : status register buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t GetSatusReg(u8_t* buff) {
  if( !ReadReg(STATUS_REG, buff) )
      return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : GetAngRateRaw
* Description    : Read the Angular Rate Registers
* Input          : None
* Output         : Angular Rate Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t GetAngRateRaw(AngRateRaw_t* buff) {
  u8_t valueL;
  u8_t valueH;
  

  if( !ReadReg(OUT_X_L, &valueL) )
      return MEMS_ERROR;
  
  if( !ReadReg(OUT_X_H, &valueH) )
      return MEMS_ERROR;
  
  buff->x = (i16_t)( (valueH << 8) | valueL );
  
  if( !ReadReg(OUT_Y_L, &valueL) )
      return MEMS_ERROR;
  
  if( !ReadReg(OUT_Y_H, &valueH) )
      return MEMS_ERROR;
  
  buff->y = (i16_t)( (valueH << 8) | valueL );
  
   if( !ReadReg(OUT_Z_L, &valueL) )
      return MEMS_ERROR;
  
  if( !ReadReg(OUT_Z_H, &valueH) )
      return MEMS_ERROR;
  
  buff->z = (i16_t)( (valueH << 8) | valueL );
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t GetInt1Src(u8_t* buff) {
  
  if( !ReadReg(INT1_SRC, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t GetFifoSourceReg(u8_t* buff) {
  
  if( !ReadReg(FIFO_SRC_REG, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : SetOutputDataAndFifoFilters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to output and fifo registers
*                : See Table 8 of AN3393 for more details
* Input          : NONE, HPH, LPF2, HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetOutputDataAndFifoFilters(HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !ReadReg(CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case NONE:
    value &= 0xfc;
    value |= 0x00; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 0
    break;
    
  case HPF:
    value &= 0xfc;
    value |= 0x01; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 1
    break;

  case LPF2:
    value &= 0xed;
    value |= 0x02; //hpen = 0, Out_sel_1 = 1, Out_sel_0 = x
    break;    
   
  case HPFLPF2:
    value &= 0xed;
    value |= 0x12; //hpen = 1, Out_sel_1 = 1, Out_sel_0 = x
    break;    
  }
  
  if( !WriteReg(CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : SetInt1Filters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to Int1 circuitery
*                : See Table 9 of AN3393 for more details
* Input          : NONE, HPH, LPF2, HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetInt1Filters(HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !ReadReg(CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case NONE:
    value &= 0xf3;
    value |= 0x00<<INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 0
    break;
    
  case HPF:
    value &= 0xf3;
    value |= 0x01<<INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 1
    break;

  case LPF2:
    value &= 0xe7;
    value |= 0x02<<INT1_SEL0; //hpen = 0, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
   
  case HPFLPF2:
    value &= 0xe7;
    value |= 0x01<<HPEN;
    value |= 0x02<<INT1_SEL0; //hpen = 1, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
  }
  
  if( !WriteReg(CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : SPI_3_WIRE, SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t SetSPIInterface(SPIMode_t spi) {
  u8_t value;
  
  if( !ReadReg(CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xFE;
  value |= spi<<SIM;
  
  if( !WriteReg(CTRL_REG4, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
}
