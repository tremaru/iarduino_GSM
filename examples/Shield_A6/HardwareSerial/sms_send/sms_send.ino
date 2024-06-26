//  Данный пример демонстрирует отправку SMS сообщений:
//  ==========================================================================================
    #define pinPWR 9                                                                          // Вывод Arduino используемый для включения GSM/GPRS Shield.
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
    if( !gsm.begin(Serial1)     ){ Serial.println("Error!"); while(1); }                      // Инициируем работу GSM/GPRS Shield, указывая объект шины UART (Serial1, или Serial2, или Serial3..., или SoftSerial).
    while( gsm.status()!=GSM_OK ){ Serial.print("."); delay(1000);     }                      // Ждём завершения регистрации модема в сети оператора связи.
    Serial.println(" OK!");                                                                   //
    Serial.println( F("--------------------") );                                              //
                                                                                              //
//  Установка кодировки для символов Кириллицы:                                               //
    gsm.TXTsendCodingDetect("п");                                                             // Выполняем автоопределение кодировки скетча для отправки текста на Русском языке.
//  gsm.TXTsendCoding(GSM_TXT_CP866);                                                         // Если символы Кириллицы исходящих SMS сообщений некорректно отображаются на телефоне, выберите одну из кодировок: GSM_TXT_CP866 / GSM_TXT_UTF8 / GSM_TXT_WIN1251.
//  gsm.TXTreadCoding(GSM_TXT_CP866);                                                         // Если символы Кириллицы входящих  SMS сообщений некорректно отображаются в мониторе,  выберите одну из кодировок: GSM_TXT_CP866 / GSM_TXT_UTF8 / GSM_TXT_WIN1251.
                                                                                              // По умолчанию для входящих и исходящих SMS сообщений установлена кодировка GSM_TXT_UTF8.
//  Отправка короткого SMS сообщения:                                                         //
    Serial.println(F("Sending normal SMS ..."));                                              //
    gsm.SMSsend(F("Это обычное SMS сообщение"), "70123456789");                               // Отправка обычного (короткого) SMS сообщения на номер +7(012)345-67-89.
                                                                                              //
//  Отправка длинного (составного) SMS сообщения:                                             //
    Serial.println(F("Sending long SMS ..."));                                                // Отправка составного (состоящего из нескольких SMS) сообщения на номер +7(012)345-67-89.
    gsm.SMSsend(F("Данное SMS сообщение является составным, так как содержит более 70"), "70123456789", 0x1234, 3, 1); // синтаксис: gsm.SMSsend("текст", "адрес получателя", идентификатор составного SMS сообщения, количество SMS в составном сообщении, номер SMS в составном сообщении).
    gsm.SMSsend(F(" символов. По этому это сообщение будет отправлено как 3 SMS сообщ"), "70123456789", 0x1234, 3, 2); // синтаксис: gsm.SMSsend("текст", "адрес получателя", идентификатор составного SMS сообщения, количество SMS в составном сообщении, номер SMS в составном сообщении).
    gsm.SMSsend(F("ения.\nНо получатель увидит его как один большой целый текст."     ), "70123456789", 0x1234, 3, 3); // синтаксис: gsm.SMSsend("текст", "адрес получателя", идентификатор составного SMS сообщения, количество SMS в составном сообщении, номер SMS в составном сообщении).
                                                                                              //
//  Отправка короткого SMS сообщения с выводом в монитор порта результата отправки:           //
    Serial.print(F("Sending normal SMS with verification ... "));                             //
    if(gsm.SMSsend(F("Это обычное SMS сообщение"), "70123456789"))                            // Отправка обычного (короткого) SMS сообщения на номер +7(012)345-67-89.
    {Serial.println(F("Sent!"));}else{Serial.println(F("Error!"));}                           //
                                                                                              //
//  Отправка короткого SMS сообщения в условиях плохой связи или недостаточного питания:      //
    Serial.print(F("Sending normal SMS in poor radio conditions ... "));                      //
    for(int i=0; i<20; i++){                                                                  // Цикл попыток отправки SMS сообщения (количество попыток = 20, можно изменить).
        if(gsm.SMSsend(F("Это обычное SMS сообщение"), "70123456789")){i=20;}                 // Пытаемся отправить сообщение и досрочно выходим из цикла если отправка удалась.
        gsm.status();                                                                         // Запрашиваем статус связи после каждой попытки отправки SMS сообщения, это увеличивает шансы на успех следующей попытки отправки.
    }                                                                                         //
                                                                                              //
//  Отправка Flash SMS сообщения:                                                             //
    Serial.println(F("Sending SMS class 0 ..."));                                             //
    gsm.SMSsendClass(GSM_SMS_CLASS_0);                                                        // Указываем присваивать 0 класс всем отправляемым SMS сообщениям (такие SMS сообщения называют Flash-SMS, они сразу отображаются на экране телефона получателя и не сохраняются на большинстве моделей телефонов).
    gsm.SMSsend(F("Это SMS сообщение класса 0"), "70123456789");                              // Отправка SMS сообщения на номер +7(012)345-67-89, которому будет присвоен 0 класс.
    gsm.SMSsendClass(GSM_SMS_CLASS_NO);                                                       // Указываем не присваивать класс отправляемым SMS сообщениям. Сообщения без класса - это обычные SMS сообщения (по умолчанию).
}                                                                                             //
                                                                                              //
void loop (){                                                                                 // В данном скетче сообщения не отправляются в цикле loop, для экономии Ваших средств на счёте.
}                                                                                             //
                                                                                              //
/*  Примечание:
 *  ==========================================================================================
 *  Перед загрузкой данного скетча измените номера телефонов получателей SMS сообщений !!!
 *  ==========================================================================================
 *  Обычное SMS сообщение без символов Кириллицы может содержать до 160 символов текста.
 *  Обычное SMS сообщение содержащие хотя бы 1 символ Кириллицы может содержать до 70 символов текста.
 *  ==========================================================================================
 *  Если количество символов в тексте Вашего сообщения превышает указанные значения, то такое сообщение можно отправить как составное (состоящее из нескольких коротких SMS).
 *  Для таких SMS дополнительно указываются 3 параметра: идентификатор составного SMS сообщения, количество SMS в составном сообщении, номер SMS в составном сообщении.
 *  Идентификатор составного SMS сообщения - целое число от 0 до 65535 (придумываете сами), не позволит телефону склеить SMS от разных составных сообщений.
 *  Количество SMS в составном сообщении - целое число от 2 до 255, указывает телефону сколько SMS ему отправлено (требуется "склеить").
 *  Номер SMS в составном сообщении - число от 1 до количества SMS, указывает телефону в каком порядке требуется склеивать принятые SMS сообщения.
 *  Телефон получателя составного SMS сообщения не уведомит своего хозяина до тех пор, пока все SMS данного сообщения не будут получены и "склеены" в один длинный текст.
 *  Все SMS одного составного сообщения должны иметь одинаковый идентификатор и одинаковое значение количества SMS в составном сообщении.
 *  Идентификаторы разных составных сообщений должны отличаться друг от друга, чтоб телефон получателя не перепутал SMS сообщения разных одновременно принимаемых составных сообщений.
 *  ==========================================================================================
 *  Одно SMS составного сообщения без символов Кириллицы может содержать до 152 символов текста.
 *  Одно SMS составного сообщения содержащие хотя бы 1 символ Кириллицы (в данном SMS сообщении, а не во всем тексте составного сообщения) может содержать до 66 символов текста.
 */