#include <Arduino.h>
#include <OLED_I2C.h>
#include <EEPROM.h> 


#define bt_Up  PIN_PA3 
#define bt_Down PIN_PA6 
#define bt_Left PIN_PA4 
#define bt_Right  PIN_PA5 

#define bt_Ok PIN_PA7 
#define bt_No PIN_PC7 


#define Down_  1<<6   // соответствует порту PA6
#define Up_    1<<3   //3
#define Left_  1<<4   //4
#define Right_ 1<<5   //5


int8_t cursor_x = 0 ;
int8_t cursor_y = 0 ;
int8_t scale    = 2 ;
uint8_t v_arrBuff[126] = {0,1,3};




// Главная страница
#define def_play      0   // позиция курсора для строчки "играть"
#define def_setting   1  
#define def_saves     2  
#define def_loed      3 

// Страница Меню
#define def_back      0  
#define def_options   1   
#define def_hot_key   2   

// Список страниц
#define def_Page_Screen_Start      0 
#define def_Page_Screen_Play       1 
#define def_Page_Screen_Save       2 
#define def_Page_Screen_Setting    3 

// Список элементов правого меню
#define def_rmenu_clear            0 
#define def_rmenu_random           1 
#define def_rmenu_Glider           2 
#define def_rmenu_Brain            3 
#define def_rmenu_Glider_Gan       4 
#define def_rmenu_last_itm         5 



// флаг для включения и отключения обновления карты. 
uint8_t v_StepUp_flag    = 0 ;
// флаг для включения и отключения меню. 
uint8_t v_Menu_flag      = 0 ;
// флаг для включения и отключения основного режима игры
uint8_t v_Play_flag      = 0 ;
// флаг для индикации правого меню.
uint8_t v_right_menu_flag = 0;




// Флаг для стартого экрана с названием игры. 
uint8_t v_StartScrin_flag      = 1 ;

uint8_t test_falg = 1;

uint8_t v_Page             = 0 ; // номер страницы
int8_t  v_selecte_item     = 0 ; // номер строчки 
int8_t  v_selecte_r_menu   = 0 ; // номер строчки 









OLED  myOLED(SDA, SCL, 8);

extern uint8_t SmallFont[];
 






uint16_t v_delay_StepUp = 500;

uint32_t counter_moves =0 ;


uint32_t Timer_before =0 ;
uint32_t Timer_after =0 ;

uint32_t Timer_before_test =0 ;


// флаг для запуска функция при старте игры
uint8_t v_Start_flag = 1 ;






void move_cursor(int x , int y , int scale_f);
uint8_t isPush( char Port_ , uint8_t pin);
void Push_But(uint8_t i);
void draw_Pixl(uint8_t scale_f,uint8_t update_disp);
void arrBuff_write(uint8_t by , uint8_t bi );
void binOut (uint8_t byte_y, uint8_t isEeprom , uint8_t isRam );
void updataPlayScreen( uint8_t eepromUpTrue );
void StepUp (uint16_t delay_StepUp);
void Updata_Start_Screen( uint8_t page );
void f_Check_Select ();
void Save_in_eeprom ();
void Loading_from_eeprom ();
void f_right_menu();                                    // рисуем квадрат правое меню
void f_move_cursor_Rmenu(int8_t change);
void f_execute_Rmenu();                                 // выполнение выбранного элемента правого меню





void setup() 
{


  // Инициализация буферов
  for(int i =0 ; i<126; i++)
  {
   v_arrBuff[i] = 0;
  }






  DDRA=0;
  PORTA=0b11111110;

  DDRC=0;
  PORTC=0xFF;

  DDRB=0b00000000;
  PORTB=0b00000000;

  myOLED.begin();
  myOLED.setFont(SmallFont);





  //  myOLED.fillScr(); digitalWrite(8, HIGH);  digitalWrite(PIN_PB0, HIGH);
  //  myOLED.clrScr();
  //  myOLED.setPixel(10, 10);
  //  myOLED.clrPixel( x,  y);
  //  myOLED.invPixel(10, 10);

  //  myOLED.print("A", 50, 20);  
  //  myOLED.print("Hello", 50, 20); 
  //  myOLED.invertText(bool mode);

  //  myOLED.printNumI(1234, 50, 20);
  //  myOLED.printNumF(12.55, 2, 50, 20);
  
  //  myOLED.drawLine(20, 10, 40, 10);
  //  myOLED.clrLine(int x1, int y1, int x2, int y2);

  //  myOLED.drawRect(20, 10, 40, 50);
  //  myOLED.clrRect(int x1, int y1, int x2, int y2);

  //  myOLED.drawRoundRect(20, 10, 40, 50);
  //  myOLED.clrRoundRect(int x1, int y1, int x2, int y2);

  //  myOLED.drawCircle(30, 30, 20);
  //  myOLED.clrCircle(int x, int y, int radius);

  //  myOLED.invertText(1);
  //  myOLED.print("Hello", 50, 20); 

  //  drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy);
    

    // рисуем пиксили курсора
    myOLED.drawRect( cursor_x, cursor_y, cursor_x+scale+1 , cursor_y+scale+1 ); 

    myOLED.update();


}










