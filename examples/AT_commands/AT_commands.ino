//  Данный пример демонстрирует отправку AT-команд с получением ответов:
//  ==========================================================================================
    #include <iarduino_GSM.h>                                                                 // Подключаем библиотеку iarduino_GSM для работы с GSM/GPRS Shield.
    iarduino_GSM gsm;                                                                         // Создаём объект gsm для работы с функциями и методами библиотеки iarduino_GSM.
                                                                                              //
//  Следующие 2 строки нужны для работы по программной шине UATR (выводы 7 и 8):              //
    #include <SoftwareSerial.h>                                                               // Подключаем библиотеку SoftwareSerial для программной реализации шины UART.
    SoftwareSerial softSerial(7,8);                                                           // Создаём объект softSerial указывая выводы RX и TX платы Arduino.
//  Если Вы работаете c GSM/GPRS Shield по аппаратной шине UATR, удалите 2 предыдущие строки, //
//  а в функции gsm.begin(), вместо softSerial укажите Serial или Serial1, Serial2 ...        // Зависит от номера аппаратной шины UART использующей выводы 0 и 1 Вашей платы Arduino.
                                                                                              //
void setup(){                                                                                 //
//  Выводим текст в монитор последовательного порта:                                          //
    Serial.begin(9600);                                                                       // Инициируем передачу данных по аппаратной шине UART для вывода результата в монитор последовательного порта.
    Serial.print( F("Initialization, please wait ... ") );                                    // Выводим текст.
                                                                                              //
//  Инициируем GSM/GPRS Shield и проверяем его готовность к работе:                           //
    gsm.begin(softSerial);                                                                    // Инициируем работу GSM/GPRS Shield, указывая объект шины UART.
    while(gsm.status()!=GSM_OK){Serial.print("."); delay(1000);} Serial.println(" OK!");      // Ждём завершения регистрации модема в сети оператора связи.
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