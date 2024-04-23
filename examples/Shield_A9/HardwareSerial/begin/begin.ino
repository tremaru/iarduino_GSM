//  Данный пример демонстрирует инициализацию работы GSM/GPRS Shield с проверкой его статуса:
//  ==========================================================================================
    #define pinPWR 7                                                                          // Вывод Arduino используемый для включения GSM/GPRS Shield.
                                                                                              //
    #include <iarduino_GSM.h>                                                                 // Подключаем библиотеку iarduino_GSM для работы с GSM/GPRS Shield.
    iarduino_GSM gsm(pinPWR);                                                                 // Создаём объект gsm для работы с функциями и методами библиотеки iarduino_GSM, указав вывод PWR.
                                                                                              //
    bool f=false;                                                                             // Определяем флаг обнаружения статуса при котором модуль не может работать.
                                                                                              //
void setup(){                                                                                 //
//  Выводим текст в монитор последовательного порта:                                          //
    Serial.begin(9600);                                                                       // Инициируем передачу данных по аппаратной шине UART для вывода результата в монитор последовательного порта.
    Serial.print( F("Initialization, please wait ... ") );                                    // Выводим текст.
//  Инициируем работу GSM/GPRS Shield:                                                        //
    if( !gsm.begin(Serial1) ){ Serial.println("Error!"); while(1); }                          // Инициируем работу GSM/GPRS Shield, указывая объект шины UART (Serial1, или Serial2, или Serial3..., или SoftSerial).
//  Проверяем готовность GSM/GPRS Shield к работе:                                            //
    switch( gsm.status() ){                                                                   // 
      case GSM_OK          :                                                           break; // Модуль готов к работе.
      case GSM_REG_NO      :                                                           break; // Требуется время ...,      на данный момент модем не зарегистрирован в сети оператора связи.
      case GSM_SPEED_ERR   : Serial.println("UART speed setting error.");        f=1;  break; // Модуль не может работать, не удалось согласовать скорость UART.
      case GSM_UNAVAILABLE : Serial.println("Module is not available.");         f=1;  break; // Модуль не может работать, он недоступен и не выполят AT-команды.
      case GSM_UNKNOWN     : Serial.println("Unknown module status.");           f=1;  break; // Модуль не может работать, его статус неизвестен и корректное выполнение AT-команд не гарантируется.
      case GSM_SLEEP       : Serial.println("Module is in sleep mode.");         f=1;  break; // Модуль не может работать, он находится в режиме ограниченной функциональности.
      case GSM_SIM_NO      : Serial.println("No SIM-card.");                     f=1;  break; // Модуль не может работать, нет SIM-карты.
      case GSM_SIM_FAULT   : Serial.println("Defective SIM-card.");              f=1;  break; // Модуль не может работать, SIM-карта неисправна.
      case GSM_SIM_ERR     : Serial.println("SIM-card is not working");          f=1;  break; // Модуль не может работать, SIM-карта не прошла проверку.
      case GSM_REG_FAULT   : Serial.println("Registration rejected");            f=1;  break; // Модуль не может работать, оператор сотовой связи отклонил регистрацию модема в своей сети.
      case GSM_REG_ERR     : Serial.println("Registration status is undefined"); f=1;  break; // Модуль не может работать, статус регистрации в сети оператора не читается.
      case GSM_SIM_PIN     : if( !gsm.pin("0123") )                             {f=1;} break; // Требуется ввод PIN-кода,  если PIN-код не принят, то устанавливаем флаг f.
      case GSM_SIM_PUK     : Serial.println("Enter PUK-code");                   f=1;  break; // Требуется ввод PUK1,      данный статус свидетельствует о некорректном вводе PIN-кода 3 раза подряд, тогда нужно ввести PUK1 и новый PIN-код: gsm.puk("01234567","1234");
      default              : /* неизвестное состояние */                         f=1;  break; // Требуется ввод PIN2, требуется ввод PUK2, ...
    }                                                                                         // 
//  Ждём завершения регистрации модема в сети оператора связи:                                // 
    while(gsm.status()==GSM_REG_NO){Serial.print("."); delay(1000);}                          // Проверяем регистрацию модема раз в секунду.
//  Если модуль не может работать, то не даём скетчу выполняться дальше:                      // 
    if(f){Serial.println( "Stop the program!" );} while(f){;}                                 // Предупреждаем об "остановке" программы и входим цикл while который будет выполняться бесконечно, следовательно, дальнейший код и цикл loop() не запустятся.
//  Если модуль может работать, то информируем об успешной инициализации:                     // 
    Serial.println("Ok.");                                                                    //
    Serial.println( "The program is running!" );                                              //
}                                                                                             //
                                                                                              //
void loop (){                                                                                 //
}                                                                                             //
                                                                                              //
/*  Примечание:
 *  ==========================================================================================
 *  Если Ваша SIM-карта требует ввод PIN-кода, то измените его в строке "0123" аргумента функции gsm.pin() до загрузки скетча!!!
 *  Будьте внимательны, некорректный ввод PIN-кода 3 раза подряд потребует ввод PUK1 и нового PIN-кода.
 *  Будьте внимательны, некорректный ввод PUK1 10 раз подряд потребует замены SIM-карты.
 */