// ФУНКЦИЯ _ РИСОВАНИЯ _ КУРСОРА
void move_cursor(int x , int y , int scale_f)
{

  scale_f+=1; // это масштаб с учетом толщины курсора

 // стираем курсор ранее нарисованные
 myOLED.clrRect( cursor_x, cursor_y, cursor_x+scale_f , cursor_y+scale_f ); 
  

  if( (x==0) && (y==0) )
  {
     // рисуем квадрат курсора
  myOLED.drawRect( cursor_x, cursor_y, cursor_x+scale_f , cursor_y+scale_f ); 
 
 // обновляем экран
  myOLED.update();                     
  return;
  }


  // меняем положение курсора
  if (x<0)
  {
    cursor_x = cursor_x + x*scale - 1 ;
  }
  if (x>0)
  {
    cursor_x = cursor_x + x*scale + 1 ;
  }

  if (y<0)
  {
    cursor_y = cursor_y + y*scale - 1 ;
  }
  if (y>0)
  {
    cursor_y = cursor_y + y*scale + 1 ;
  }
      


  

 // проверка выхода за границы экрана
  if (cursor_x>123 )  //  123/3=41    
  { cursor_x = 0;
  } 
  else if (cursor_x<0 )
  { cursor_x = 123 ;
  } 
  
  if (cursor_y>60 )   //  60/3=20
  { cursor_y = 0;
  } 
  else if (cursor_y<0 )
  { cursor_y = 60 ;
  } 






  // рисуем квадрат курсора
  myOLED.drawRect( cursor_x, cursor_y, cursor_x+scale_f , cursor_y+scale_f ); 
 
 // обновляем экран
  myOLED.update();                     

}







// Проверка дребизка контактов 
uint8_t isPush( char Port_ , uint8_t pin)
{
  delayMicroseconds(20);                                                 // ждем 5 микросекунд
  
  if( (Port_ == 'A') &&  ((PINA & pin) != pin) )  { return 1; }         // заново проверяем порт А нажата ли нужная кнопка
  if( (Port_ == 'C') &&  ((PINC & pin) != pin) )  { return 2; }


  return 0 ;
}










void Push_But(uint8_t i)
{

  i= (~i)&0b01111000;    // применяем маску и выделяем нудные кнопки

  switch (i)                                // проверяем какая кнопка была нажата
  {
    case (Down_):
     if(v_right_menu_flag==0)  move_cursor(0,1,scale);   // если правое меню выключено, то двигаем основной курсор.
     else f_move_cursor_Rmenu(1);    // если правое меню включено то двигаем кусор меню.
    break;
    

    case (Up_):      
     if(v_right_menu_flag==0) move_cursor(0,-1,scale);  
     else f_move_cursor_Rmenu(-1);
    break;

    case (Left_):      
     if(v_right_menu_flag==0)  move_cursor(-1,0,scale);
     else ;
    break;

    case (Right_):      
     if(v_right_menu_flag==0) move_cursor(1,0,scale);
     else ;  
    break;

  

    default:
      myOLED.invPixel(126, 62); 
      myOLED.invPixel(126, 63); 
      myOLED.invPixel(127, 63); 
      myOLED.update();
    break;
  }
}









void draw_Pixl(uint8_t scale_f,uint8_t update_disp)
{
  for (int8_t i = 0; i < scale_f ; i++)    
  {
    for (int8_t t = 0; t < scale_f ; t++)
    {
     myOLED.invPixel(cursor_x+t+1, cursor_y+i+1); 
    }
  } 
  if (update_disp)  myOLED.update();
}












