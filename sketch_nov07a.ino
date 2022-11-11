                    //подключение библиотек - начало

#include <SPI.h>          //Стандартная SPI библиотека        
#include <Adafruit_GFX.h> //библиотека, отвечающая за настройку LED-матриц            
#include <Max72xxPanel.h> //библиотека, отвечающая за настройку LED-матриц  
#include <HCSR04.h>       //библиотека, отвечающая за настройку моторов через H-мост
                    //подключение библиотек - конец
                                                
                          //Кол-во LED-матриц по вертикали и горизонтали
int numberOfHorizontalDisplays = 1;           
int numberOfVerticalDisplays = 1;   
                    //Блок описания переменных - начало
int pinCS = 9;            //пин, отвечающий за управление матрицей

                          //Массивы для вывода "смайликов" - начало
 const byte uwu[8] = {                   
  0b00000000,
  0b00000110,
  0b01100110,
  0b00000000,
  0b00000000,
  0b01000010,
  0b00111100,
  0b00000000 };    
 const byte wow[8] = {                    
  0b11100111,
  0b10100101,
  0b10100101,
  0b11100111,
  0b00000000,
  0b00000000,
  0b01111110,
  0b00000000 };  
 const byte sad[8] = {                   
  0b00000000,
  0b00000000,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00111100,
  0b01000010 };    
const byte smile[8] = {                   
  0b00000000,
  0b00100100,
  0b00100100,
  0b00000000,
  0b00000000,
  0b01000010,
  0b00111100,
  0b00000000 };  
const byte who[8] = {                   
  0b00011100,
  0b00100010,
  0b00100000,
  0b00010000,
  0b00001000,
  0b00000000,
  0b00001000,
  0b00001000 };  
                          //Массивы для вывода "смайликов" - конец
                          
 int s = 2;               //Переменная для управления "смайликами" и моторами                           Значения: 0-вывод "who", пробная переменная. 
 int piezoPin=3;          //Пин "пищалки"                                                                         1-вывод "uwu", положение робота при максимально приближенном фонарике.
                          //Пины моторов                                                                          2-вывод "smile", состояние движения робота.
int leftDirPin = 4;       //Пин направления левого мотора                                                         3-вывод "wow", знак препятствия
int leftSpeedPin = 5;     //Пин скорости левого мотора                                                            4-вывод "sad", робот спит при низкой освещенности
int rightDirPin = 7;      //Пин направления правого мотора  
int rightSpeedPin = 6;    //Пин скорости правого мотора     
                          //Пины датчиков света
int lightPin1 = A3;
int lightPin2 = A4;
                    //Блок описания переменных - конец
                    
                          //Подключение матрицы
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
                    
void setup() {
                           // Управляющие пины на стороне Motor Shield сигнал
                           // принимают, но на стороне Arduino они сигнал
                           // выдают, поэтому мы должны сделать их выходами.
    pinMode(leftDirPin, OUTPUT);
    pinMode(leftSpeedPin, OUTPUT);
    pinMode(rightDirPin, OUTPUT);
    pinMode(rightSpeedPin, OUTPUT);
                             // Установим оба канала в одном направлении
    digitalWrite(leftDirPin, HIGH);
    digitalWrite(rightDirPin, HIGH);
                            // Установим оба канала на одинаковую скорость
    analogWrite(leftSpeedPin, 5);
    analogWrite(rightSpeedPin, 5);
                           //Яркость свечения и угол поворота матрицы
    matrix.setIntensity(10);                
    matrix.setRotation(4); 
                          
  Serial.begin(9600);      //Подключение монитора порта
  pinMode(A2, INPUT);      //Подключение датчика линий
}


