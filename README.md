[![](https://iarduino.ru/img/logo.svg)](https://iarduino.ru)[![](https://wiki.iarduino.ru/img/git-shop.svg?3)](https://iarduino.ru) [![](https://wiki.iarduino.ru/img/git-wiki.svg?2)](https://wiki.iarduino.ru) [![](https://wiki.iarduino.ru/img/git-lesson.svg?2)](https://lesson.iarduino.ru)[![](https://wiki.iarduino.ru/img/git-forum.svg?2)](http://forum.trema.ru)

# iarduino\_GSM

**This is a library for Arduino IDE. It allows to work with [GSM/GPRS Shield](https://iarduino.ru/shop/Expansion-payments/gsm-gprs-shield.html) for Arduino made by iArduino.ru**

**Данная библиотека для Arduino IDE позволяет работать с [GSM/GPRS Sield](https://iarduino.ru/shop/Expansion-payments/gsm-gprs-shield.html) для Arduino от [iArduino.ru](https://iarduino.ru)**

> Подробнее про установку библиотеки читайте в нашей [инструкции](https://wiki.iarduino.ru/page/Installing_libraries/).

> Подробнее про подключение к [Arduino UNO](https://iarduino.ru/shop/boards/arduino-uno-r3.html)/[Piranha UNO](https://iarduino.ru/shop/boards/piranha-uno-r3.html) читайте на нашей [wiki](https://wiki.iarduino.ru/page/gsm-gprs-shield/#h3_3)


| Модель | Ссылка на магазин |
|---|---|
| <p>GSM/GPRS Shield</p> <img src="https://wiki.iarduino.ru/img/resources/327/327.svg" width="235px"></img>| https://iarduino.ru/shop/Expansion-payments/gsm-gprs-shield.html |


## Назначение функций:

**Подробное описание работы с библиотекой и примеры смотрите на [нашем сайте](https://wiki.iarduino.ru/page/gsm-gprs-shield/#h3_5)**

**Подключение библиотеки**
```C++
#include <iarduino_gsm.h> // Подключаем библиотеку iarduino_GSM для работы с GSM/GPRS Shield.

iarduino_GSM ОБЪЕКТ; // Создаём объект для работы с функциями и методами библиотеки iarduino_GSM.
```

**Инициализация работы** 

```C++
ОБЪЕКТ.begin( UART ); // Инициализация работы и указание объекта UART.
```

**Получение состояния GSM** 

```C++
ОБЪЕКТ.status(); // Получение состояния GSM.
```

**Ввод PIN-кода**

```C++
ОБЪЕКТ.pin( "PIN" ); // Ввод PIN-кода (CHV1).
```

**Ввод PUK-кода и PIN-кода**

```C++
ОБЪЕКТ.puk( "PUK", "PIN" ); // Ввод PUK-кода (PUK1) и нового PIN-кода (CHV1).
```

**Включение/выключение модуля**

```C++
ОБЪЕКТ.pwr( ФЛАГ ); // Включение/выключение модуля.
```

**Перезагрузка модуля**

```C++
ОБЪЕКТ.reset(); // Перезагрузка модуля.
```

**Выполнение AT-команды**

```C++
ОБЪЕКТ.runAT( "КОМАНДА" [,ТАЙМАУТ [,ФЛАГ]] ); // Выполнение AT-команды.
```

**Выполнение USSD запроса** 

```C++
ОБЪЕКТ.runUSSD( "КОМАНДА" [,ТАЙМАУТ] ); // Выполнение USSD запроса.
```

**Получение уровня сигнала** 

```C++
ОБЪЕКТ.signal(); // Получение уровня принимаемого сигнала.
```

**Получение количества непрочитанных SMS** 

```C++
ОБЪЕКТ.SMSavailable(); // Получение количества принятых непрочитанных SMS сообщений.
```

**Получение объема памяти** 

```C++
ОБЪЕКТ.SMSmax(); // Получение объема памяти SMS на SIM карте.
```

**Чтение SMS**

```C++
ОБЪЕКТ.SMSread( ТЕКСТ [,АДРЕС [,ДАТА [,ID,КОЛИЧЕСТВО,НОМЕР]]] ); // Чтение SMS.
```

**Отправка SMS**

```C++
ОБЪЕКТ.SMSsend( "ТЕКСТ" ,"АДРЕС" [,ID,КОЛИЧЕСТВО,НОМЕР] ); // Отправка SMS.
```

**Указание класса SMS** 

```C++
ОБЪЕКТ.SMSsendClass( КЛАСС ); // Указание класса отправляемым SMS сообщениям.
```

**Указание кодировки получаемого текста** 

```C++
ОБЪЕКТ.TXTreadCoding( КОДИРОВКА ); // Указание кодировки для получаемого (выводимого) текста.
```

**Указание кодировки отправляемого текста** 

```C++
ОБЪЕКТ.TXTsendCoding( КОДИРОВКА ); // Указание кодировки отправляемого (вводимого) текста.
```

**Автоопределение кодировки отправляемого текста** 

```C++
ОБЪЕКТ.TXTsendCodingDetect( 'п' ); // Автоопределение кодировки отправляемого текста.
```

**Проверка наличия вызова** 

```C++
ОБЪЕКТ.CALLavailable( [ АДРЕС ] ); // Проверка наличия входящего (звонящего) голосового вызова.
```

**Принять вызов** 

```C++
ОБЪЕКТ.CALLup(); // Ответ на входящий голосовой вызов (поднятие трубки).
```

**Завершить вызов** 

```C++
ОБЪЕКТ.CALLend(); // Завершение голосовых вызовов (опускание трубки).
```

**Инициализация исходящего вызова** 

```C++
ОБЪЕКТ.CALLdial( "АДРЕС" ); // Инициализация исходящего голосового вызова (набор номера).
```

**Получение состояния вызова** 

```C++
ОБЪЕКТ.CALLstatus(); // Получение состояния голосового вызова.
```

**Выбор устройства ввода/вывода звука**

```C++
ОБЪЕКТ.SOUNDdevice( [ УСТРОЙСТВО ] ); // Выбор устройства ввода/вывода звука.
```

**Указание громкости** 

```C++
ОБЪЕКТ.SOUNDvolume( [ ГРОМКОСТЬ ] ); // Указание громкости звука.
```

**Включение немого режима** 

```C++
ОБЪЕКТ.SOUNDmute( [ ФЛАГ ] ); // Включение немого режима (режим без микрофона).
```