void arrBuff_write(uint8_t by , uint8_t bi )
{
 v_arrBuff[by] = v_arrBuff[by] ^ (1<<bi);            // инвертируем бит в массиве
}










// побитовый вывод числа
void binOut (uint8_t byte_y, uint8_t isEeprom , uint8_t isRam )
{

  myOLED.print("          ", 50, 45);
  myOLED.print("          ", 50, 55);

  if (isEeprom==1)
  {
    uint8_t dataEeprom = EEPROM.read(byte_y);
    for (int i =0 ; i<8; i++)
    {      
      if( dataEeprom & (1<<i))  myOLED.printNumI(1, 50+i*6, 45);
      else myOLED.printNumI(0, 50+i*6, 45);
    }
  }


  if (isRam==1)
  {
    uint8_t data = v_arrBuff[ byte_y];
    for (int i =0 ; i<8; i++)
    {      
      if( data & (1<<i))  myOLED.printNumI(1, 50+i*6, 55);
      else myOLED.printNumI(0, 50+i*6, 55);
    }  
    myOLED.update();
  }
}









void updataPlayScreen( uint8_t eepromLoadTrue )
{

  uint8_t cursor_x_save = cursor_x;
  uint8_t cursor_y_save = cursor_y;

  
  // очищаем монитор
  myOLED.clrScr();
  

  uint8_t data_arr = 0;
  // Считываем ЕЕПРОМ 
  for(uint8_t addres=0;  addres<126;  addres++)
  {
    // выбираем от куда читать данные Из буфера или из еепром
   
   
    if ( eepromLoadTrue == 1 )
    { 
      data_arr = EEPROM.read(addres);  
      v_arrBuff[addres] = data_arr;       // востанавливаем буфер из еепром
    }
    else{
      data_arr = v_arrBuff[addres];
    }
    

    //побитовый вывод  
    for(uint8_t t=0;t<8;t++)
    {    
      // Если бит в 1 то рисуем пиксель
      if ( data_arr & (1<<t) )  
      { 
        // выставляем курсор 
       cursor_y = (addres / 42) * 8 * 3 + t * 3  ;         // 3 - это масштаб
       cursor_x = (addres % 42 ) * 3 ;

       // рисуем пиксель
       draw_Pixl(scale, false);    
      }   
    }
    
  }

  // выводим счетчик ходов
  myOLED.print("   ", 0, 50);
  myOLED.printNumI(counter_moves, 110, 0, 3);
  
  // возвращаем курсор на прежнее место.
  if ( eepromLoadTrue == 1 )
  { 
    cursor_x = 0;
    cursor_y = 0;
  }
  else
  {
    cursor_x = cursor_x_save;
    cursor_y = cursor_y_save;
  }

  move_cursor(0,0,scale);

}








