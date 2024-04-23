//  Данный пример демонстрирует отправку USSD запросов с получением ответов:
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
//  Установка кодировки для символов Кириллицы:                                               //
//  gsm.TXTreadCoding(GSM_TXT_CP866);                                                         // Если символы Кириллицы в ответе на USSD запрос некорректно отображаются в мониторе, выберите одну из кодировок: GSM_TXT_CP866 / GSM_TXT_UTF8 / GSM_TXT_WIN1251.
                                                                                              // По умолчанию установлена кодировка GSM_TXT_UTF8.
//  Отправка USSD запроса *100# с выводом ответа в монитор последовательного порта:           //
    Serial.println(F("Sending USSD request \"*100#\" ..."));                                  // Выводим текст: Отправка USSD запроса "*100#" ...
    Serial.print  (F("Answer: "));                                                            // Выводим текст: Ответ
    Serial.println( gsm.runUSSD("*100#") );                                                   // Отравляем USSD запрос "проверка баланса" возвращая ответ в монитор последовательного порта.
}                                                                                             //
                                                                                              //
void loop (){                                                                                 //
}                                                                                             //