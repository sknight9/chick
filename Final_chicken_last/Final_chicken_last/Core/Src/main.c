/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ssd1306.h"
#include "ee.h"
#include "ee.c"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//PA6 (a5 pin) is 3.3v power supply to joystick
 uint8_t menu_Cursor;
 uint8_t shop_Cursor;


typedef struct saved_Data{
	// uint8_t
	 uint8_t hunger;
	 uint8_t money;
	 uint8_t flower;
	 uint8_t hat;
 }saved_Data;
volatile saved_Data saved_stats;

typedef struct joystick_pos{
	// uint8_t
	 uint16_t x;
	 uint16_t y;
}joystick_pos;
joystick_pos joystick;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


void setChick(int level){ //update the character on screen
	if(level==0)
		ssd1306_DrawBitmap(68, 15, chick_sad_01, 64, 48, White); //set to sad character
	else if(level==1)
		ssd1306_DrawBitmap(68, 15, chick_love_01, 64, 48, White); //set to happy character
	else if(level==2){                                            //draw to main character with current equipment on
		if(saved_stats.flower == 1 && saved_stats.hat == 1)
			ssd1306_DrawBitmap(62, 7, hat_flower_chick, 64, 58, White);
		else if(saved_stats.hat == 1)
			ssd1306_DrawBitmap(68, 7, hat_chick, 64, 58, White);
		else if(saved_stats.flower == 1)
			ssd1306_DrawBitmap(62, 15, flower_chick, 64, 48, White);
		else
			ssd1306_DrawBitmap(68, 15, chick_01, 64,48 , White);
	}

	ssd1306_UpdateScreen();
	return;
}



void setMenuCursor(int x){
	//clear old cursor, put new one
	ssd1306_FillRectangle(2, menu_Cursor*12, 5, menu_Cursor*12+3, Black);
	//1 is move up, 2 is move down;
	if(x==1 && menu_Cursor>1)
	menu_Cursor--;
	else if((x == 2) && (menu_Cursor<4)) //check to move up or down and error check
		menu_Cursor++;  //move cursor down
	ssd1306_FillRectangle(2, menu_Cursor*12, 5, menu_Cursor*12+3, White);
	ssd1306_UpdateScreen();

}

void setShopCursor(int x){
	//clear old cursor, put new one
	ssd1306_FillRectangle(shop_Cursor*35+5, 40,shop_Cursor*35+15,43 , Black);
	//1 is move up, 2 is move down;
	if(x==1 && shop_Cursor>0)
		shop_Cursor--;
	else if(x==2 && shop_Cursor<4)
		shop_Cursor++;

	ssd1306_FillRectangle(shop_Cursor*35+5, 40,shop_Cursor*35+15,43 , White);
	ssd1306_UpdateScreen();

}


void setMenu(){
	char* hun[7];
	uint16_t x,y;	//char* shop={'S','h','o','p','\n'};
	x=10;y=10;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("shop\n", Font_7x10, White);
	y+=12;x=10;

	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("game\n", Font_7x10, White);
	y+=12;x=10;

	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("hunger\n", Font_7x10, White);
	y+=12;x=10;

	ssd1306_SetCursor(x, y);
	sprintf(hun, "%d", saved_stats.hunger);
	hun[3]='%';
	ssd1306_WriteString(hun, Font_7x10, White);

	setMenuCursor(1);

}
void setshop(){
	shop_Cursor=0;
	ssd1306_Fill(Black);
	char* mun[7];
	uint16_t x,y;
	x=10;y=2;
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString("Money\n", Font_6x8, White);
	y+=12;x=10;

	sprintf(mun, "%d", saved_stats.money);
	mun[3]='%';
		//hun[4]='\n';
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString(mun, Font_6x8, White);

	ssd1306_DrawBitmap(40,4 , hat_small, 40, 40, White);
	ssd1306_SetCursor(40, 30);
	ssd1306_WriteString("Hat\n", Font_6x8, White);

	ssd1306_DrawBitmap(75,4 , flower_shop, 40, 40, White);
	ssd1306_SetCursor(75, 30);
	ssd1306_WriteString("FLWR\n", Font_6x8, White);

	ssd1306_SetCursor(2, 30);
	ssd1306_WriteString("menu\n", Font_6x8, White);

	ssd1306_SetCursor(70, 55);

	if(saved_stats.flower==0 ||saved_stats.flower==255)
		ssd1306_WriteString("40$", Font_6x8, White);
	else if(saved_stats.flower==1)
		ssd1306_WriteString("on", Font_6x8, White);
	else if(saved_stats.flower==2)
		ssd1306_WriteString("off", Font_6x8, White);

	ssd1306_SetCursor(45, 55);
	if(saved_stats.hat==0 ||saved_stats.hat==255)
		ssd1306_WriteString("40$", Font_6x8, White);
	else if(saved_stats.hat==1)
		ssd1306_WriteString("on", Font_6x8, White);
	else if(saved_stats.hat==2)
			ssd1306_WriteString("off", Font_6x8, White);

	ssd1306_UpdateScreen();
	setShopCursor(0);
return;
}