void StepUp (uint16_t delay_StepUp)
{
  delayMicroseconds(delay_StepUp);

   //Создаем временный буфер следующего кадра
   uint8_t arrBuff_after[126] = {0,1,3};



    // копируем буфер. 
   for(int8_t i = 0 ; i<126; i++)
   {
     arrBuff_after[i] = v_arrBuff[i];
   }



    int8_t cursor_y_func=0;                         // переменная для хранения координат пикселя вокруг которого будет проверка
    for (int8_t var_f = 0; var_f < 21; var_f++) 
    {
       
      cursor_y_func= var_f;
      int8_t cursor_x_func=0;
      for (int8_t var_r = 0; var_r < 42; var_r++) 
      {
        
        cursor_x_func = var_r;

       // переменная для подсчета живых соседних клеток
       uint8_t pixle_On_func = 0 ;
        // проверяем ячейки вокруг пикселя
        for (int8_t i = -1; i < 2; i++)     // i - это изменение по x
        {  
          for (int8_t t = -1; t < 2; t++)   // t - это изменение по y
          {    
                
            if((i!=0) || (t!=0)){
              
              if( 
                (cursor_x_func + i) >= 0   && 
                (cursor_x_func + i) < 42   && 
                (cursor_y_func + t) >=0    && 
                (cursor_y_func + t) < 21 )
              {                                                                                                                 
                // Получаем байт с записанными пикселями
                uint8_t data_arr   =  v_arrBuff[  ((cursor_y_func+t) / 8) * 42 + cursor_x_func+i ];
                uint8_t bit      =  (cursor_y_func+t) % 8 ;  // получаем номер бита  
                if ( data_arr  &  (1<<bit) )  pixle_On_func++  ;  // проверяем установлен ли нужный бит в байте массива
              }
            }        
          }     
        }

        // прверяем пиксель на условие рождения или смерти
        uint8_t data_arr   =  arrBuff_after[  (cursor_y_func/ 8) * 42 + cursor_x_func ];
        uint8_t bit      =  (cursor_y_func) % 8 ;   
        if ( pixle_On_func==3  )
        {
          // записываем единицу в байт массива следующего кадра по уже расчитанным координатам.
          arrBuff_after[  (cursor_y_func / 8) * 42 + cursor_x_func ] = data_arr | (1 << bit) ;
        }
        else if (pixle_On_func<2 || pixle_On_func>3 )
        {
          arrBuff_after[  (cursor_y_func / 8) * 42 + cursor_x_func ] = data_arr & ~(1 << bit) ;
        }      


      }
    }


    for(int8_t var_i = 0 ; var_i<126; var_i++)
    {
      v_arrBuff[var_i] = arrBuff_after[var_i];
    }
    // это счетсик шагов
    if(counter_moves<1000)
    {
     counter_moves++; 
    }
    else 
    {
      counter_moves=0;
    }

    // Обновляем кадр монитора через буфер arrBuff
    updataPlayScreen(false);
 
}




// Обновление страницы экрана 
void Updata_Start_Screen( uint8_t page )
{

  // экран настроек
  if (page == def_Page_Screen_Setting)
  {

    // Проверяем не вышли ли за границы меню
    if (v_selecte_item>2)
    {
      v_selecte_item=2;
    }
    else if(v_selecte_item<0)
    {
      v_selecte_item=0;
    }


    myOLED.clrScr();
    myOLED.print("back", 10, 5);
    myOLED.print("options", 10, 15);
    myOLED.print("hot_key", 10, 25);
    uint8_t cursor [3] {10,10,4};

    // установка курсора
    for(int v_t=0; v_t<3; v_t++)
    {
      for (int v_i =0; v_i<8; v_i++)
      {
        if (cursor[v_t] & (1<<v_i))  
        { 
          myOLED.setPixel(v_t, v_i+4+(v_selecte_item*10));            
        }
        else myOLED.clrPixel(v_t, v_i+4+(v_selecte_item*10));
      }
    }
  }



  if (page == def_Page_Screen_Save)
  {

  }



  // Главное страница
  if (page == def_Page_Screen_Start)
  {
    
    if (v_selecte_item>3)        {      v_selecte_item=3;    }
    else if(v_selecte_item<0)       {      v_selecte_item=0;    }

    int pos_x =0+20;
    int pos_y =0+30;

    myOLED.clrScr();
    myOLED.print("Game of Life", 30, 5);
    myOLED.print("play",     pos_x     , pos_y     );   
    myOLED.print("setting",  pos_x     , pos_y + 15);
    myOLED.print("saves",    pos_x + 60, pos_y     );
    myOLED.print("load",     pos_x + 60, pos_y + 15);

    myOLED.printNumI(v_selecte_item, 115, 50);
    uint8_t cursor [3] {10,10,4};

    if (v_selecte_item<2)         { pos_x= pos_x -4 ;        pos_y= pos_y +2 + (v_selecte_item*15);      }
    else if (v_selecte_item>=2 )  { pos_x= pos_x - 4 + 60 ;  pos_y= pos_y +2 + ((v_selecte_item-2)*15);  }

    for(int v_t=0; v_t<3; v_t++)
    {
      for (int v_i =0; v_i<8; v_i++)
      {
        if (cursor[v_t] & (1<<v_i))  
        { 
          //myOLED.setPixel(v_t+44, v_i+32+(v_selecte_item*10));
          myOLED.setPixel(v_t+pos_x, v_i+pos_y);            
        }
        else myOLED.clrPixel(v_t+pos_x, v_i + pos_y);
      }
    }
  }



  myOLED.update();

}







