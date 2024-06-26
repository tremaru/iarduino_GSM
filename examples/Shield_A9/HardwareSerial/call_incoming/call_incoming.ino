//  Данный пример демонстрирует получение и ответ на входящий голосовой вызов с выводом информации о завершении вызова:
//  ==========================================================================================
    #define pinPWR 7                                                                          // Вывод Arduino используемый для включения GSM/GPRS Shield.
                                                                                              //
    #include <iarduino_GSM.h>                                                                 // Подключаем библиотеку iarduino_GSM для работы с GSM/GPRS Shield.
    iarduino_GSM gsm(pinPWR);                                                                 // Создаём объект gsm для работы с функциями и методами библиотеки iarduino_GSM, указав вывод PWR.
                                                                                              //
    char num[13];                                                                             // Объявляем строку для получения адреса (номера) звонящего абонента.
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
}                                                                                             //
                                                                                              //
void loop (){                                                                                 //
//  Если есть входящий вызов:                                                                 //  
    if(gsm.CALLavailable(num)){                                                               // Функция CALLavailable() возвращает true если есть входящий дозванивающийся вызов. В качестве аргумента функции можно указать строку в которую будет записан адрес (номер) вызывающего абонента.
//      Выводим оповещение о входящем вызове:                                                 //
        Serial.print  ( "Incoming call from the " );                                          // Входящий вызов от.
        Serial.println( num );                                                                // Адрес (номер) звонящего.
        gsm.CALLup();                                                                         // Отвечаем на вызов. Если нужно сбросить вызов, то обращаемся к функции CALLend().
//      Ждём завершения дозвона входящего вызова:                                             //
        while(gsm.CALLstatus()==GSM_CALL_IN_BEEP){;}                                          // Функция CALLstatus() возвращает статус текущего голосового вызова, значение GSM_CALL_IN_BEEP указывает на наличие входящего дозванивающегося вызова.
//      Проверяем выполнено ли голосовое соединение:                                          //
        if(gsm.CALLstatus()==GSM_CALL_ACTIVE){                                                // Функция CALLstatus() возвращает статус текущего голосового вызова, значение GSM_CALL_ACTIVE указывает на наличие активного голосового соединения.
//          Ждём завершения активного голосового соединения:                                  //
            while(gsm.CALLstatus()==GSM_CALL_ACTIVE){                                         // Цикл выполняется пока установлено активное голосовое соединение ...
//              Можно добавить код который будет выполняться в процессе разговора             //
            }                                                                                 //
//          Разговор завершён:                                                                // Попасть сюда можно только после выхода из цикла while(), значит соединение было завершено.
            Serial.println( "Call ended" );                                                   // Выводим сообщение: "Вызов завершён".
        }else{                                                                                // Иначе, если голосовое соединение не было установлено, значит звонящий абонент сбросил вызов до ответа.
//          Голосовое соединение не установлено:                                              //
            Serial.println( "Call is rejected." );                                            // Вызов отклонён, голосовое соединение не установлено.
        }                                                                                     //
    }                                                                                         //
}                                                                                             //
                                                                                              //
/*  Примечание:
 *  ==========================================================================================
 *  Перед ответом можно проверить номер телефона в строке num, таким образом отвечать только указанным абонентам.
 *  Если номер не требуется узнавать или сверять, то функцию CALLavailable() можно оставить без параметра, а строку num не объявлять.
 *  Проверку статуса соединения можно опустить, как это сделано в примере call_incoming_mini.
 *  В данном примере не реализован разрыв голосового соединения функцией CALLend() во время разговора, см. пример all.
 *  Во время разговора используется громкая связь, для переключения на гарнитуру и обратно воспользуйтесь функцией SOUNDdevice(), см. пример all.
 */