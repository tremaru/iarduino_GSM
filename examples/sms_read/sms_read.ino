//  Данный пример демонстрирует получение SMS сообщений:
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
    char     SMStxt[161];                                                                     // Объявляем строку для хранения текста принятых SMS сообщений.
    char     SMSnum[ 13];                                                                     // Объявляем строку для хранения номера отправителя SMS сообщений.
    char     SMStim[ 18];                                                                     // Объявляем строку для хранения даты и времени отправки SMS сообщений.
    uint16_t SMSlongID;                                                                       // Объявляем переменную для хранения идентификатора составного SMS сообщения.
    uint8_t  SMSlongSUM;                                                                      // Объявляем переменную для хранения количества SMS в составном сообщении.
    uint8_t  SMSlongNUM;                                                                      // Объявляем переменную для хранения номера SMS в составном сообщении.
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
//  Установка кодировки для символов Кириллицы:                                               //
    gsm.TXTsendCodingDetect("п");                                                             // Выполняем автоопределение кодировки скетча для отправки текста на Русском языке.
//  gsm.TXTsendCoding(GSM_TXT_CP866);                                                         // Если символы Кириллицы исходящих SMS сообщений некорректно отображаются на телефоне, выберите одну из кодировок: GSM_TXT_CP866 / GSM_TXT_UTF8 / GSM_TXT_WIN1251.
//  gsm.TXTreadCoding(GSM_TXT_CP866);                                                         // Если символы Кириллицы входящих  SMS сообщений некорректно отображаются в мониторе,  выберите одну из кодировок: GSM_TXT_CP866 / GSM_TXT_UTF8 / GSM_TXT_WIN1251.
}                                                                                             // По умолчанию для входящих и исходящих SMS сообщений установлена кодировка GSM_TXT_UTF8.
                                                                                              //
void loop (){                                                                                 //
    if(millis()%1000<100){                                                                    // Выполняем код в теле оператора if первые 100 мс каждой секунды.
        delay(100);                                                                           // Устанавливаем задержку в 100 мс, чтоб не выполнить код более 1 раза за секунду.
        if(gsm.SMSavailable()){                                                               // Функция SMSavailable() возвращает количество входящих непрочитанных SMS сообщений.
//      Если есть входящие непрочитанные SMS сообщения:                                       //
            Serial.println( F("SMS came, I read ...") );                                      // Выводим текст "Пришла SMS, читаю ... ".
            gsm.SMSread(SMStxt, SMSnum, SMStim, SMSlongID, SMSlongSUM, SMSlongNUM);           // Читаем SMS сообщение в ранее объявленные переменные (текст SMS сообщения, адрес отправителя, дата отправки, идентификатор SMS, количество SMS, номер SMS).
            Serial.print  ( F("SMS ")                   ); Serial.print  ( SMSlongNUM );      //
            Serial.print  ( F(" in ")                   ); Serial.print  ( SMSlongSUM );      //
            Serial.print  ( F(", ID=")                  ); Serial.print  ( SMSlongID );       //
            Serial.print  ( F(", sent on ")             ); Serial.print  ( SMStim );          //
            Serial.print  ( F(" from number ")          ); Serial.print  ( SMSnum );          //
            Serial.println( F(", message text:")        ); Serial.println( SMStxt );          //
//          gsm.SMSsend( F("Ваше сообщение получено, спасибо!"), SMSnum);                     // Данная строка будет отвечать отправителю указанным SMS сообщением.
            Serial.println( F("--------------------")   );                                    //
        }                                                                                     //
    }                                                                                         //
}                                                                                             //