void f_Check_Select ()
{

if ( (v_Page == def_Page_Screen_Start) && ( v_selecte_item== def_play) )
{
  v_Page = def_Page_Screen_Play ;   // запоминаем на какой странице находимся
  v_Play_flag=1;                    // открываем раздел для запуска игры
  v_StartScrin_flag=0;              // закрываем раздел меню     
  updataPlayScreen(false);           // стираем экран и загружаем последнюю фигуру из еепром
}


else if ((v_Page == def_Page_Screen_Start) && ( v_selecte_item == def_setting))
{
  v_selecte_item =0;
  v_Page = def_Page_Screen_Setting;
  Updata_Start_Screen( def_Page_Screen_Setting );
}


else if ((v_Page == def_Page_Screen_Start) && ( v_selecte_item == def_saves))
{
    
  Save_in_eeprom () ;

  myOLED.drawRect(111, 28, 125, 38);
  myOLED.print("ok",    112, 30     );
  myOLED.update();
  delay(300);
  
  for(int _i=0;_i<7;_i++){
  myOLED.clrRect(111+_i, 28+_i, 125-_i, 38-_i);  
  }
  myOLED.update();
  
}


else if ((v_Page == def_Page_Screen_Start) && ( v_selecte_item == def_loed))
{
  v_Page = def_Page_Screen_Play ;   // запоминаем на какой странице находимся
  v_Play_flag=1;                    // открываем раздел для запуска игры
  v_StartScrin_flag=0;              // закрываем раздел меню  
  Loading_from_eeprom ();   
  updataPlayScreen(false);           // стираем экран и загружаем последнюю фигуру из еепром

}


else if ((v_Page == def_Page_Screen_Setting) && ( v_selecte_item == def_back))
{
  v_selecte_item =0;
  v_Page = def_Page_Screen_Start;
  Updata_Start_Screen( def_Page_Screen_Start );
}





}






void Save_in_eeprom () 
{
  uint8_t data_func=0;
  // Сохраняем кадр из буфера в Еепром. 


  for(uint8_t addres=0;  addres<126;  addres++)
  {
    data_func = v_arrBuff[addres];
    
    EEPROM.write(addres, data_func  ) ;
    /*
    myOLED.printNumI(addres, 0, 45);
    myOLED.update();    
    */
  }

}








void Loading_from_eeprom () 
{
  for(uint8_t addres=0;  addres<126;  addres++)
  {
    v_arrBuff[addres] = EEPROM.read( addres );
  }
}








// рисуем квадрат правое меню
void f_right_menu()
{
 
  
  uint8_t vf_speed = 6;
  uint8_t vf_size = 69;
  uint8_t vf_last_value = (vf_size/vf_speed)*vf_speed;


  // Закрываем меню
  if (v_right_menu_flag==1)
  {

    for (int8_t _i = vf_last_value ; _i >=0 ; _i-=vf_speed)
    {

      for(int _r=1; _r < vf_speed;  _r++)
      {
        //myOLED.clrLine(int x1, int y1, int x2, int y2);  // -------------------
      }
      
      myOLED.drawLine(127-_i, 0, 127-_i, 63);
      myOLED.drawLine(127-_i, 0, 127, 0);
      myOLED.drawLine(127-_i, 63, 127, 63);
      myOLED.update();
      myOLED.clrRect( 127-_i, 0, 127, 63);

      for(int _r=0; _r <= vf_speed-1;  _r++)
      {
       
       for(uint8_t _t=0;_t<64;_t++)
       {
          myOLED.clrPixel(  (127-_i+_r) ,  _t  );
       }
      }



    }
  
    v_right_menu_flag=0;
    updataPlayScreen(false);
    return;
  }



  // открываем меню
  if (v_right_menu_flag==0){
    for (uint8_t _i = 0; _i <= vf_size; _i+=vf_speed)
    {
        // очистка облости под правым меню во время рисования
      for( uint8_t _t=0;  _t<vf_speed ;  _t++){
        myOLED.clrLine(127-_i+_t, 0+1, 127-_i+_t, 63);
      }

      myOLED.clrLine(127-_i+1, 0+1, 127-_i+1, 63-1);
      myOLED.print("            ", 127-_i+8, 5);
      myOLED.print("clear",        127-_i+8, 5);
      myOLED.print("            ", 127-_i+8, 15);
      myOLED.print("random",       127-_i+8, 15);
      myOLED.print("            ", 127-_i+8, 25);
      myOLED.print("Glider",       127-_i+8, 25);
      myOLED.print("            ", 127-_i+8, 35);
      myOLED.print("Brain",        127-_i+8, 35);
      myOLED.print("            ", 127-_i+8, 45);
      myOLED.print("GliderGan",   127-_i+8, 45);
      myOLED.drawLine(127-_i, 0, 127-_i, 63);
      myOLED.drawLine(127-_i, 0, 127, 0);
      myOLED.drawLine(127-_i, 63, 127, 63);

      // проверяем не наступила ли последняя итерация. тогда рисуем линию у края экрана завершая построение квадрата.
      if (vf_last_value == _i)  myOLED.drawLine(127, 0, 127, 63);
      myOLED.update();
      if(_i < vf_last_value )     myOLED.clrRect( 127-_i, 0, 127, 63); // определяем предпоследний элемент и пропускаем стирание
 
    }
    v_right_menu_flag=1;
    // Ставим курсор на нулевую позицию
    f_move_cursor_Rmenu(0);
    return;
  }


}