void save_stats(){

	//return;

	  FLASH_EraseInitTypeDef s_eraseinit;
	  uint32_t page_error;

	  s_eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
	  s_eraseinit.NbPages     = 1;
	  s_eraseinit.Page        = 126;
	  s_eraseinit.Banks       = 1;




	     HAL_FLASH_Unlock();
	     __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
	     HAL_FLASHEx_Erase(&s_eraseinit, &page_error);

	     //HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,0x803f0000, &saved_stats);

	     ee_write(0, sizeof(saved_stats), &saved_stats);
	      HAL_FLASH_Lock();

}






void check_joystick(){
	  //HAL_ADC_Start(&hadc1);
	//  HAL_ADC_Start(&hadc2);
	//  HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);
	//  joystick.y=HAL_ADC_GetValue(&hadc1);
	//  HAL_ADC_PollForConversion(&hadc2,HAL_MAX_DELAY);
	//  joystick.x=HAL_ADC_GetValue(&hadc2);
	return;
}





/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
    ssd1306_Init();
    ssd1306_Fill(White);
	//ssd1306_DrawBitmap(68, 15, chick_01, 64,48 , Black);
	//uint8_t data[2];
//	data[0]=0xFA;
	//data[1]=0xAF;

	//HAL_SPI_Transmit(&hspi3, data, 2, HAL_MAX_DELAY);
	ssd1306_UpdateScreen();
	HAL_Delay(500);



	ee_read(0,sizeof(saved_stats),&saved_stats);

	saved_stats.hat=0;
	saved_stats.flower=0;
	saved_stats.hunger++;
	saved_stats.money=0;

	//if(saved_stats.money==255)
	//saved_stats.money=0;

	//if(saved_stats.hunger==255)
	//saved_stats.hunger=1;
	HAL_Delay(400);
	save_stats();
	ee_read(0,sizeof(saved_stats),&saved_stats);


	uint8_t game_mode=0;
	uint8_t chick=0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  switch(game_mode){

	  	  case 0: ssd1306_Fill(Black);
	  		  	  setChick(chick);
	  	  	  	  setMenu();
	  	  	  	  if(chick!=2){
	  	  	  		  HAL_Delay(1700);
	  	  	  		  chick=2;
	  	  	  		ssd1306_FillRectangle(62,7,128,64, Black);
	  	  	  		  setChick(chick);
	  	  	  	  }

	  	  	  	HAL_Delay(100);
	  		  while(1){

	  			HAL_Delay(2000);
	  			ssd1306_UpdateScreen();
	  			 /*
	  	  	  	  check_joystick();
	  	  		  if(joystick.y<800){

	  	  			  setMenuCursor(1);
	  	  			HAL_Delay(500);
	  	  		  }
	  	  		  else if(joystick.y>3500){

	  	  		setMenuCursor(2);
	  	  		HAL_Delay(500);
	  	  		}

	  	  		  if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){

	  	  			  if(menu_Cursor==1)
	  	  				  game_mode=1;
	  	  			  else if(menu_Cursor==2)
	  	  				  game_mode=2;
	  	  			  break;
	  	  			HAL_Delay(100);
	  	  		  }

	  	  		  */
	  			HAL_Delay(500);
	  		  }

	  	  break;
	  	  case 1:
	  		//  setShop();
	  		  setshop();
	  		  HAL_Delay(200);
	  		  while(1){

	  	  	  	  check_joystick();
	  	  		  if(joystick.x<800){

	  	  			  setShopCursor(1);
	  	  			HAL_Delay(500);
	  	  		  }
	  	  		  else if(joystick.x>3500){

	  	  		setShopCursor(2);
	  	  		HAL_Delay(500);
	  	  		}
	  	  		  if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){
	  	  			  while(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){
	  	  				  HAL_Delay(10);
	  	  			  }
	  	  			HAL_Delay(30);
	  	  			  if(shop_Cursor==0){
	  	  				game_mode=0;
	  	  				HAL_Delay(10);
	  	  				break;
	  	  			  }

	  	  			  else if(shop_Cursor==1){

	  	  				  if(saved_stats.hat==1){
	  	  					  saved_stats.hat=2; //hat owned, off
	  			  				ssd1306_SetCursor(45, 55);
	  			  				ssd1306_WriteString("off", Font_6x8, White);
	  			  				ssd1306_UpdateScreen();

	  	  				  }
	  	  				else if(saved_stats.hat==2){
	  	  					saved_stats.hat=1; //hat owned, off
	  		  				ssd1306_SetCursor(45, 55);
	  		  				ssd1306_WriteString("on ", Font_6x8, White);
	  		  				ssd1306_UpdateScreen();
	  	  				}
	  	  			  else if(saved_stats.money>39){
	  	  			  				  saved_stats.hat=1;//hat owned, and on
	  	  			  				  saved_stats.money=saved_stats.money-40;
	  	  			  				  chick=1;
	  	  			  				ssd1306_SetCursor(45, 55);
	  	  			  				ssd1306_WriteString("on ", Font_6x8, White);


	  	  			  			char* mun[7];
	  	  			  			ssd1306_SetCursor(10, 14);
	  	  			  			sprintf(mun, "%d", saved_stats.money);
	  	  			  			mun[3]='%';
	  	  			  			ssd1306_WriteString(mun, Font_6x8, White);




	  	  			  				ssd1306_UpdateScreen();
	  	  			  				//save_stats();
	  	  		  				  }

	  	  			  }
	  	  			  else if(shop_Cursor==2){

	  	  				  if(saved_stats.flower==1){
	  	  					  saved_stats.flower=2; //hat owned, off
	  			  				ssd1306_SetCursor(70, 55);
	  			  				ssd1306_WriteString("off", Font_6x8, White);
	  			  				ssd1306_UpdateScreen();

	  	  				  }
	  	  				else if(saved_stats.flower==2){
	  	  					saved_stats.flower=1; //hat owned, off
	  		  				ssd1306_SetCursor(70, 55);
	  		  				ssd1306_WriteString("on ", Font_6x8, White);
	  		  				ssd1306_UpdateScreen();
	  	  				}
	  	  				else if(saved_stats.money>49){
	  		  				  saved_stats.flower=1;//hat owned, and on
	  		  				  saved_stats.money=saved_stats.money-50;
	  		  				ssd1306_SetCursor(70, 55);
	  		  				ssd1306_WriteString("on ", Font_6x8, White);
	  		  				chick=1;//show the love chick for a sec after leaving shop
	    			  			char* mun[7];
	    			  			ssd1306_SetCursor(10, 14);
	    			  			sprintf(mun, "%d", saved_stats.money);
	    			  			mun[3]='%';
	    			  			ssd1306_WriteString(mun, Font_6x8, White);


	  		  				ssd1306_UpdateScreen();
	  		  				//save_stats();
	  	  				  }

	  	  			  }
	  	  		  }
	  		  }



	  	//  break;

	  	  case 2:
	  		  ssd1306_Fill(Black);
	  		  uint8_t snakex[255];
	  		  uint8_t snakey[255];
	  		  uint8_t foodx=12*5;
	  		  uint8_t foody=8*5;
	  		  uint8_t head=1;
	  		  uint8_t randomishx=12;
	  		  uint8_t randomishy=8;
	  		  ssd1306_FillRectangle(foodx,foody,foodx+5,foody+4, White);//food first box
	  		  uint8_t i=0;
	  		  uint8_t tail=0;
	  		  uint8_t snake_bad=0;
	  		  uint8_t snakelen=0;
	  		  snakex[0]=0;snakey[0]=0;
	  		  snakex[1]=0;snakey[1]=0;
	  		  uint8_t direction=0;//0=right, 1=left, 2 = down, 3=up
	  		  ssd1306_UpdateScreen();
	  		  while(1){

	  				 ssd1306_FillRectangle(snakex[tail],snakey[tail],snakex[tail]+5,snakey[tail]+5, Black);
	  				 ssd1306_FillRectangle(snakex[head],snakey[head],snakex[head]+5,snakey[head]+5, White);
	  				 ssd1306_UpdateScreen();

	  				 i=tail;
	  				 if(head!=tail+1)
	  				 while(i!=head){
	  					 if(i==head)
	  						 break;
	  					if(snakex[head]==snakex[i] && snakey[head]==snakey[i] ){
	  						snake_bad=1;
	  						break;
	  					}
	  					i++;
	  				 }
	  				 if (snakex[head]>125 || snakey[head]>60 ||snake_bad){//out of bounds-gameover
	  					 ssd1306_Fill(Black);
	  					 ssd1306_SetCursor(2, 5);
	  					 uint8_t scorei=head-tail-4;
	  					 if(scorei<4){
	  					 ssd1306_WriteString("you can do better\n", Font_7x10, White);
	  					 chick=0;
	  					 }
	  					 else if(scorei<8)
	  					 ssd1306_WriteString("okok good stuff!\n", Font_7x10, White);
	  					 else if(scorei<12){
	  					 ssd1306_WriteString("nice score!\n", Font_7x10, White);
	  					 chick=1;
	  					 }
	  					 else{
	  						 chick=1;
	  					 ssd1306_WriteString("S loves Z!\n", Font_7x10, White);
	  					 }

	  					 ssd1306_SetCursor(2, 20);
	  					 char score[4];
	  					 ssd1306_WriteString("score: ", Font_7x10, White);
	  					 ssd1306_SetCursor(51, 20);
	  					  sprintf(score, "%d", scorei);
	  					  ssd1306_WriteString(score, Font_7x10, White);
	  					  saved_stats.money+=(scorei);
	  					  ssd1306_UpdateScreen();

	  					 while(1){
	  						 HAL_Delay(20);
	  					 if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)){
	  					 game_mode=0;
	  					 HAL_Delay(20);
	  					 while(1){
	  						 if(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
	  							 break;
	  					 }
	  					 break;
	  					 }
	  					 }
	  				 }
	  				 if(game_mode==0)
	  					 break;
	  				 HAL_Delay(230);

	  			 check_joystick();

	  	 		  if(joystick.x<800 && direction!=0){
	  	 			  direction=1;
	  		  		  }
	  		  	else if(joystick.x>3500 &&direction!=1){
	  		  		direction=0;
	  		  	}
	  		  	else if(joystick.y>3500 && direction!=3){
	  		  		direction=2;
	  		  	}
	  		  	else if(joystick.y<800 && direction!=2){
	  		  		direction=3;
	  		  }

	  			 //move snake forward
	  			 if(direction==0){ //right
	  			 snakex[head+1]=snakex[head]+5;
	  			 snakey[head+1]=snakey[head];
	  			 }
	  			 else if(direction==1){//left
	  				 snakex[head+1]=snakex[head]-5;
	  			 snakey[head+1]=snakey[head];
	  			 }
	  			 else if(direction==3){//up
	  				 snakey[head+1]=snakey[head]-5;
	  			 snakex[head+1]=snakex[head];
	  			 }
	  			 else if(direction==2){//down
	  			 snakey[head+1]=snakey[head]+5;
	  			 snakex[head+1]=snakex[head];
	  			 }
	  			 head++;

	  			 if(snakex[head]==foodx && snakey[head]==foody){//on food box,

	  				 foodx=((HAL_GetTick()%23)+1)*5;//semi random way to set new food location
	  				 foody=((HAL_GetTick()>>4)%11)*5;

	  				 ssd1306_FillRectangle(foodx,foody,foodx+5,foody+5, White);//move food box

	  			 }
	  			 else if(head>4)
	  			 tail++;//if on a food don't move tail forward(make snake larger)
	  				 }

	  			 }
	  			 ssd1306_UpdateScreen();


	  		  }





  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA7 PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB7);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