void loop() 
{ 
                      //Блок датчика линий - начало
                      
                           //Считывание данных с датчика линий
  int table = analogRead(A2);
                           //Преобразование данных
  table = map(table, 0, 1023, 0, 255);
                           //Если датчик не видит стол S=3, если всё хорошо, s=4.
  if(table>238) s=3; else {if  (table<=238) s=4;}
  
                     //Блок датчика линий - конец

                        
                     //Блок дальномера - начало
                        
  int dal = A1;            //Пин дальномера
  int val = analogRead(dal);//Считывание данных с дальномера
                          //Преобразование данных
  val = map(val, 0, 1023, 0, 255);
  
  int a; a=0;             //Переменная, отвечающая за тип препятствия          Значения: 1-стена, 0-стены нет
  if (val>80) {s=3; a=1;} //Если датчик видит препятствие, s=3, a=1.
                          //Если препятствий нет, s=4.
  if ((val<=80)&(table<=238)) s=4; 
                       
                     //Блок дальномера - конец

                       
                     //Блок датчиков света - начало
                     
                          //Считывние показателей
  int light1 = analogRead(lightPin1);
  int light2 = analogRead(lightPin2);
                          //Преобразование данных
  light1 = map(light1, 0, 1023, 0, 255); 
  light2 = map(light2, 0, 1023, 0, 255); 
  if (s!=3)               //Цикл, устанавлиающий значение s
  {                       //При отсутствии освещения s=4
    if                    //При наличии фонарика s=2, при его максимальной близости s=1
          ((light1>13)&(light2>13)) s=4;
    else 
      {if ((light1<5)&(light2<5)) s=1; else s=2;}
  }
                     //Блок датчиков света - конец

                     //Отладочные данные  
Serial.print(table);
Serial.print('\t');
Serial.print(light1);
Serial.print('\t');
Serial.println(light2);


                     //Блок LED-матрицы - начало
                     
matrix.fillScreen(LOW);     //Включение матрицы                      
                            //Цикл, отвечающий за ввод "смайликов"
    for (int y = 0; y < 8; y++ ) 
    {           
        for (int x = 0; x < 8; x++ ) 
        {
                            //Включение "пищалки" при наличии припятствия
          if (s==3) tone(piezoPin, 100, 500);
          switch (s)        //Выобор "смайлика" в зависимости от условий
           { 
            case 1:         //"Смайлик подмигивание", если фонарик близко 
              matrix.drawPixel(x, y, uwu[y] & (1<<x));
              break;
             
            case 2:         //"Смайлик улыбка", если нужно ехать за фонариком
              matrix.drawPixel(x, y, smile[y] & (1<<x));
              break;
            
            case 3:         //"Смайлик испуг", если есть препятствие
              matrix.drawPixel(x, y, wow[y] & (1<<x));
              break;
            
            case 4:         //"Смайлик сон", если фонарика нет
              matrix.drawPixel(x, y, sad[y] & (1<<x));
              break;
           
            default:        //Отладочный массив
              matrix.drawPixel(x, y, who[y] & (1<<x));
              break;
            }
        }
    }
    matrix.write();         //Вывод "смайлика"
    
                     //Блок LED-матрицы - конец    


                    //Блок мотора - начало  
                    
    if (s==3)           //Работа моторов при наличии препятстствия
    { 
                        //Даём команду двигаться назад
        digitalWrite(leftDirPin, HIGH);
        digitalWrite(rightDirPin, HIGH);
        if (a==0)        //Если препятствие - край стола, скорость и длительность больше 
        {        
          analogWrite(leftSpeedPin, 140);
          analogWrite(rightSpeedPin, 140);
          delay (300);
        }
       else              //Если препятствие - стена, скорость и длительность меньше
        {
          analogWrite(leftSpeedPin, 60);
          analogWrite(rightSpeedPin, 60);
          delay (100);
        }
     }
     else               //Работа моторов при отсутствии препятствия                  
        if ((s==4)or(s==1)) 
        {               //Установка нулевой скорости при минимальном и масимальном свете
          analogWrite(leftSpeedPin, 0);
          analogWrite(rightSpeedPin, 0);
        }
        else 
        {
                        //Установка движения вперед
          digitalWrite(leftDirPin, LOW);
          digitalWrite(rightDirPin, LOW);
                        //Проверка направления света
          if ((light1>10)&(light2<10))
          {
                        //Поворот направо, если освещение справа
             digitalWrite(leftDirPin, LOW);
             digitalWrite(rightDirPin, HIGH);
             digitalWrite(leftSpeedPin, 30);
             digitalWrite(rightSpeedPin, 30);
          }
                        //Проверка направления света
          if ((light1<10)&(light2>10))
          {
                        //Поворот налево, если освещение слева        
            digitalWrite(leftDirPin, HIGH);
            digitalWrite(rightDirPin, LOW);
            digitalWrite(leftSpeedPin, 50);
            digitalWrite(rightSpeedPin, 50);
          }
                        //Если освещение спереди, устанавливается движение вперед для 2-х моторов
                        //На значение скорости подается значение освещенности.
          digitalWrite(leftDirPin, LOW);
          digitalWrite(rightDirPin, LOW);
          analogWrite(leftSpeedPin, light2*6);
          analogWrite(rightSpeedPin, light1*6);
        }
        
                  //Блок мотора - конец
                  
delay(10);    
}