void f_move_cursor_Rmenu(int8_t change)
{

  
  uint8_t cursor [3] {10,10,4};
  // позиция курсора
  int pos_x =65;
  int pos_y =7;



  // Стираем предыдущий курсор
  for(int v_t=0; v_t<3; v_t++)
  {
    for (int v_i=0; v_i<8; v_i++)
    {     
      myOLED.clrPixel ( v_t+pos_x, v_i+pos_y+(v_selecte_r_menu*10));
    }
  }


  // Меняем курсор если он не вышел за пределы количества элементов .
  if (   ((v_selecte_r_menu + change)>=0)  &&  ((v_selecte_r_menu + change) < def_rmenu_last_itm )   ) 
  {v_selecte_r_menu += change;
  }


  // установка курсора
  for(int v_t=0; v_t<3; v_t++)
  {
    for (int v_i=0; v_i<8; v_i++)
    {
      if (cursor[v_t] & (1<<v_i))  
      { 
        myOLED.setPixel    ( v_t+pos_x, v_i+pos_y+(v_selecte_r_menu*10) );            
      }
      else myOLED.clrPixel ( v_t+pos_x, v_i+pos_y+(v_selecte_r_menu*10));
    }
  }


  myOLED.update();
}







// Выполняем функции правого меню
void f_execute_Rmenu()
{

  if( v_selecte_r_menu == def_rmenu_clear)
  {
    for(int i =0 ; i<126; i++)
    {
      v_arrBuff[i] = 0;
    }
    v_right_menu_flag=0; 
    counter_moves=0;
    updataPlayScreen(false);
  }

  if( v_selecte_r_menu == def_rmenu_random)
  {
    for(int i =0 ; i<126; i++)
    {
      v_arrBuff[i] = random(254);
    }
    v_right_menu_flag=0; 
    updataPlayScreen(false);
  }

  if( v_selecte_r_menu == def_rmenu_Glider)
  {
    for(int i =0 ; i<126; i++)
    {
      v_arrBuff[i] = 0;
    }          

    // записываем выбранную фигуру
    v_arrBuff[2] = 0b00010000;
    v_arrBuff[3] = 0b00010100;
    v_arrBuff[4] = 0b00011000;

    v_right_menu_flag=0; 
    updataPlayScreen(false);
  }


  if( v_selecte_r_menu == def_rmenu_Brain)
  {          

    for(int i =0 ; i<126; i++)
    {
      v_arrBuff[i] = 0;
    }          

    // записываем выбранную фигуру
    v_arrBuff[2]  = 0b00001000;
    v_arrBuff[3]  = 0b00001000;
    v_arrBuff[4]  = 0b00110000;
    v_arrBuff[5]  = 0b01110000;
    v_arrBuff[6]  = 0b10110000;
    v_arrBuff[7]  = 0b10100000;
    v_arrBuff[8]  = 0b10000000;
    v_arrBuff[9]  = 0b01101000;
    v_arrBuff[10] = 0b01010100;
    v_arrBuff[11] = 0b11010100;
    v_arrBuff[12] = 0b00111000;

    v_arrBuff[45] = 0b00011011;
    v_arrBuff[46] = 0b10010001;
    v_arrBuff[47] = 0b11001010;
    v_arrBuff[48] = 0b10101010;
    v_arrBuff[49] = 0b10101010;
    v_arrBuff[50] = 0b00101010;
    v_arrBuff[51] = 0b11011011;
    v_arrBuff[52] = 0b01000000;
    v_arrBuff[53] = 0b01100000;
    v_arrBuff[54] = 0b10000000;

    v_arrBuff[86] = 0b00000010;
    v_arrBuff[87] = 0b00000010;
    v_arrBuff[88] = 0b00000001;
    v_arrBuff[89] = 0b00000001;
    v_arrBuff[90] = 0b00000001;
    v_arrBuff[91] = 0b00000000;
    v_arrBuff[92] = 0b00000000;
    v_arrBuff[93] = 0b00000010;
    v_arrBuff[94] = 0b00000101;
    v_arrBuff[95] = 0b00000101;
    v_arrBuff[96] = 0b00000011;

    v_right_menu_flag=0; 
    updataPlayScreen(false);
  }

  if( v_selecte_r_menu == def_rmenu_Glider_Gan)
  { 
    for(int i =0 ; i<126; i++)
    {
      v_arrBuff[i] = 0;
    }        

    // записываем выбранную фигуру
    v_arrBuff[1]   = 0b11000000;
    v_arrBuff[2]   = 0b11000000;

    v_arrBuff[12]  = 0b11100000;
    v_arrBuff[13]  = 0b00010000;
    v_arrBuff[14]  = 0b00001000;
    v_arrBuff[15]  = 0b00010000;
    v_arrBuff[16]  = 0b11100000;
    v_arrBuff[17]  = 0b11100000;

    v_arrBuff[22]  = 0b00111000;
    v_arrBuff[23]  = 0b01101100;
    v_arrBuff[24]  = 0b01101100;
    v_arrBuff[25]  = 0b01111100;
    v_arrBuff[26]  = 0b11000110;

    v_arrBuff[31]  = 0b00001100;

    v_arrBuff[35]  = 0b00110000;
    v_arrBuff[36]  = 0b00110000;

    v_arrBuff[55]  = 0b00000001;
    v_arrBuff[56]  = 0b00000010;
    v_arrBuff[57]  = 0b00000001;





    v_right_menu_flag=0; 
    updataPlayScreen(false); 
  }

}







