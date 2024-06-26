//  Данный пример демонстрирует отправку AT-команд с получением ответов:
//  ==========================================================================================
    #define pinPWR 7                                                                          // Вывод Arduino используемый для включения  GSM/GPRS Shield A9.
    #define pinRX  8                                                                          // Вывод Arduino RX подключаемый к выводу TX GSM/GPRS Shield A9.
    #define pinTX  9                                                                          // Вывод Arduino TX подключаемый к выводу RX GSM/GPRS Shield A9.
                                                                                              //
    #include <SoftwareSerial.h>                                                               // Подключаем библиотеку SoftwareSerial для программной реализации шины UART, до подключения библиотеки iarduino_GSM.
    SoftwareSerial softSerial(pinRX, pinTX);                                                  // Создаём объект softSerial указав выводы RX и TX Arduino используемые для программной шины UART.
                                                                                              //
    #include <iarduino_GSM.h>                                                                 // Подключаем библиотеку iarduino_GSM для работы с GSM/GPRS Shield.
    iarduino_GSM gsm(pinPWR);                                                                 // Создаём объект gsm для работы с функциями и методами библиотеки iarduino_GSM, указав вывод PWR.
                                                                                              //
void setup(){                                                                                 //
//  Выводим текст в монитор последовательного порта:                                          //
    Serial.begin(9600);                                                                       // Инициируем передачу данных по аппаратной шине UART для вывода результата в монитор последовательного порта.
    Serial.print( F("Initialization, please wait ... ") );                                    // Выводим текст.
                                                                                              //
//  Инициируем GSM/GPRS Shield и проверяем его готовность к работе:                           //
    if( !gsm.begin(softSerial)  ){ Serial.println("Error!"); while(1); }                      // Инициируем работу GSM/GPRS Shield, указывая объект шины UART.
    while( gsm.status()!=GSM_OK ){ Serial.print("."); delay(1000);     }                      // Ждём завершения регистрации модема в сети оператора связи.
    Serial.println(" OK!");                                                                   //
    Serial.println( F("--------------------") );                                              //
                                                                                              //
//  Отправка AT-команд с выводом ответов в монитор последовательного порта:                   //
    Serial.println( "Manufacturer:"         );                                                //
    Serial.println( gsm.runAT("AT+GMI\r\n") );                                                // AT-команда AT+GMI запрашивает информацию о производителе модуля.
    Serial.println( "---------------------" );                                                //
    Serial.println( "Serial number:"        );                                                //
    Serial.println( gsm.runAT("AT+GSN\r\n") );                                                // AT-команда AT+GSN запрашивает информацию о серийном номере модуля.
    Serial.println( "---------------------" );                                                //
    Serial.println( "Version:"              );                                                //
    Serial.println( gsm.runAT("AT+GMR\r\n") );                                                // AT-команда AT+GMR запрашивает информацию о версии прошивки модуля.
    Serial.println( "---------------------" );                                                //
}                                                                                             //
                                                                                              //
void loop (){                                                                                 //
}                                                                                             //
                                                                                              //
/*  Примечание:
 *  ==========================================================================================
 *  АТ-команды должны заканчиваться символом окончания строки: \r
 *  или символами окончания и начала строки: \r\n
 *  В ответах на большинство АТ-команд так же содержатся символы: \r\n
 */