//              ------------------ LOOP -------------------
void loop() {




//    -----------------   МЕНЮ   -------------------
while ( v_StartScrin_flag == 1 )
{


  // В начале обновляем Стартовую страницу
  if (test_falg ==1)
  {
    Updata_Start_Screen( def_Page_Screen_Start);
    test_falg =0;
  }


  //  Правая кнопка
  if (    ((PINA&0b10000000) != 0b10000000) && (millis()-Timer_before >= 250))     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {
    uint8_t v_PIN =0;
    if (  isPush( 'A' , 1<<7 ) )
    { 
      Timer_before = millis();




      
    }
  } 
  



  // ЛЕВАЯ КНОПКА    ___    подвеждение выбора страницы 
  if ( ((PINC & (1<<7) ) != 1<<7) && (millis()-Timer_before >= 250) )     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {  
    if (isPush( 'C' , 1<<7 ) )
    {
      Timer_before = millis();
      f_Check_Select ();
    }
  }





  // перемещение курсора __ Клавиши верх/низ/лево/право
  if (  ((PINA&0b01111000) != 0b01111000) && (millis()-Timer_before >= 250) )     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {
    if ( isPush( 'A' , 0b01111000) )
    {
      Timer_before = millis();
      uint8_t v_PIN =0;  
      v_PIN = PINA;
      v_PIN= (~v_PIN)&0b01111000; 
      switch (v_PIN)                                // проверяем какая кнопка была нажата
      {

      case (Down_):        
        v_selecte_item +=1;        
      break;      

      case (Up_):        
        v_selecte_item -= 1;      
      break;


      case (Left_):  
       if( v_Page == def_Page_Screen_Start )  v_selecte_item -= 2;  
      break;

      case (Right_): 
       if( v_Page == def_Page_Screen_Start )  v_selecte_item += 2; 
      break;

    

      default:
        myOLED.invPixel(126, 62); 
        myOLED.invPixel(126, 63); 
        myOLED.invPixel(127, 63); 
        myOLED.update();
      break;
     }
      // после того как перемещен курсор то необходимо обновит экран
      Updata_Start_Screen(v_Page);
      
     
      
     
    }
  }


}




// ----------------ОСНОВНАЯ ЧАСТЬ ИГРЫ --------------------
while ( v_Play_flag == 1 )
{



  //  Правая кнопка
  if (  ((PINA&0b10000000) != 0b10000000) && (millis()-Timer_before >= 250)  )     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {
    uint8_t v_PIN =0;
    
    // Активируем только если правая кнопка зажата
    while ( isPush( 'A' , 1<<7 ) )
    { 
       Timer_before = millis();
      int8_t switch_flag =0;
      while (isPush( 'A' , 0b01111000)   )
      {         
        if (switch_flag == 0)
        {
          v_PIN = PINA;
          v_PIN= (~v_PIN)&0b01111000;    // применяем маску и выделяем нудные кнопки

          switch (v_PIN)                                // проверяем какая кнопка была нажата
          {

            case (Right_):
          
              f_right_menu();       // функция которая выдвигает правое меню
              switch_flag=1;
              v_StepUp_flag=0;

            break;
            


            case (Left_):               
              v_Play_flag=0;                    // Закрываем раздел для игры
              v_StartScrin_flag=1;              // Открываем раздел для стартового экрана    
              v_Page= def_Page_Screen_Start ;   // сбрасываем номер страницы на страницу стартового экрана
              v_selecte_item=0;                 // сбрасываем в по уполчанию номер выбранного пункта на главном экране
              Updata_Start_Screen(def_Page_Screen_Start);
              switch_flag=1;                    // флаг для того что бы не заходить сюда при зажатой клавиши
              v_right_menu_flag=0;              // сбрасываем флаг правого меню для того что бы при возращении меню в выдвигалось из-за экрана
              v_StepUp_flag=0;                  // останавливаем автоматическое обновление игры если ранее было активировано
              v_selecte_r_menu=0;
            break;


            case ( Down_):              
            if (v_right_menu_flag==0)     // выполняем функции только если правое меню закрыто
            {
              v_StepUp_flag= v_StepUp_flag ^ 1;
              switch_flag=1;          
            }
            break;


            case ( Up_):
            if (v_right_menu_flag==0)
            {                    
              StepUp (v_delay_StepUp);
              switch_flag=1; 
              v_StepUp_flag=0;
            }
            break;
          

            default:
              myOLED.invPixel(126, 62); 
              myOLED.invPixel(126, 63); 
              myOLED.invPixel(127, 63); 
              myOLED.update();            
            break;

          }
        } 
      }
    }
  }









  // ЛЕВАЯ КНОПКА    ___     записываем пиксель в еепром
  if (  ((PINC & (1<<7) ) != 1<<7) && (millis()-Timer_before >= 250) )     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {
  
    if ( isPush( 'C' , 1<<7 ) )
    {
      Timer_before = millis();
      
      if (v_right_menu_flag==1)   // если правое меню включено то...
      {
        // Выполняем функции правого меню
        f_execute_Rmenu();        
      }
      else if (v_right_menu_flag==0)   // если провое меню выключено то...
      { 
        // рисуем пиксель под курсором. 
        draw_Pixl(scale,1);        

        uint8_t byte_y   =  ((cursor_y/3) / 8) * 42 + (cursor_x/3) ;
        uint8_t bite_y   =  ((cursor_y/3) % 8); 

        arrBuff_write(byte_y, bite_y );

      }
        


    }

  }











  // перемещение курсора __ Клавиши верх/низ/лево/право
  if (  ((PINA&0b01111000) != 0b01111000) && (millis()-Timer_before >= 250) )     // проверяем порт А если какая-либо кнопка нажатае то уходим на обработку
  {
    if ( isPush( 'A' , 0b01111000) )
    {
      Timer_before = millis();
      Push_But(PINA);                             // Передаем значение соответствующего нажатия



      uint8_t byte_y   =  ((cursor_y/3) / 8) * 42 + (cursor_x/3) ;
      uint8_t bite_y   =  ((cursor_y/3) % 8);

      uint8_t _x   =  cursor_x/3;
      uint8_t _y   =  cursor_y/3;

      myOLED.update();

      
    }
  }






  // если включен флаг обновления монитора. То монитор обнавляет автоматически
  if (v_StepUp_flag==1) 
  {
    StepUp (v_delay_StepUp);
  }




}



}
