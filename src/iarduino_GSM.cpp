#include "iarduino_GSM.h"																																	//	
#include "SoftwareSerial.h"																																	//	
																																							//	
//		КОНСТРУКТОР КЛАССА:																																	//	
		iarduino_GSM::iarduino_GSM(uint8_t pin){pinPWR=pin;}																								//	Аргументы конструктора:	pin - номер вывода Arduino к которому подключён вывод PWR.
																																							//	
//		ФУНКЦИЯ ИНИЦИАЛИЗАЦИИ МОДУЛЯ:																														//	Функция возвращает:	флаг результата инициализации (true/false).
bool	iarduino_GSM::_begin(void){																															//	Аргументы функции:	отсутствуют.
			flgSpeed = true;																																//	Устанавливаем флаг согласования скорости, иначе функция runAT() не будет работать.
//			Выключаем и включаем модуль:																													//	
			pinMode     (pinPWR, OUTPUT);																													//	Переводим вывод pinPWR в режим выхода.
			digitalWrite(pinPWR, HIGH); delay(2000);																										//	Устанавливаем на выводе pinPWR уровень логической 1 (выключаем модуль).
			digitalWrite(pinPWR, LOW ); delay(2000);																										//	Устанавливаем на выводе pinPWR уровень логического 0 (включаем модуль).
//			Настройка скорости модуля:																														//	
			for(uint8_t i=0; i<5; i++){																														//	Всего 5 попыток
//				Разрываем связь UART:																														//	
				if(flgType)	{(*(SoftwareSerial*)objSerial).end();}																							//	
				else		{(*(HardwareSerial*)objSerial).end();}																							//	
				delay(100);																																	//	
//				Инициируем передачу данных по UART на скорости 115200:																						//	115200 бит/сек - это скорость на которой модуль работает по умолчанию
				if(flgType)	{(*(SoftwareSerial*)objSerial).begin(115200);}																					//	
				else		{(*(HardwareSerial*)objSerial).begin(115200);}																					//	
				delay(100);																																	//	
//				Ждём готовность аппаратного UART после инициализации:																						//	На случай работы с Serial1, Serial2 и т.д.
				if(!flgType){while(!(*(HardwareSerial*)objSerial)){;}}																						//	
//				Отправляем команду модулю, перейти на скорость GSM_UART_SPEED																				//	
				runAT( ((String) "ATZ+IPR=" + GSM_UART_SPEED + "\r\n"), 500, false);																		//	Команда ATZ+IPR=СКОРОСТЬ - указывает модулю перейти на указанную скорость передачи данных по шине UART.
//				Разрываем связь UART:																														//	
				if(flgType)	{(*(SoftwareSerial*)objSerial).end();}																							//	
				else		{(*(HardwareSerial*)objSerial).end();}																							//	
				delay(100);																																	//	
//				Инициируем передачу данных по UART на скорости GSM_UART_SPEED:																				//	При работе с аппаратным UART не на всех платах Arduino удаётся стабильно работать на скорости 115200.
				if(flgType)	{(*(SoftwareSerial*)objSerial).begin(GSM_UART_SPEED);}																			//	По этому приходится переходить на более низкие скорости. Новая скорость указана в константе GSM_UART_SPEED.
				else		{(*(HardwareSerial*)objSerial).begin(GSM_UART_SPEED);}																			//	
				delay(100);																																	//	
//				Ждём готовность аппаратного UART после инициализации:																						//	На случай работы с Serial1, Serial2 и т.д.
				if(!flgType){while(!(*(HardwareSerial*)objSerial)){;}}																						//	
//				Проверяем наличие связи с модулем:																											//	
				for(uint8_t j=0; j<10; j++){ if(runAT(F("AT\r\n")).indexOf(F("\r\nOK\r\n")) > -1){i=5; j=10; flgSpeed=false;} }								//	Если на команду "AT" модуль ответит "\r\nOK\r\n", то принудительно выходим из циклов.
			}	if(!flgSpeed){flgSpeed = true;}else{flgSpeed=false; return false;}																			//	Если флаг flgSpeed сброшен, значит скорость установлена, иначе сбрасываем флаг flgSpeed и выходим из функции.
//			Ждём завершения потока незапрашиваемых кодов от модуля:																							//	
			uint32_t millisEnd;																																//	Объявляем переменную для хранения время выхода из режима ожидания завершения незапрашиваемых кодов от модуля.
			bool flgBuff;																																	//	Объявляем флаг получения незапрашиваемых кодов от модуля.
			do{	flgBuff = false;																															//	Сбрасываем флаг flgBuff.
				millisEnd = millis() + 5000;																												//	Устанавливаем время выхода из режима ожидания завершения незапрашиваемых кодов от модуля.
				while(millis()<millisEnd){																													//	Пока указанное время не достигнуто ...
					if(flgType)	{if((*(SoftwareSerial*)objSerial).available()>0){(*(SoftwareSerial*)objSerial).read(); flgBuff=true;}}						//	Проверяем наличие сомволов в буфере UART, если символ есть, то читаем его в никуда и устанавливаем флаг flgBuff.
					else		{if((*(HardwareSerial*)objSerial).available()>0){(*(HardwareSerial*)objSerial).read(); flgBuff=true;}}						//	Проверяем наличие сомволов в буфере UART, если символ есть, то читаем его в никуда и устанавливаем флаг flgBuff.
					if(flgBuff)	{millisEnd=millis();}																										//	Если флаг flgBuff установлен, значит мы продолжаем получать незапрашиваемые коды, выходим из цикла для обновления времени ожидания.
				}																																			//	
			}	while(flgBuff);																																//	Если флаг flgBuff установлен, повторяем цикл, при новом проходе цикла время ожидания будет обновлено.
//			Проверяем готовность модуля:																													//	
			switch( status() ){																																//	
				case GSM_SIM_NO:		return false; break;																								//	Модуль не может работать, нет сим карты.
				case GSM_SIM_FAULT:		return false; break;																								//	Модуль не может работать, сим карта неисправна.
				case GSM_SIM_ERR:		return false; break;																								//	Модуль не может работать, сим карта не прошла проверку.
				case GSM_REG_FAULT:		return false; break;																								//	Модуль не может работать, оператор сотовой связи отклонил регистрацию модема в своей сети.
				case GSM_UNAVAILABLE:	return false; break;																								//	Модуль не может работать, так как он недоступен и не выполняет AT-команды.
				case GSM_UNKNOWN:		return false; break;																								//	Модуль не может работать, так как его статус неизвестен и не гарантирует корректное выполнение AT-команд.
				case GSM_SLEEP:			return false; break;																								//	Модуль не может работать, так как он находится в режиме ограниченной функциональности.
			}																																				//	
//			Отправляем модулю команды конфигурации:																											//	
			runAT(F("AT+CSCS=\"HEX\"\r\n"));																												//	AT+CSCS="HEX"			- Устанавливаем шестнадцатиричный набор символов.
			runAT(F("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"));																									//	AT+CPMS="SM","SM","SM"	- "SM"-использовать память SIM-карты для просмотра, чтения и удаления сообщений, "SM"-использовать память SIM-карты для написания и отправки сообщений, "SM"-использовать память SIM-карты для получения и сохранения сообщений.
			runAT(F("ATE0\r\n"));																															//	ATE0					- Отключить эхо.
			runAT(F("ATV1\r\n"));																															//	ATV1					- Отвечать на команды текстом, а не которкими цифровыми ответами.
			runAT(F("AT+CMEE=1\r\n"));																														//	AT+CMEE=1				- При возникновении ошибок возвращать ERROR и код ошибки.
			runAT(F("AT+CREG=0\r\n"));																														//	AT+CREG=0				- Отключить незапрашиваемые сообщения о статусе регистрации в сети.
			runAT(F("AT+CMGF=0\r\n"));																														//	AT+CMGF=0				- Устанавливаем режим PDU.
			runAT(F("AT+CNMI=1,0,0,0,0\r\n"));																												//	AT+CNMI=1,0,0,0,0		- Индикация новых сообщений: 1-коды отображаются без буферизации, 0-SMS не отображать, 0-рассылку не отображать, 0-подтверждения о доставке не отображать, 0-работа с буфером на момент написания библиотеки не поддерживается.
			runAT(F("AT+CMGD=1,3\r\n"), 10000);																												//	AT+CMGD=1,3				- Удаляем все прочитанные, отправленные и неотправленные SMS сообщения. На выполнение команды выделяем до 10 секунд.
			runAT(F("AT+CSCS=\"HEX\"\r\n"));																												//	AT+CSCS="HEX"			- Устанавливаем шестнадцатиричный набор символов.
			runAT(F("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"));																									//	AT+CPMS="SM","SM","SM"	- "SM"-использовать память SIM-карты для просмотра, чтения и удаления сообщений, "SM"-использовать память SIM-карты для написания и отправки сообщений, "SM"-использовать память SIM-карты для получения и сохранения сообщений.
//			Определяем значения переменных:																													//	
			maxSMS=SMSmax();																																//	Максимально допустимое количество входящих непрочитанных SMS сообщений.
			numSMS=maxSMS;																																	//	Номер последнего прочитанного входящего SMS сообщения.
//			Возвращаем положительный ответ инициализации:																									//	
			return true;																																	//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ СИМВОЛА В ЧИСЛО:																												//	Функция возвращает:	число uint8_t.
uint8_t	iarduino_GSM::_num(char symbol){																													//	Аргументы функции:	символ 0-9,a-f,A-F.
			uint8_t i = uint8_t(symbol);																													//	Получаем код символа
			if ( (i>=0x30) && (i<=0x39) )	{ return i-0x30; }else																							//	0-9
			if ( (i>=0x41) && (i<=0x46) )	{ return i-0x37; }else																							//	A-F
			if ( (i>=0x61) && (i<=0x66) )	{ return i-0x57; }else																							//	a-f
											{ return      0; }																								//	остальные символы вернут число 0.
}																																							//	
																																							//	
//		ФУНКЦИЯ ПРЕОБРАЗОВАНИЯ ЧИСЛА В СИМВОЛ:																												//	Функция возвращает:	символ char.
char	iarduino_GSM::_char(uint8_t num){																													//	Аргументы функции:	число 0-15.
			if(num<10){return char(num+0x30);}else																											//	0-9
			if(num<16){return char(num+0x37);}else																											//	A-F
			          {return '0';}																															//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ КОЛИЧЕСТВА ВСЕХ SMS В ПАМЯТИ:																										//	Функция возвращает:	число uint8_t соответствующее общему количеству SMS хранимых в выбранной области памяти.
uint8_t	iarduino_GSM::_SMSsum(void){																														//	Аргументы функции:	отсутствуют.
			uint8_t i, j=0;																																	//	Объявляем временные переменные.
			runAT(F("AT+CPMS?\r\n"));																														//	Выполняем команду получения выбранных областей памяти для хранения SMS сообщений.
			if( strBuffer.indexOf( F("+CPMS:")    ) < 0){return 0;}																							//	Возвращаем 0, так как в ответе нет текста "+CPMS:".
			i = strBuffer.indexOf( F("+CPMS:")    );																										//	Получаем позицию начала текста "+CPMS:" в ответе "ЭХО\r\n+CPMS: "ПАМЯТЬ1",ИСПОЛЬЗОВАНО,ОБЪЁМ, "ПАМЯТЬ2",ИСПОЛЬЗОВАНО,ОБЪЁМ, "ПАМЯТЬ3",ИСПОЛЬЗОВАНО,ОБЪЁМ\r\n\r\nOK\r\n".
			for(uint8_t k=i; k<254; k++){ if(strBuffer.charAt(k)==','){j++;} if(j==7){i=k+1; k=254;} }														//	Получаем позицию параметра ИСПОЛЬЗОВАНО, он находится через 7 запятых после текста "+CPMS:".
			j = _num(strBuffer.charAt(i)); i++;																												//	Получаем первую цифру в найденной позиции, это первая цифра количества использованной памяти.
			if( strBuffer.charAt(i)!=','  ){j*=10; j+= _num(strBuffer.charAt(i));}																			//	Если за первой цифрой не стоит знак запятой, значит там вторая цифра числа, учитываем и её.
			return j;																																		//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ КОЛИЧЕСТВА СИМВОЛОВ В СТРОКЕ С УЧЁТОМ КОДИРОВКИ:																					//	Функция возвращает:	число uint16_t соответствующее количеству символов (а не байт) в строке.
uint16_t iarduino_GSM::_SMStxtLen(const char* txt){																											//	Аргументы функции:	txt - строка с текстом
			uint16_t numIn=0, sumSymb=0;																													//	Объявляем временные переменные.
			uint16_t lenIn=strlen(txt);																														//
			uint8_t  valIn=0;																																//
			switch(codTXTsend){																																//	Тип кодировки строки txt.
			//	Получаем количество символов в строке txt при кодировке UTF-8:																				//
				case GSM_TXT_UTF8:																															//
					while(numIn<lenIn){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						valIn=uint8_t(txt[numIn]); sumSymb++;																								//
						if(valIn<0x80){numIn+=1;}else																										//  Символ состоит из 1 байта
						if(valIn<0xE0){numIn+=2;}else																										//  Символ состоит из 2 байт
						if(valIn<0xF0){numIn+=3;}else																										//  Символ состоит из 3 байт
						if(valIn<0xF8){numIn+=4;}else																										//  Символ состоит из 4 байт
						              {numIn+=5;}																											//	Символ состоит из 5 и более байт
					}																																		//
				break;																																		//
			//	Получаем количество символов в строке txt при кодировке CP866:																				//
				case GSM_TXT_CP866:   sumSymb=lenIn; break;																									//
			//	Получаем количество символов в строке txt при кодировке Windows1251:																		//
				case GSM_TXT_WIN1251: sumSymb=lenIn; break;																									//
			}	return sumSymb;																																//	
}																																							//	
																																							//	
//		ФУНКЦИЯ КОДИРОВАНИЯ ТЕКСТА SMS СООБЩЕНИЯ В СТРОКУ strBuffer В ФОРМАТЕ GSM:																			//	Функция возвращает:	число uint16_t соответствующее позиции после последнего закодированного символа из строки txt.
uint16_t iarduino_GSM::_SMScoderGSM(const char* txt, uint16_t pos, uint16_t len){																			//	Аргументы функции:	txt - строка с текстом, pos - позиция взятия первого символа из строки txt, len - количество кодируемых символов из строки txt.
			uint8_t  valByteIn = 0;																															//	Определяем временную переменную для хранения значения очередного читаемого символа из строки txt.
			uint16_t numByteIn = 0;																															//	Определяем временную переменную для хранения номера   очередного читаемого символа из строки txt.
			uint8_t  numBitIn = 0;																															//	Определяем временную переменную для хранения номера   очередного бита в байте valByteIn.
			uint8_t  valByteOut = 0;																														//	Определяем временную переменную для хранения значения очередного сохраняемого байта в строку strBuffer.
			uint8_t  numBitOut = 0;																															//	Определяем временную переменную для хранения номера   очередного бита в байте valByteOut.
			if(len==255){len=strlen(txt)-pos;}																												//
			while( numByteIn < len ){																														//	Пока номер очередного читаемого символа меньше заданного количества читаемых символов.
				valByteIn = txt[pos+numByteIn]; numByteIn+=1;																								//	Читаем значение символа с номером numByteIn в переменную valByteIn.
				for(numBitIn=0; numBitIn<7; numBitIn++){																									//	Проходим по битам байта numByteIn (номер бита хранится в numBitIn).
					bitWrite( valByteOut, numBitOut, bitRead(valByteIn,numBitIn) ); numBitOut++;															//	Копируем бит из позиции numBitIn байта numByteIn (значение valByteIn) в позицию numBitOut символа numByteOut, увеличивая значение numBitOut после каждого копирования.
					if(numBitOut>7){																														//	
						strBuffer+= _char(valByteOut/16);																									//	
						strBuffer+= _char(valByteOut%16);																									//	
						valByteOut=0; numBitOut=0;																											//	
					}																																		//	При достижении numBitOut позиции старшего бита в символе (numBitOut>=7), обнуляем старший бит символа (txt[numByteOut]&=0x7F), обнуляем позицию бита в символе (numBitOut=0), переходим к следующему символу (numByteOut++).
				}																																			//	
			}																																				//	
			if(numBitOut){																																	//	
				strBuffer+= _char(valByteOut/16);																											//	
				strBuffer+= _char(valByteOut%16);																											//	
			}																																				//	
			return pos+numByteIn;																															//	
}																																							//	
																																							//	
//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ GSM ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	iarduino_GSM::_SMSdecodGSM(char* txt, uint16_t len, uint16_t pos, uint16_t udh_len){																//	Аргументы функции:	txt - строка для текста, len - количество символов в тексте, pos - позиция начала текста в строке strBuffer, udh_len количество байт занимаемое заголовком.
			if(udh_len>0){ len -= udh_len*8/7; if(udh_len*8%7){len--;} }																					//	Если текст начинается с заголовка, то уменьшаем количество символов в тексте на размер заголовка.
			uint8_t  valByteIn  = 0;																														//	Определяем временную переменную для хранения значения очередного читаемого байта из строки strBuffer.
			uint16_t numByteIn  = udh_len*2;																												//	Определяем временную переменную для хранения номера   очередного читаемого байта из строки strBuffer.
			uint8_t  numBitIn   = udh_len==0?0:(7-(udh_len*8%7));																							//	Определяем временную переменную для хранения номера   очередного бита в байте numByteIn.
			uint16_t numByteOut = 0;																														//	Определяем временную переменную для хранения номера   очередного раскодируемого символа для строки txt.
			uint8_t  numBitOut  = 0;																														//	Определяем временную переменную для хранения номера   очередного бита в байте numByteOut.
			while(numByteOut<len){																															//	Пока номер очередного раскодируемого символа не станет больше объявленного количества символов.
				valByteIn = _num(strBuffer.charAt(pos+numByteIn))*16 + _num(strBuffer.charAt(pos+numByteIn+1)); numByteIn+=2;								//	Читаем значение байта с номером numByteIn в переменную valByteIn.
				while(numBitIn<8){																															//	Проходим по битам байта numByteIn (номер бита хранится в numBitIn).
					bitWrite( txt[numByteOut], numBitOut, bitRead(valByteIn,numBitIn) ); numBitOut++;														//	Копируем бит из позиции numBitIn байта numByteIn (значение valByteIn) в позицию numBitOut символа numByteOut, увеличивая значение numBitOut после каждого копирования.
					if(numBitOut>=7){ txt[numByteOut]&=0x7F; numBitOut=0; numByteOut++;}																	//	При достижении numBitOut позиции старшего бита в символе (numBitOut>=7), обнуляем старший бит символа (txt[numByteOut]&=0x7F), обнуляем позицию бита в символе (numBitOut=0), переходим к следующему символу (numByteOut++).
					numBitIn++;																																//	
				}	numBitIn=0;																																//	
			}		txt[numByteOut]=0;																														//	Присваиваем символу len+1 значение 0 (конец строки).
}																																							//	
																																							//	
//		ФУНКЦИЯ КОДИРОВАНИЯ ТЕКСТА SMS СООБЩЕНИЯ В СТРОКУ strBuffer В ФОРМАТЕ 8BIT:																			//	Функция возвращает:	число uint16_t соответствующее позиции после последнего закодированного символа из строки txt.
//uint16_t iarduino_GSM::_SMScoder8BIT(String txt, uint16_t pos, uint16_t len){																				//	Аргументы функции:	txt - строка с текстом, pos - позиция взятия первого символа из строки txt, len - количество кодируемых символов из строки txt.
		//	В библиотеке не реализован 8-битный метод передачи данных.																						//	
		//	Для передачи данных текст кодируется в формат GSM (7бит) или UCS2 (16бит).																		//	
		//	Для 8-битных данных реализован только приём.																									//	
//}																																							//	
																																							//	
//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ 8BIT ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	iarduino_GSM::_SMSdecod8BIT(char* txt, uint16_t len, uint16_t pos){																					//	Аргументы функции:	txt - строка для текста, len - количество байт в тексте, pos - позиция начала текста в строке strBuffer.
			uint16_t numByteIn  = 0;																														//	Определяем временную переменную для хранения номера   очередного читаемого байта из строки strBuffer.
			uint16_t numByteOut = 0;																														//	Определяем временную переменную для хранения номера очередного раскодируемого символа для строки txt.
			while(numByteOut<len){																															//	Пока номер очередного раскодируемого символа не станет больше объявленного количества символов.
				txt[numByteOut]= _num(strBuffer.charAt(pos+numByteIn))*16 + _num(strBuffer.charAt(pos+numByteIn+1)); numByteIn+=2; numByteOut++;			//	Читаем значение байта с номером numByteIn в символ строки txt под номером numByteOut.
			}	txt[numByteOut]=0;																															//	Присваиваем символу len+1 значение 0 (конец строки).
}																																							//	
																																							//	
//		ФУНКЦИЯ КОДИРОВАНИЯ ТЕКСТА SMS СООБЩЕНИЯ В СТРОКУ strBuffer В ФОРМАТЕ UCS2:																			//	Функция возвращает:	число uint16_t соответствующее позиции после последнего закодированного символа из строки txt.
uint16_t iarduino_GSM::_SMScoderUCS2(const char* txt, uint16_t pos, uint16_t len){																			//	Аргументы функции:	txt - строка с текстом, pos - позиция взятия первого символа из строки txt, len - количество кодируемых символов из строки txt.
			uint8_t  valByteInThis  = 0;																													//	Определяем временную переменную для хранения значения очередного читаемого байта.
			uint8_t  valByteInNext  = 0;																													//	Определяем временную переменную для хранения значения следующего читаемого байта.
			uint16_t numByteIn      = pos;																													//	Определяем временную переменную для хранения номера очередного   читаемого байта.
			uint16_t numSymbIn      = 0;																													//	Определяем временную переменную для хранения номера очередного   читаемого символа.
			uint8_t  lenTXT         = strlen(txt);																											//	Определяем временную переменную для хранения длины строки в байтах.
			switch(codTXTsend){																																//	Тип кодировки строки StrIn.
//				Преобразуем текст из кодировки UTF-8 кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)							//	
				case GSM_TXT_UTF8:																															//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						valByteInThis = uint8_t(txt[numByteIn  ]);																							//	Читаем значение очередного байта.
						valByteInNext = uint8_t(txt[numByteIn+1]);																							//	Читаем значение следующего байта.
						numSymbIn++;																														//	Увеличиваем количество прочитанных символов.
								if (valByteInThis==0x00)							{return numByteIn;														//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80)							{numByteIn+=1;	strBuffer+="00";										//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем его старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xD0)							{numByteIn+=2;	strBuffer+="04";										//	Очередной прочитанный символ состоит из 2 байт и является символом Русского алфавита, записываем 04 и проверяем значение байта valByteInNext ...
								if (valByteInNext==0x81)							{				strBuffer+="01";}										//	Симол  'Ё'       - 208 129            =>  04 01
								if((valByteInNext>=0x90)&&(valByteInNext<=0xBF))	{				strBuffer+=_char((valByteInNext-0x80)/16);				//	Симолы 'А-Я,а-п' - 208 144 - 208 191  =>  04 16 - 04 63
																									strBuffer+=_char((valByteInNext-0x80)%16);}				//	
						}else	if (valByteInThis==0xD1)							{numByteIn+=2;	strBuffer+="04";										//	Очередной прочитанный символ состоит из 2 байт и является символом Русского алфавита, записываем 04 и проверяем значение байта valByteInNext ...
								if (valByteInNext==0x91)							{				strBuffer+="51";}										//	Симол  'ё'       - 209 145            =>  04 81
								if((valByteInNext>=0x80)&&(valByteInNext<=0x8F))	{				strBuffer+=_char((valByteInNext-0x40)/16);				//	Симолы 'р-я'     - 209 128 - 209 143  =>  04 64 - 04 79
																									strBuffer+=_char((valByteInNext-0x40)%16);}				//	
						}else	if (valByteInThis <0xE0)							{numByteIn+=2;	strBuffer+="043F";										//  Очередной прочитанный символ состоит из 2 байт,         записываем его как символ '?'
						}else	if (valByteInThis <0xF0)							{numByteIn+=3;	strBuffer+="003F";										//  Очередной прочитанный символ состоит из 3 байт,         записываем его как символ '?'
						}else	if (valByteInThis <0xF8)							{numByteIn+=4;	strBuffer+="003F";										//  Очередной прочитанный символ состоит из 4 байт,         записываем его как символ '?'
						}else														{numByteIn+=5;	strBuffer+="003F";}										//	Очередной прочитанный символ состоит из 5 и более байт, записываем его как символ '?'
					}																																		//
				break;																																		//
//				Преобразуем текст из кодировки CP866 в кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)						//	
				case GSM_TXT_CP866:																															//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						valByteInThis = uint8_t(txt[numByteIn]);																							//	Читаем значение очередного символа.
						numSymbIn++; numByteIn++;																											//	Увеличиваем количество прочитанных символов и номер прочитанного байта.
								if (valByteInThis==0x00)							{return numByteIn;														//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80)							{				strBuffer+="00";										//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем его старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xF0)							{				strBuffer+="0401";										//	Симол  'Ё'       - 240            =>  04 01              	        записываем его байты.
						}else	if (valByteInThis==0xF1)							{				strBuffer+="0451";										//	Симол  'e'       - 241            =>  04 81                         записываем его байты.
						}else	if((valByteInThis>=0x80)&&(valByteInThis<=0xAF))	{				strBuffer+="04"; valByteInThis-=0x70;					//  Симолы 'А-Я,а-п' - 128 - 175      =>  04 16 - 04 63                 записываем 04 и вычисляем значение для кодировки UCS2:
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем младший полубайт.
						}else	if((valByteInThis>=0xE0)&&(valByteInThis<=0xEF))	{				strBuffer+="04"; valByteInThis-=0xA0;					//	Симолы 'р-я'     - 224 - 239      =>  04 64 - 04 79                 записываем 04 и вычисляем значение для кодировки UCS2:
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем младший полубайт.
						}																																	//
					}																																		//
				break;																																		//
//				Преобразуем текст из кодировки Windows1251 в кодировку UCS2: (и записываем его в строку strBuffer в формате текстового HEX)					//	
				case GSM_TXT_WIN1251:																														//
					while(numSymbIn<len && numByteIn<lenTXT){																								//	Пока номер очередного читаемого символа не станет больше объявленного количества кодируемых символов или не превысит длину строки.
						valByteInThis = uint8_t(txt[numByteIn]);																							//	Читаем значение очередного символа.
						numSymbIn++; numByteIn++;																											//	Увеличиваем количество прочитанных символов и номер прочитанного байта.
								if (valByteInThis==0x00)							{return numByteIn;														//  Очередной прочитанный символ является символом конца строки, возвращаем номер прочитанного байта.
						}else	if (valByteInThis <0x80)							{				strBuffer+="00";										//  Очередной прочитанный символ состоит из 1 байта и является символом латинского алфавита, записываем 00 и его значение.
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем его старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем его младший полубайт.
						}else	if (valByteInThis==0xA8)							{				strBuffer+="0401";										//	Симол  'Ё'       - 168            =>  04 01              	        записываем его байты.
						}else	if (valByteInThis==0xB8)							{				strBuffer+="0451";										//	Симол  'e'       - 184            =>  04 81                         записываем его байты.
						}else	if((valByteInThis>=0xC0)&&(valByteInThis<=0xEF))	{				strBuffer+="04"; valByteInThis-=0xB0;					//  Симолы 'А-Я,а-п' - 192 - 239      =>  04 16 - 04 63                 записываем 04 и вычисляем значение для кодировки UCS2:
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем младший полубайт.
						}else	if((valByteInThis>=0xF0)&&(valByteInThis<=0xFF))	{				strBuffer+="04"; valByteInThis-=0xB0;					//	Симолы 'р-я'     - 240 - 255      =>  04 64 - 04 79                 записываем 04 и вычисляем значение для кодировки UCS2:
																									strBuffer+=_char(valByteInThis/16);						//	                                                                    записываем старший полубайт.
																									strBuffer+=_char(valByteInThis%16);						//	                                                                    записываем младший полубайт.
						}																																	//
					}																																		//
				break;																																		//
			}																																				//	
			return numByteIn;																																//	
}																																							//	
																																							//	
//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ UCS2 ТЕКСТА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	iarduino_GSM::_SMSdecodUCS2(char* txt, uint16_t len, uint16_t pos){																					//	Аргументы функции:	txt - строка для текста, len - количество байт в тексте, pos - позиция начала текста в строке strBuffer.
			uint8_t  byteThis = 0;																															//	Определяем временную переменную для хранения значения очередного читаемого байта.
			uint8_t  byteNext = 0;																															//	Определяем временную переменную для хранения значения следующего читаемого байта.
			uint16_t numIn    = 0;																															//	Определяем временную переменную для хранения номера   очередного читаемого байта.
			uint16_t numOut   = 0;																															//	Определяем временную переменную для хранения номера   очередного раскодируемого символа.
			len*=2;																																			//	Один байт данных занимает 2 символа в строке strBuffer.
			switch(codTXTread){																																//	Тип кодировки строки StrIn.
//				Преобразуем текст из кодировки UCS2 в кодировку UTF-8:																						//
				case GSM_TXT_UTF8:																															//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){                            txt[numOut]=byteNext;      numOut++;}else											//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=0xD0; numOut++; txt[numOut]=byteNext+0x80; numOut++;}else											//	Симол  'Ё'       - 04 01          =>  208 129
						if(byteNext==0x51){txt[numOut]=0xD1; numOut++; txt[numOut]=byteNext+0x40; numOut++;}else											//	Симол  'ё'       - 04 81          =>  209 145
						if(byteNext< 0x40){txt[numOut]=0xD0; numOut++; txt[numOut]=byteNext+0x80; numOut++;}else											//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  208 144 - 208 191
						                  {txt[numOut]=0xD1; numOut++; txt[numOut]=byteNext+0x40; numOut++;}												//	Симолы 'р-я'     - 04 64 - 04 79  =>  209 128 - 209 143
					}                      txt[numOut]=0;																									//
				break;																																		//
//				Преобразуем текст из кодировки UCS2 в кодировку CP866:																						//
				case GSM_TXT_CP866:																															//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){txt[numOut]=byteNext;      numOut++;}else																		//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=byteNext+0xEF; numOut++;}else																		//	Симол  'Ё'       - 04 01          =>  240
						if(byteNext==0x51){txt[numOut]=byteNext+0xA0; numOut++;}else																		//	Симол  'ё'       - 04 81          =>  141
						if(byteNext< 0x40){txt[numOut]=byteNext+0x70; numOut++;}else																		//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  128 - 175
						                  {txt[numOut]=byteNext+0xA0; numOut++;}																			//	Симолы 'р-я'     - 04 64 - 04 79  =>  224 - 239
					}                      txt[numOut]=0;																									//
				break;																																		//
//				Преобразуем текст из кодировки UCS2 в кодировку Windows1251:																				//
				case GSM_TXT_WIN1251:																														//
					while(numIn<len){																														//	Пока номер очередного читаемого байта не станет больше объявленного количества байтов.
						byteThis = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение очередного байта в переменную byteThis.
						byteNext = _num(strBuffer.charAt(pos+numIn))*16 + _num(strBuffer.charAt(pos+numIn+1)); numIn+=2;									//	Читаем значение следующего байта в переменную byteNext.
						if(byteThis==0x00){txt[numOut]=byteNext;      numOut++;}else																		//  Символы латинницы
						if(byteNext==0x01){txt[numOut]=byteNext+0xA7; numOut++;}else																		//	Симол  'Ё'       - 04 01          =>  168
						if(byteNext==0x51){txt[numOut]=byteNext+0x67; numOut++;}else																		//	Симол  'ё'       - 04 81          =>  184
						if(byteNext< 0x40){txt[numOut]=byteNext+0xB0; numOut++;}else																		//	Симолы 'А-Я,а-п' - 04 16 - 04 63  =>  192 - 239
						                  {txt[numOut]=byteNext+0xB0; numOut++;}																			//	Симолы 'р-я'     - 04 64 - 04 79  =>  240 - 255
					}                      txt[numOut]=0;																									//
				break;																																		//
			}																																				//	
}																																							//	
																																							//	
//		ФУНКЦИЯ КОДИРОВАНИЯ АДРЕСА SMS СООБЩЕНИЯ В СТРОКУ strBuffer:																						//
void	iarduino_GSM::_SMScoderAddr(const char* num){																										//	Аргументы функции:	num - строка с адресом для кодирования.
		uint16_t j=num[0]=='+'?1:0, len=strlen(num);																										//	Определяем временные переменные.
		for(uint16_t i=j; i<len; i+=2){																														//	
			if( (len<=(i+1)) || (num[i+1]==0) ){strBuffer+="F";}else{strBuffer+=num[i+1];}																	//	Сохраняем следующий символ из строки num в строку strBuffer, если символа в строке num нет, то сохраняем символ 'F'.
			if( (len<= i   ) || (num[i  ]==0) ){strBuffer+="F";}else{strBuffer+=num[i  ];}																	//	Сохраняем текущий   символ из строки num в строку strBuffer, если символа в строке num нет, то сохраняем символ 'F'.
		}																																					//
}																																							//	
																																							//	
//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ АДРЕСА SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																					//
void	iarduino_GSM::_SMSdecodAddr(char* num, uint16_t len, uint16_t pos){																					//	Аргументы функции:	num - строка для получения адреса.
		uint8_t j=0;																																		//						len - количество полубайт в адресе (количество символов в номере).
		for(uint16_t i=0; i<len; i+=2){																														//						pos - позиция адреса в строке strBuffer.
			if( (strBuffer.charAt(pos+i+1)!='F') && (strBuffer.charAt(pos+i+1)!='f') ){num[i]=strBuffer.charAt(pos+i+1); j++;}								//	Сохраняем следующий символ из строки strBuffer на место текущего   в строке num, если этот символ не 'F' или 'f'.
			if( (strBuffer.charAt(pos+i  )!='F') && (strBuffer.charAt(pos+i  )!='f') ){num[i+1]=strBuffer.charAt(pos+i); j++;}								//	Сохраняем текущий   символ из строки strBuffer на место следующего в строке num, если этот символ не 'F' или 'f'.
		}	num[j]=0;																																		//	
}																																							//	
																																							//	
//		ФУНКЦИЯ РАЗКОДИРОВАНИЯ ДАТЫ ОТПРАВКИ SMS СООБЩЕНИЯ ИЗ СТРОКИ strBuffer:																				//
void	iarduino_GSM::_SMSdecodDate(char* tim, uint16_t pos){																								//	Аргументы функции:	tim - строка для даты
			tim[ 0]=strBuffer.charAt(pos+5);																												//	ст. день.			pos - позиция даты в строке strBuffer
			tim[ 1]=strBuffer.charAt(pos+4);																												//	мл. день.
			tim[ 2]='.';																																	//	
			tim[ 3]=strBuffer.charAt(pos+3);																												//	ст. мес.
			tim[ 4]=strBuffer.charAt(pos+2);																												//	мл. мес.
			tim[ 5]='.';																																	//	
			tim[ 6]=strBuffer.charAt(pos+1);																												//	ст. год.
			tim[ 7]=strBuffer.charAt(pos+0);																												//	мл. год.
			tim[ 8]=' ';																																	//	
			tim[ 9]=strBuffer.charAt(pos+7);																												//	ст. час.
			tim[10]=strBuffer.charAt(pos+6);																												//	мл. час.
			tim[11]=':';																																	//	
			tim[12]=strBuffer.charAt(pos+9);																												//	ст. мин.
			tim[13]=strBuffer.charAt(pos+8);																												//	мл. мин.
			tim[14]=':';																																	//	
			tim[15]=strBuffer.charAt(pos+11);																												//	ст. сек.
			tim[16]=strBuffer.charAt(pos+10);																												//	мл. сек.
			tim[17]=0;																																		//	конец строки.
}																																							//	
																																							//	
//		ФУНКЦИЯ ВЫПОЛНЕНИЯ AT-КОМАНД:																														//	Функция возвращает:	строку с ответом модуля.
String	iarduino_GSM::runAT(String command, uint32_t timeout, bool early){																					//	Аргументы функции:	command - строка с АТ-командой,
//			Проверяем устанавливалась ли скорость обмена данными по шине UART:																				//						timeout - таймаут в миллисекундах,	
			if(!flgSpeed){return "";}																														//						early   - флаг разрешающий досрочный выход при ответе модуля "\r\nOK\r\n" или "ERROR".
//			Объявляем временные переменные:																													//	
            char i;																																			//	Переменная для хранения очередного принятого символа из строки ответа.
			bool f;																																			//	Флаг указывающий на то, что принят очередной символ  из строки ответа.
			uint8_t cnt = 0;																																//	Счетчик совпадний ответа со строкой "\r\nOK\r\n" или "ERROR".
			uint32_t millisEnd;																																//	Переменная для хранения времени выхода из функции.
//			Чистим буфер UART:																																//	
			if(flgType)	{while((*(SoftwareSerial*)objSerial).available()>0){(*(SoftwareSerial*)objSerial).read();}}											//	Читаем все символы из буфера в никуда.
			else		{while((*(HardwareSerial*)objSerial).available()>0){(*(HardwareSerial*)objSerial).read();}}											//	Читаем все символы из буфера в никуда.
//			Передаём AT-команду:																															//	
			if(flgType)	{(*(SoftwareSerial*)objSerial).print(command);}																						//	
			else		{(*(HardwareSerial*)objSerial).print(command);}																						//	
//			Чистим строку для получения ответа:																												//	
			strBuffer = "";																																	//	
//			Определяем время выхода из функции:																												//	
			millisEnd = millis() + timeout;																													//	Вычисляем время выхода из функции.
//			Читаем ответ модуля в строку strBuffer:																											//	
			while(millis()<millisEnd){																														//	
//				Читаем очередной символ:																													//	
				f=0;																																		//
				if(flgType)	{if((*(SoftwareSerial*)objSerial).available()>0){i=(*(SoftwareSerial*)objSerial).read(); strBuffer+=i; f=1;}else{delay(10);}}	//	
				else		{if((*(HardwareSerial*)objSerial).available()>0){i=(*(HardwareSerial*)objSerial).read(); strBuffer+=i; f=1;}else{delay(10);}}	//	
//				Досрочно выходим из цикла получения ответа:																									//	Проверяем не пришли ли подряд символы "\r\nOK\r\n" или "ERROR".
				if(early){																																	//
					if(f){																																	//
						if(cnt==1) {if(i=='\n'){cnt++; }                        else{cnt=0;}}else															//	Если после 1 совпадения пришел символ '\n', то считаем его 2 символом совпавшим со строкой "\r\nOK\r\n".
						if(cnt==2) {if(i== 'O'){cnt++; }                        else{cnt=0;}}else															//	Если после 2 совпадений пришел символ  'O', то считаем его 3 символом совпавшим со строкой "\r\nOK\r\n".
						if(cnt==3) {if(i== 'K'){cnt++; }                        else{cnt=0;}}else															//	Если после 3 совпадений пришел символ  'K', то считаем его 4 символом совпавшим со строкой "\r\nOK\r\n".
						if(cnt==4) {if(i=='\r'){cnt++; }                        else{cnt=0;}}else															//	Если после 4 совпадений пришел символ '\r', то считаем его 5 символом совпавшим со строкой "\r\nOK\r\n".
						if(cnt==5) {if(i=='\n'){cnt=10;}                        else{cnt=0;}}else															//	Если после 5 совпадений пришел символ '\n', значит в ответе есть "\r\nOK\r\n", выходим из цикла.
						if(cnt==6) {                         if(i=='R'){cnt++; }else{cnt=0;}}else															//	Если после 1 совпадения пришел символ  'R', то считаем его 2 символом совпавшим со строкой "ERROR"
						if(cnt==7) {                         if(i=='R'){cnt++; }else{cnt=0;}}else															//	Если после 2 совпадений пришел символ  'R', то считаем его 3 символом совпавшим со строкой "ERROR"
						if(cnt==8) {                         if(i=='O'){cnt++; }else{cnt=0;}}else															//	Если после 3 совпадений пришел символ  'O', то считаем его 4 символом совпавшим со строкой "ERROR"
						if(cnt==9) {                         if(i=='R'){cnt=10;}else{cnt=0;}}																//	Если после 4 совпадений пришел символ  'R', значит в ответе есть "ERROR", выходим из цикла.
						if(cnt==0) {if(i=='\r'){cnt=1; }else if(i=='E'){cnt=6; }else{cnt=0;}}																//	Если при отсутствии совпадений пришел символ '\r' или 'E', то считаем его 1 символом совпавшим со строкой "\r\nOK\r\n" или "ERROR".
						if(cnt==10){millisEnd=millis();}																									//	Обратите внимание на то что первый символ проверяется в конце, иначе в строке "...\r\n\r\nOK\r\n" не будет обнаружено совпадений.
					}																																		//	
				}																																			//	
			}																																				//	
//			Возвращаем ответ модуля																															//	
			return strBuffer;																																//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ВЫПОЛНЕНИЯ USSD-ЗАПРСА:																														//	Функция возвращает:	строку с ответом модуля.
String	iarduino_GSM::runUSSD(String command, uint32_t timeout){																							//	Аргументы функции:	command - строка с USSD-командой,
//			Объявляем временные переменные:																													//						timeout - таймаут в миллисекундах,	
			int i;																																			//	Переменная для хранения найденных позиций в ответе.
            char j;																																			//	Переменная для хранения очередного принятого символа из строки ответа.
			bool f;																																			//	Флаг указывающий на то, что принят очередной символ  из строки ответа.
			uint8_t cnt = 0;																																//	Счетчик совпадний ответа со строкой "\r\nOK\r\n" или "ERROR".
			uint32_t millisEnd;																																//	Переменная для хранения времени выхода из функции.
			uint16_t pos, len; 																																//	Переменные для хранения позиции начала и длины информационного блока в ответе (текст ответа).
			uint8_t stat, coder;																															//	Переменные для хранения статуса USSD запроса и кодировки USSD ответа.
			char _txt[161];																																	//	Строка для временного хранения раскодированного текста.
//			Чистим строку для получения ответа:																												//	
			strBuffer = "";																																	//	
//			Определяем время выхода из функции:																												//	
			millisEnd = millis() + timeout;																													//	Вычисляем время выхода из функции.
//			Отправляем запрос:																																//	
			runAT((String)"AT+CUSD=1,\""+command+"\",15\r\n", timeout);																						//	Выполняем команду отправки USSD запроса, модуль ответит "ЭХО\r\nOK\r\n\r\n+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n", но функция runAT досрочно выйдет из цикла чтения после обнаружения комбинации символов "\r\nOK\r\n", значит остальная часть ответа еще будет в буфере UART.
//			Дочитываем ответ модуля в строку strBuffer:																										//	Дочитывается ответ содержащий строку "+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n"
			while(millis()<millisEnd){																														//	
//				Читаем очередной символ:																													//	
				f=0;																																		//
				if(flgType)	{if((*(SoftwareSerial*)objSerial).available()>0){j=(*(SoftwareSerial*)objSerial).read(); strBuffer+=j; f=1;}else{delay(10);}}	//	
				else		{if((*(HardwareSerial*)objSerial).available()>0){j=(*(HardwareSerial*)objSerial).read(); strBuffer+=j; f=1;}else{delay(10);}}	//	
//				Досрочно выходим из цикла получения ответа:																									//	Проверяем не пришли ли подряд символы "\r\nOK\r\n" или "ERROR".
				if(f){																																		//
					if(cnt==0) {if(j=='\r'){cnt++;}else{cnt=0;}}else																						//	Если при отсутствии совпадений пришел символ '\r', то считаем его 1 символом совпавшим с 1 строкой "\r\n".
					if(cnt==1) {if(j=='\n'){cnt++;}else{cnt=0;}}else																						//	Если после 1 совпадения пришел символ '\n', то считаем его 2 символом совпавшим с 1 строкой "\r\n".
					if(cnt==2) {if(j=='\r'){cnt++;}else{cnt=2;}}else																						//	Если после 2 совпадения пришел символ '\r', то считаем его 1 символом совпавшим с 2 строкой "\r\n".
					if(cnt==3) {if(j=='\n'){cnt++;}else{cnt=2;}}																							//	Если после 3 совпадения пришел символ '\n', то считаем его 2 символом совпавшим с 2 строкой "\r\n".
					if(cnt==4) {millisEnd=millis();}																										//	Если зафиксировано 4 совпадения, значит в ответе есть две комбинации символов "\r\n", выходим из цикла.
				}																																			//	
			}																																				//	
//			Разбираем ответ:																																//	
			i = strBuffer.indexOf( F("+CUSD:") );	if(i<0){return "";}		i+=7;	stat = _num(strBuffer.charAt(i));										//	Получаем  позицию начала текста "+CUSD:" в ответе "ЭХО\r\nOK\r\n\r\n+CUSD: СТАТУС, "ТЕКСТ" ,КОДИРОВКА\r\n".
			i = strBuffer.indexOf( '"' , i);		if(i<0){return "";}		i++;	pos = i;																//	Сохраняем позицию первого символа текста ответа на USSD запрос.
			i = strBuffer.indexOf( '"' , i);		if(i<0){return "";}				len = i-pos; len/=2;													//	Сохраняем количество байтов в тексте ответа на USSD запрос.
			i = strBuffer.indexOf( ',' , i);		if(i<0){return "";}		i++;	coder = _num(strBuffer.charAt(i)); i++;									//	Сохраняем значение первой цифры кодировки текста.
			if( strBuffer.charAt(i)!='\r'  ){coder*=10;								coder+= _num(strBuffer.charAt(i)); i++;}								//	Сохраняем значение второй цифры кодировки текста (если таковая существует).
			if( strBuffer.charAt(i)!='\r'  ){coder*=10;								coder+= _num(strBuffer.charAt(i)); i++;}								//	Сохраняем значение третей цифры кодировки текста (если таковая существует).
//			Разкодируем ответ из строки strBuffer в строку _txt:																							//
			if(coder==72){_SMSdecodUCS2(_txt, len,			pos);}else																						//	Разкодируем ответ в строку _txt указав len - количество байт     и pos - позицию начала закодированного текста в строке strBuffer.
			if(coder==15){_SMSdecodGSM (_txt, (len/7*8),	pos);}else																						//	Разкодируем ответ в строку _txt указав len - количество символов и pos - позицию начала закодированного текста в строке strBuffer.
			             {_SMSdecod8BIT(_txt, len,			pos);}																							//	Разкодируем ответ в строку _txt указав len - количество байт     и pos - позицию начала закодированного текста в строке strBuffer.
//			Возвращаем ответ:																																//	
			strBuffer=_txt;																																	//	
			return(strBuffer);																																//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ВКЛЮЧЕНИЯ И ВЫКЛЮЧЕНИЯ МОДУЛЯ:																												//	
void	iarduino_GSM::pwr(bool f){																															//	Аргументы функции:	f - флаг (0-выключить / 1-включить)
			if(f){_begin();}else{digitalWrite(pinPWR, HIGH);}																								//	Для выключения модуля нужно подать 1 на вывод pinPWR, а включение модуля осуществляется функцией _begin() в которой на вывод pinPWR подаётся 0.
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ СОСТОЯНИЯ GSM:																													//	Функция возвращает:	целочисленное значение uint8_t соответствующее текущему состоянию GSM.
uint8_t	iarduino_GSM::status(void){																															//	Аргументы функции:	отсутствуют.
			int i;																																			//	Объявляем временную переменную.
			if (!flgSpeed                                       )	{return GSM_SPEED_ERR;		}															//	Не удалось согласовать скорость UART.
			runAT(F("AT+CPAS\r\n"));																														//	Выполняем команду получения статуса модуля.
			if ( strBuffer.indexOf( F("+CPAS:1"       )   ) > -1)	{return GSM_UNAVAILABLE;	}															//	Если статус модуля равен 1 значит он недоступен и AT-команды не выполняются, хотя мне интересно, а как он ответит?)))
			if ( strBuffer.indexOf( F("+CPAS:2"       )   ) > -1)	{return GSM_UNKNOWN;		}															//	Если статус модуля равен 2 значит он неизвестен и корректное выполнение AT-команд не гарантируется.
			if ( strBuffer.indexOf( F("+CPAS:5"       )   ) > -1)	{return GSM_SLEEP;			}															//	Если статус модуля равен 5 значит он находится в режиме ограниченной функциональности.
			runAT(F("AT+CPIN?\r\n"));																														//	Выполняем команду проверки аутентификации SIM-карты.
			if ( strBuffer.indexOf( F("+CPIN:SIM PIN" )   ) > -1)	{return GSM_SIM_PIN;		}															//	Требуется ввод PIN1 (CHV1).
			if ( strBuffer.indexOf( F("+CPIN:SIM PUK" )   ) > -1)	{return GSM_SIM_PUK;		}															//	Требуется ввод PUK1 и новый PIN1.
			if ( strBuffer.indexOf( F("+CPIN:SIM PIN2")   ) > -1)	{return GSM_SIM_PIN2;		}															//	Требуется ввод PIN2 (CHV2).
			if ( strBuffer.indexOf( F("+CPIN:SIM PUK2")   ) > -1)	{return GSM_SIM_PUK2;		}															//	Требуется ввод PUK2 и новый PIN2.
			if ( strBuffer.indexOf( F("+CME ERROR:10" )   ) > -1)	{return GSM_SIM_NO;			}															//	Нет сим карты.
			if ( strBuffer.indexOf( F("+CME ERROR:13" )   ) > -1)	{return GSM_SIM_FAULT;		}															//	SIM-карта неисправна.
			if ( strBuffer.indexOf( F("+CPIN:READY"   )   ) <  0)	{return GSM_SIM_ERR;		}															//	Неопределённое состояние SIM-карты.
			runAT(F("AT+CREG?\r\n"));																														//	Выполняем команду проверки статуса регистрации в сети сотового оператора.
			if ( strBuffer.indexOf( F("+CREG:"        )   ) <  0)	{return GSM_REG_ERR;		}															//	Статус регистрации в сети неизвестен, так как в ответе нет текста "+CREG:".
			i =  strBuffer.indexOf( F("+CREG:"        )   );																								//	Получаем позицию начала текста "+CREG:" в ответе "ЭХО\r\n+CREG: РЕЖИМ,СТАТУС\r\n\r\nOK\r\n".
			i =  strBuffer.indexOf( F(","             ),i ) +  1;																							//	Получаем позицию символа следующего за первой запятой встреченной после текста "+CREG:", это второй аргумент ответа являющийся статусом регистрации в сети.
			if ( strBuffer.charAt ( i+1 ) != 13                 )	{return GSM_REG_NO;			}															//	Статус регистрации модема в сети неизвестен, так как параметр статус является двухзначным числом.
			if ( strBuffer.charAt ( i   ) =='0'                 )	{return GSM_REG_NO;			}															//	Если параметр статус равен 0, значит модем не зарегистрирован в сети и поиск оператора не выполняется.
			if ( strBuffer.charAt ( i   ) =='2'                 )	{return GSM_REG_NO;			}															//	Если параметр статус равен 2, значит модем не зарегистрирован в сети, но выполняется поиск оператора.
			if ( strBuffer.charAt ( i   ) =='3'                 )	{return GSM_REG_FAULT;		}															//	Если параметр статус равен 3, значит оператор отклонил регистрацию модема в сети.
			if(( strBuffer.charAt ( i   ) !='1'           )&&																								//	Если параметр статус не равен 1 (зарегистрирован в сети домашнего оператора) и ...
			   ( strBuffer.charAt ( i   ) !='5'           )     )	{return GSM_REG_NO;			}															//	Если параметр статус не равен 5 (зарегистрирован в роуменге), то статус регистрации модема в сети неизвестен.
																	 return GSM_OK;																			//	Возвращаем положительный статус состояния модуля.
}																																							//	
																																							//	
//		ФУНКЦИЯ ВВОДА PIN-КОДА:																																//	Функция возвращает:	флаг корректности введённого pin-кода.
bool	iarduino_GSM::pin(const char* pin_code){																											//	Аргументы функции:	pin_code - строка с pin-кодом.
			runAT(F("AT+CPIN?\r\n"));																														//	Выполняем команду проверки аутентификации SIM-карты.
			if ( strBuffer.indexOf( F("+CPIN:SIM PIN" ) ) <  0)		{return false;}																			//	Если ввод PIN1 (CHV1) не требуется, то возвращаем false.
			runAT((String) "AT+CPIN=\"" + pin_code + "\"\r\n" );																							//	Выполняем команду ввода PIN1 (CHV1).
			if ( strBuffer.indexOf( F("ERROR"         ) ) > -1)		{return false;}																			//	Если PIN1 (CHV1) не принят, то возвращаем false.
			return true;																																	//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ВВОДА PUK-КОДА И НОВОГО PIN-КОДА:																											//	Функция возвращает:	флаг корректности введённого puk-кода.
bool	iarduino_GSM::puk(const char* puk_code, const char* pin_code){																						//	Аргументы функции:	puk_code - строка с puk-кодом, pin_code - строка с новым pin-кодом.
			runAT(F("AT+CPIN?\r\n"));																														//	Выполняем команду проверки аутентификации SIM-карты.
			if ( strBuffer.indexOf( F("+CPIN:SIM PUK" ) ) <  0)		{return false;}																			//	Если ввод PUK1 не требуется, то возвращаем false.
			runAT((String) "AT+CPIN=\"" + puk_code + "\",\""+ pin_code + "\"\r\n" );																		//	Выполняем команду ввода PUK1 и нового PIN1 (CHV1).
			if ( strBuffer.indexOf( F("ERROR"         ) ) > -1)		{return false;}																			//	Если PUK1 не принят, то возвращаем false.
			return true;																																	//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ УРОВНЯ СИГНАЛА:																													//	Функция возвращает:	целочисленное значение uint8_t от 0 до 31 соответствующее уровню сигнала.
uint8_t	iarduino_GSM::signal(void){																															//	Аргументы функции:	отсутствуют.
			int i; uint8_t j=0;																																//	Объявляем временные переменные.
			runAT(F("AT+CSQ\r\n"));																															//	Выполняем команду получения уровня сигнала.
			i = strBuffer.indexOf( F("+CSQ:") ); if(i<0){return 0;}																							//	Получаем позицию начала текста "+CSQ:" в ответе "ЭХО\r\n+CSQ: УРОВЕНЬ,ОШИБКИ\r\nOK\r\n".
			i+= 6;																																			//	Получаем позицию первого символа числа указвающего на уровень сигнала.
			j = _num(strBuffer.charAt(i)); i++;																												//	Получаем первую цифру уровня сигнала.
			if( strBuffer.charAt(i)!=','  ){j*=10; j+= _num(strBuffer.charAt(i));}																			//	Если за первой цифрой не стоит знак ',', значит там вторая цифра числа, учитываем и её.
			return j;																																		//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПРОВЕРКИ НАЛИЧИЯ ПРИНЯТЫХ SMS СООБЩЕНИЙ:																									//	Функция возвращает:	целочисленное значение uint8_t соответствующее количеству принятых SMS.
uint8_t	iarduino_GSM::SMSavailable(void){																													//	Аргументы функции:	отсутствуют.
			if(_SMSsum()==0){return 0;}																														//	Если в выбранной памяти нет SMS (входящих и исходящих) то возвращаем 0.
			runAT(F("AT+CMGD=1,3\r\n"),10000);																												//	Выполняем команду удаления SMS сообщений из памяти, выделяя на это до 10 секунд, второй параметр команды = 3, значит удаляются все прочитанные, отправленные и неотправленные SMS сообщения.
			return _SMSsum();																																//	Так как все прочитанные, отправленные и неотправленные SMS сообщения удалены, то функция _SMSsum вернёт только количество входящих непрочитанных SMS сообщений.
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ОБЪЕМА ПАМЯТИ SMS СООБЩЕНИЙ:																										//	Функция возвращает:	целочисленное значение uint8_t соответствующее максимальному количеству принятых SMS.
uint8_t	iarduino_GSM::SMSmax(void){																															//	Аргументы функции:	отсутствуют.
			uint8_t i, j=0;																																	//	Объявляем временные переменные.
			runAT(F("AT+CPMS?\r\n"));																														//	Выполняем команду получения выбранных областей памяти для хранения SMS сообщений.
			if( strBuffer.indexOf( F("+CPMS:") ) < 0){return 0;}																							//	Возвращаем 0, так как в ответе нет текста "+CPMS:".
			i = strBuffer.indexOf( F("+CPMS:") ); 																											//	Получаем позицию начала текста "+CPMS:" в ответе "ЭХО\r\n+CPMS: "ПАМЯТЬ1",ИСПОЛЬЗОВАНО,ОБЪЁМ, "ПАМЯТЬ2",ИСПОЛЬЗОВАНО,ОБЪЁМ, "ПАМЯТЬ3",ИСПОЛЬЗОВАНО,ОБЪЁМ\r\n\r\nOK\r\n".
			for(uint8_t k=i; k<254; k++){ if(strBuffer.charAt(k)==','){j++;} if(j==8){i=k+1; k=254;} }														//	Получаем позицию параметра ОБЪЁМ, он находится через 8 запятых после текста "+CPMS:".
			j = _num(strBuffer.charAt(i)); i++;																												//	Получаем первую цифру в найденной позиции, это первая цифра доступной памяти.
			if( strBuffer.charAt(i)!='\r'  ){j*=10; j+= _num(strBuffer.charAt(i));}																			//	Если за первой цифрой не стоит знак \r, значит там вторая цифра числа, учитываем и её.
			return j;																																		//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ЧТЕНИЯ ВХОДЯЩЕГО SMS СООБЩЕНИЯ:																												//	Функция возвращает:	флаг успешного чтения SMS сообщения true/false
bool	iarduino_GSM::SMSread(char* txt, char* num, char* tim, uint16_t& lngID, uint8_t& lngSum, uint8_t& lngNum){											//	Аргументы функции:	txt - указатель для возврата текста сообщения, num - указатель для возврата номера отправителя, tim - указатель для возврата даты отправки сообщения, lngID - идентификатор склеенных SMS, lngSum - количество склеенных SMS, lngNum - номер склеенной SMS).
//			Готовим переменные:																																//	
			txt[0]=0; num[0]=0; tim[0]=0; lngID=0; lngSum=1; lngNum=1;																						//	Чистим данные по ссылкам и указателям для ответа.
			uint8_t  i				=	numSMS;																												//	В цикле do while, значение этой переменной является индексом (номером) сообщений.
			bool	 f				=	false;																												//	Флаг указывающий на успешное чтение сообщения из памяти в строку strBuffer.
			uint8_t  PDU_SCA_LEN	=	0;																													//	Первый байт блока SCA, указывает количество оставшихся байт в блоке SCA.
			uint8_t  PDU_SCA_TYPE	=	0;																													//	Второй байт блока SCA, указывает формат адреса сервисного центра службы коротких сообщений.
			uint8_t  PDU_SCA_DATA	=	0;																													//	Позиция первого байта блока SCA, содержащего адрес сервисного центра службы коротких сообщений.
			uint8_t  PDU_TYPE		=	0;																													//	Первый байт блока TPDU, указывает тип PDU, состоит из флагов RP UDHI SRR VPF RD MTI (их назначение совпадает с первым байтом команды AT+CSMP).
			uint8_t  PDU_OA_LEN		=	0;																													//	Первый байт блока OA, указывает количество полезных (информационных) полубайтов в блоке OA.
			uint8_t  PDU_OA_TYPE	=	0;																													//	Второй байт блока OA, указывает формат адреса отправителя сообщения.
			uint8_t  PDU_OA_DATA	=	0;																													//	Позиция третьего байта блока OA, это первый байт данных содержащих адрес отправителя сообщения.
			uint8_t  PDU_PID		=	0;																													//	Первый байт после блока OA, указывает на идентификатор (тип) используемого протокола.
			uint8_t  PDU_DCS		=	0;																													//	Второй байт после блока OA, указывает на схему кодирования данных (кодировка текста сообщения).
			uint8_t  PDU_SCTS_DATA	=	0;																													//	Позиция первого байта блока SCTS, содержащего дату и время отправки сообщения.
			uint8_t  PDU_UD_LEN		=	0;																													//	Первый байт блока UD (следует за блоком SCTS), указывает на количество символов (7-битной кодировки) или количество байт (остальные типы кодировок) в блоке UD.
			uint8_t  PDU_UD_DATA	=	0;																													//	Позиция второго байта блока UD, с данного байта начинается текст SMS или блок заголовка (UDH), зависит от флага UDHI в байте PDUT (PDU_TYPE).
			uint8_t  PDU_UDH_LEN	=	0;																													//	Первый байт блока UDH, указывает количество оставшихся байт в блоке UDH. (блок UDH отсутствует если сброшен флаг UDHI в байте PDU_TYPE).
			uint8_t  PDU_UDH_IEI	=	0;																													//	Второй байт блока UDH является первым байтом блока IEI, указывает на назначение заголовка. Для составных SMS значение IEI равно 0x00 или 0x08. Если IEI равно 0x00, то блок IED1 занимает 1 байт, иначе IED1 занимает 2 байта.
			uint8_t  PDU_UDH_IED_LEN=	0;																													//	Первый байт после блока IEI, указывает на количество байт в блоке IED состояшем из IED1,IED2,IED3. Значение данного байта не учитывается в данной библиотеке.
			uint16_t PDU_UDH_IED1	=	0;																													//	Данные следуют за байтом IEDL (размер зависит от значения PDU_IEI). Для составных SMS значение IED1 определяет идентификатор длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).
			uint8_t  PDU_UDH_IED2	=	1;																													//	Предпоследний байт блока UDH. Для составных SMS его значение определяет количество SMS в составе длинного сообщения.
			uint8_t  PDU_UDH_IED3	=	1;																													//	Последний байт блока UDH. Для составных SMS его значение определяет номер данной SMS в составе длинного сообщения.
//			Пытаемся прочитать одно SMS сообщение из указанной области памяти в строку strBuffer:															//	
			do{	i++; if(i>maxSMS){i=1;}																														//	Входим в цикл do while, пока не будет прочитано SMS сообщение или пока не будут пройдены все доступные ячейки памяти SIM-карты, где хранятся SMS сообщения.
				runAT((String)"AT+CMGR="+i+"\r\n", 10000);																									//	Выполняем команду чтения одного SMS сообщения с индексом i. На выполнение команды выделяем до 10 секунд.
				if(strBuffer.indexOf( F("+CMGR:") ) > -1 ){numSMS=i; f=true;}																				//	Если в строке ответа strBuffer встретился текст "+CMGR:", значит в этой строке находятся требуемые данные, выходим из цикла do while и устанавливаем флаг f.
			}	while( i!=numSMS );																															//	Выходим из цикла при условии что были прочитаны все ячейки памяти с ответом "ERROR" (переменная i увеличивалась в цикле, пока не достигла изначально присвоенного значения) или было корректно прочитано одно SMS сообщение (где переменной i было присвоено значение numSMS).
//			Определяем значения из PDU блоков SMS сообщения находящегося в строке strBuffer:																//	
//			|                                                  PDU                                                   |										//	
//			+------------------+-------------------------------------------------------------------------------------+										//	
//			|                  |                                        TPDU                                         |										//	
//			|                  +-----------------------------------------+-------------------------------------------+										//	
//			|        SCA       |                                         |                    UD                     |										//	
//			|                  |      +---------------+                  |     +--------------------------------+    |										//	
//			|                  |      |      OA       |                  |     |             [UDH]              |    |										//	
//			+------------------+------+---------------+-----+-----+------+-----+--------------------------------+----+										//	
//			| SCAL [SCAT SCAD] | PDUT | OAL [OAT OAD] | PID | DCS | SCTS | UDL | [UDHL IEI IEDL IED1 IED2 IED3] | UD |										//	
//																																							//	
			if(f){																																			//	Если SMS сообщение находится в строке strBuffer, то ...
				i = strBuffer.indexOf( F("+CMGR:")    );																									//	Получаем позицию начала текста "+CMGR:" в ответе "ЭХО\r\n+CMGR: СТАТУС,["НАЗВАНИЕ"],РАЗМЕР\r\nPDU\r\n\r\nOK\r\n".
				i = strBuffer.indexOf( F(","     ),i+1);																									//	Получаем позицию первой запятой следующей за текстом "+CMGR:", перед этой запятой стоит цифра статуса.
				if (strBuffer.charAt ( i-1 ) != '0'   )	{ SMSavailable(); return false;}																	//	Если параметр СТАТУС не равен 0, значит это не входящее непрочитанное SMS сообщение. Обращаемся к функции SMSavailable(), чтоб удалить все сообщения, кроме входящих непрочитанных.
				i = strBuffer.indexOf( F("\r\n"  ),i+1);																									//	Получаем позицию символов "\r\n" следующих за первой запятой, за символами "\r\n" следует блок PDU.
				i+= 2;																																		//	Смещаем курсор на длинну символов "\r\n".
			//	SCAL (Service Center Address Length) - байт указывающий размер адреса сервисного центра коротких сообщений.									//	
				PDU_SCA_LEN		= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  первого байта блока SCA (оно указывает на количество оставшихся байт в блоке SCA).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	SCAT (Service Center Address Type) - байт хранящий тип адреса сервисного центра коротких сообщений.											//	
				PDU_SCA_TYPE	= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  второго байта блока SCA (тип адреса: номер, текст, ... ).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	SCAD (Service Center Address Date) - блок адреса сервисного центра коротких сообщений. С третьего байта начинается сам адрес.				//	
				PDU_SCA_DATA	= i;																														//	Сохраняем позицию  третьего байта блока SCA (для получения адреса в дальнейшем).
				i+= PDU_SCA_LEN*2 - 2;																														//	Смещаем курсор на PDU_SCA_LEN байт после байта PDU_SCA_LEN.
			//	PDUT (Packet Data Unit Type) - байт состоящий из флагов определяющих тип блока PDU.															//	
				PDU_TYPE		= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  байта PDU_TYPE (оно состоит из флагов RP UDHI SRR VPF RD MTI).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	OAL (Originator Address Length) - байт указывающий размер адреса отправителя.																//	
				PDU_OA_LEN		= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  первого байта блока OA (оно указывает на количество полезных полубайтов в блоке OA).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	OAT (Originator Address Type) - байт хранящий тип адреса отправителя.																		//	
				PDU_OA_TYPE		= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  второго байта блока OA (тип адреса: номер, текст, ... ).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	OAD (Originator Address Date) - адрес отправителя. С третьего байта начинается сам адрес, его размер равен PDU_OA_LEN полубайтов.			//	
				PDU_OA_DATA		= i;																														//	Сохраняем позицию  третьего байта блока OA (для получения адреса в дальнейшем).
				i+= (PDU_OA_LEN + (PDU_OA_LEN%2));																											//	Смещаем курсор на значение PDU_OA_LEN увеличенное до ближайшего чётного.
			//	PID (Protocol Identifier) - идентификатор протокола передачи данных, по умолчанию байт равен 00.											//	
				PDU_PID			= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  байта PID (идентификатор протокола передачи данных).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	DCS (Data Coding Scheme) - схема кодирования данных (кодировка текста сообщения).															//	
				PDU_DCS			= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  байта DCS (схема кодирования данных).
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	SCTS (Service Center Time Stam) - отметка о времени получения сообщения в сервис центр коротких сообщений.									//	
				PDU_SCTS_DATA	= i;																														//	Сохраняем позицию  первого байта блока SCTS (для получения даты и времени в дальнейшем).
				i+= 14;																																		//	Смещаем курсор на 14 полубайт (7 байт).
			//	UDL (User Data Length) - размер данных пользователя.																						//	
				PDU_UD_LEN		= _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1));																//	Получаем значение  байта UDL (размер данных пользователя). Для 7-битной кодировки - количество символов, для остальных - количество байт.
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	UD (User Data) - данные пользователя (заголовок и текст SMS сообщения).																		//	
				PDU_UD_DATA		= i;																														//	Позиция первого байта блока UD (данные). Блок UD может начинаться с блока UDH (заголовок), если установлен флаг UDHI в байте PDU_TYPE, а уже за ним будет следовать данные текста SMS.
			//	UDHL (User Data Header Length) - длина заголовока в данных пользователя.																	//	
				PDU_UDH_LEN		= (PDU_TYPE & 0b01000000)? _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1)) : 0;									//	Получаем значение  первого байта блока UDH (оно указывает на количество оставшихся байт в блоке UDH). Блок UDH отсутствует если сброшен флаг UDHI в байте PDU_TYPE, иначе блок UD начинается с блока UDH.
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	IEI (Information Element Identifier) - идентификатор информационного элемента.																//	
				PDU_UDH_IEI		= (PDU_UDH_LEN)? _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1)) : 0;											//	Получаем значение  первого байта блока IEI (блок указывает на назначение заголовка). Для составных SMS блок IEI состоит из 1 байта, а его значение равно 0x00 или 0x08. Если IEI равно 0x00, то блок IED1 занимает 1 байт, иначе IED1 занимает 2 байта.
				i+= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт).
			//	IEDL (Information Element Data Length) - длина данных информационных элементов.																//	
				PDU_UDH_IED_LEN	= (PDU_UDH_LEN)? _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1)) : 0;											//	Получаем значение  первого байта после блока IEI (оно указывает на количество байт в блоке IED состояшем из IED1,IED2,IED3). Значение данного байта не учитывается в данной библиотеке.
				i+= PDU_UDH_LEN*2 - 4;																														//	Смещаем курсор к последнему байту блока UDH.
			//	IED3 (Information Element Data 3) - номер SMS в составе составного длинного сообщения.														//	
				PDU_UDH_IED3	= (PDU_UDH_LEN)? _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1)) : 0;											//	Получаем значение  последнего байта блока UDH. (оно указывает на номер SMS в составе составного длинного сообщения).
				i-= 2;																																		//	Смещаем курсор на 2 полубайта (1 байт) к началу.
			//	IED2 (Information Element Data 2) - количество SMS в составе составного длинного сообщения.													//	
				PDU_UDH_IED2	= (PDU_UDH_LEN)? _num(strBuffer.charAt(i))*16 + _num(strBuffer.charAt(i+1)) : 0;											//	Получаем значение  предпоследнего байта блока UDH. (оно указывает на количество SMS в составе составного длинного сообщения).
				i-= 2; if(PDU_UDH_IEI){i-= 2;}																												//	Смещаем курсор на 2 или 4 полубайта (1 или 2 байта) к началу.
			//	IED1 (Information Element Data 1) - идентификатор длинного сообщения.																		//	
				PDU_UDH_IED1	= (PDU_UDH_IEI)? _num(strBuffer.charAt(i))*4096 + _num(strBuffer.charAt(i+1))*256 + _num(strBuffer.charAt(i+2))*16 + _num(strBuffer.charAt(i+3)) :
				                                 _num(strBuffer.charAt(i))*16   + _num(strBuffer.charAt(i+1));												//	Получаем значение  идентификатора длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).
//			Выполняем дополнительные преобразования значений блоков для удобства дальнейшей работы:															//	
			//	Вычисляем схему кодирования данных (текста SMS сообщения):																					//	
				if((PDU_DCS&0xF0)==0xC0){PDU_DCS=0;}else if((PDU_DCS&0xF0)==0xD0){PDU_DCS=0;}else if((PDU_DCS&0xF0)==0xE0){PDU_DCS=2;}else{PDU_DCS=(PDU_DCS&0x0C)>>2;}	//	PDU_DCS = 0-GSM, 1-8бит, 2-UCS2.
			//	Вычисляем тип адреса отправителя:																											//	
				if((PDU_OA_TYPE-(PDU_OA_TYPE%16))==0xD0){PDU_OA_TYPE=1;}else{PDU_OA_TYPE=0;}																//	PDU_OA_TYPE = 0 - адресом отправителя является номер телефона, 1 - адрес отправителя указан в алфавитноцифровом формате.
			//	Вычисляем длину адреса отправителя:																											//	
				if(PDU_OA_TYPE){PDU_OA_LEN=(PDU_OA_LEN/2)+(PDU_OA_LEN/14);}																					//	PDU_OA_LEN = количество символов для адреса отправителя в алфавитноцифровом формате, количество цифр для адреса указанного в виде номера телефона отправителя.
			//	Вычисляем длину блока UDH вместе с его первым байтом:																						//	
				if(PDU_UDH_LEN>0){PDU_UDH_LEN++;}																											//	PDU_UDH_LEN теперь указывает не количество оставшихся байт в блоке UDH, а размер всего блока UDH (добавили 1 байт занимаемый самим байтом PDU_UDH_LEN).
//			Расшифровка SMS сообщения:																														//	
			//	Получаем адрес отправителя.																													//	
				if(PDU_OA_TYPE)	{_SMSdecodGSM (num, PDU_OA_LEN, PDU_OA_DATA);}																				//	Если адрес отправителя указан в алфавитноцифровом формате, то декодируем адрес отправителя как текст в 7-битной кодировке из строки strBuffer в строку num.
				else			{_SMSdecodAddr(num, PDU_OA_LEN, PDU_OA_DATA);}																				//	Иначе декодируем адрес отправителя как номер из строки strBuffer в строку num.
			//	Получаем дату отправки сообщения (дату получения SMS сервисным центром).																	//	
				_SMSdecodDate(tim, PDU_SCTS_DATA);																											//	В строку tim вернётся текст содержания "ДД.ММ.ГГ ЧЧ.ММ.СС".
			//	Получаем текст сообщения:																													//
				if(PDU_DCS==0){_SMSdecodGSM ( txt, PDU_UD_LEN            , PDU_UD_DATA, PDU_UDH_LEN    );}													//	Получаем текст сообщения принятого в кодировке GSM.
				if(PDU_DCS==1){_SMSdecod8BIT( txt, PDU_UD_LEN-PDU_UDH_LEN, PDU_UD_DATA+(PDU_UDH_LEN*2) );}													//	Получаем текст сообщения принятого в 8-битной кодировке.
				if(PDU_DCS==2){_SMSdecodUCS2( txt, PDU_UD_LEN-PDU_UDH_LEN, PDU_UD_DATA+(PDU_UDH_LEN*2) );}													//	Получаем текст сообщения принятого в кодировке UCS2.
			//	Возвращаем параметры составного сообщения:																									//
				if(PDU_UDH_LEN>1){																															//
					lngID  = PDU_UDH_IED1;																													//	Идентификатор составного сообщения.
					lngSum = PDU_UDH_IED2;																													//	Количество SMS в составном сообщении.
					lngNum = PDU_UDH_IED3;																													//	Номер данной SMS в составном сообщении.
				}																																			//
			}																																				//
			if(SMSavailable()==0){numSMS=maxSMS;}																											//	Функция SMSavailable() удалит только что прочитанное сообщение и вернёт количество оставшихся.
			return f;																																		//	Если сообщений не осталось, то указываем что номер последнего прочитанного SMS сообщения (numSMS) равен общему количеству входящих SMS сообщений (maxSMS), тогда при сл. чтении, первым будет прочтена SMS с идентификатором 1.
}																																							//	
																																							//	
//		ФУНКЦИЯ ОТПРАВКИ SMS СООБЩЕНИЯ:																														//	Функция возвращает:	флаг успешной отправки SMS сообщения true/false
bool	iarduino_GSM::SMSsend(const char* txt, const char* num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum){											//	Аргументы функции:	txt - передаваемый текст, num - номер получателя, lngID - идентификатор склеенных SMS, lngSum - количество склеенных SMS, lngNum - номер данной склеенной SMS.
//			Готовим переменные:																																//	
			uint16_t txtLen			=	_SMStxtLen(txt);																									//	Количество символов (а не байтов) в строке txt.
			uint8_t  PDU_TYPE		=	lngSum>1?0x41:0x01;																									//	Первый байт блока TPDU, указывает тип PDU, состоит из флагов RP UDHI SRR VPF RD MTI (их назначение совпадает с первым байтом команды AT+CSMP). Если сообщение составное (склеенное), то устанавливаем флаг UDHI для передачи заголовка.
			uint8_t  PDU_DA_LEN		=	strlen(num); if(num[0]=='+'){PDU_DA_LEN--;}																			//	Первый байт блока DA, указывает количество полезных (информационных) полубайтов в блоке DA. Так как адрес получателя указывается ввиде номера, то значение данного блока равно количеству цифр в номере.
			uint8_t  PDU_DCS		=	0x00;																												//	Второй байт после блока DA, указывает на схему кодирования данных (кодировка текста сообщения).
			uint8_t  PDU_UD_LEN		=	0x00;																												//	Первый байт блока UD, указывает на количество символов (в 7-битной кодировке GSM) или количество байт (остальные типы кодировок) в блоке UD.
			uint16_t PDU_UDH_IED1	=	lngID;																												//	Данные следуют за байтом IEDL (размер зависит от значения PDU_IEI). Для составных SMS значение IED1 определяет идентификатор длинного сообщения (все SMS в составе длинного сообщения должны иметь одинаковый идентификатор).
			uint8_t  PDU_UDH_IED2	=	lngSum;																												//	Предпоследний байт блока UDH. Для составных SMS его значение определяет количество SMS в составе длинного сообщения.
			uint8_t  PDU_UDH_IED3	=	lngNum;																												//	Последний байт блока UDH. Для составных SMS его значение определяет номер данной SMS в составе длинного сообщения.
			uint16_t PDU_TPDU_LEN	=	0x00;																												//	Блок TPDU включает все блоки PDU кроме блока SCA.
//			Определяем формат кодировки SMS сообщения:																										//	
			for(uint8_t i=0; i<strlen(txt); i++){ if(uint8_t(txt[i])>=0x80){PDU_DCS=0x08;} }																//	Если в одном из байтов отправляемого текста установлен 7 бит, значит сообщение требуется закодировать в формате UCS2
//			Определяем класс SMS сообщения;																													//	
			if(clsSMSsend==GSM_SMS_CLASS_0){PDU_DCS|=0x10;}else																								//	SMS сообщение 0 класса
			if(clsSMSsend==GSM_SMS_CLASS_0){PDU_DCS|=0x11;}else																								//	SMS сообщение 1 класса
			if(clsSMSsend==GSM_SMS_CLASS_0){PDU_DCS|=0x12;}else																								//	SMS сообщение 2 класса
			if(clsSMSsend==GSM_SMS_CLASS_0){PDU_DCS|=0x13;}																									//	SMS сообщение 3 класса
//			Проверяем формат номера (адреса получателя):																									//	
			if(num[0]=='+'){if(num[1]!='7'){return false;}}																									//	Если первые символы не '+7' значит номер указан не в международном формате.
			else           {if(num[0]!='7'){return false;}}																									//	Если первый символ  не  '7' значит номер указан не в международном формате.
//			Проверяем значения составного сообщения:																										//	
			if(lngSum==0)    {return false;}																												//	Количество SMS в составном сообщении должно находиться в диапазоне от 1 до 255.
			if(lngNum==0)    {return false;}																												//	Номер      SMS в составном сообщении должен находиться в диапазоне от 1 до 255.
			if(lngNum>lngSum){return false;}																												//	Номер SMS не должен превышать количество SMS в составном сообщении.
//			Проверяем длину текста сообщения:																												//	
			if(PDU_DCS%16==0){ if(lngSum==1){ if(txtLen>160){txtLen=160;} }else{ if(txtLen>152){txtLen=152;} } }											//	В формате GSM  текст сообщения не должен превышать 160 символов для короткого сообщения или 152 символа  для составного сообщения.
			if(PDU_DCS%16==8){ if(lngSum==1){ if(txtLen>70 ){txtLen= 70;} }else{ if(txtLen> 66){txtLen= 66;} } }											//	В формате UCS2 текст сообщения не должен превышать  70 символов для короткого сообщения или 66  символов для составного сообщения.
//			Определяем размер блока UD: (блок UD может включать блок UDH - заголовок, который так же учитывается)											//	Количество байт отводимое для кодированного текста и заголовка (если он присутствует).
			if(PDU_DCS%16==0){PDU_UD_LEN=txtLen;	if(lngSum>1){PDU_UD_LEN+=8;}}																			//	Получаем размер блока UD в символах, при кодировке текста сообщения в формате GSM  и добавляем размер заголовка (7байт = 8символов) если он есть.
			if(PDU_DCS%16==8){PDU_UD_LEN=txtLen*2;	if(lngSum>1){PDU_UD_LEN+=7;}}																			//	Получаем размер блока UD в байтах,   при кодировке текста сообщения в формате UCS2 и добавляем размер заголовка (7байт) если он есть.
//			Определяем размер блока TPDU:																													//
			if(PDU_DCS%16==0){PDU_TPDU_LEN = 0x0D + PDU_UD_LEN*7/8; if(PDU_UD_LEN*7%8){PDU_TPDU_LEN++;} }													//	Размер блока TPDU = 13 байт занимаемые блоками (PDUT, MR, DAL, DAT, DAD, PID, DCS, UDL) + размер блока UD (рассчитывается из количества символов указанных в блоке UDL).
			if(PDU_DCS%16==8){PDU_TPDU_LEN = 0x0D + PDU_UD_LEN;}																							//	Размер блока TPDU = 13 байт занимаемые блоками (PDUT, MR, DAL, DAT, DAD, PID, DCS, UDL) + размер блока UD (указан в блоке UDL).
//			Отправляем SMS сообщение:																														//
			runAT( ((String)"AT+CMGS="+PDU_TPDU_LEN+"\r\n"), 1000);																							//	Выполняем команду отправки SMS без сохранения в область памяти, отводя на ответ до 1 сек.
//			Проверяем готовность модуля к приёму блока PDU для отправки SMS сообщения:																		//
			if( strBuffer.indexOf( F("\r\n>") ) < 0){ return false; }																						//	Если модуль отказывается принять сообщение, то выходим из функции.
//			Создаём блок PDU в строке strBuffer:																											//	
//			|                                                     PDU                                                     |									//	
//			+------------------+------------------------------------------------------------------------------------------+									//	
//			|                  |                                           TPDU                                           |									//	
//			|                  +----------------------------------------------+-------------------------------------------+									//	
//			|        SCA       |                                              |                    UD                     |									//	
//			|                  |           +---------------+                  |     +--------------------------------+    |									//	
//			|                  |           |      DA       |                  |     |              UDH               |    |									//	
//			+------------------+------+----+---------------+-----+-----+------+-----+--------------------------------+----+									//	
//			| SCAL [SCAT SCAD] | PDUT | MR | DAL [DAT DAD] | PID | DCS | [VP] | UDL | [UDHL IEI IEDL IED1 IED2 IED3] | UD |									//	
//																																							//	
				strBuffer  = "";																															//	Готовим строку strBuffer.
				strBuffer += "00";																			// SCAL	(Service Center Address Length)			//	Байт указывающий размер адреса сервисного центра.		Указываем значение 0x00. Значит блоков SCAT и SCAD не будет. В этом случае модем возьмет адрес сервисного центра не из блока SCA, а с SIM-карты.
				strBuffer += _char(PDU_TYPE/16);			strBuffer += _char(PDU_TYPE%16);				// PDUT	(Packet Data Unit Type)					//	Байт состоящий из флагов определяющих тип блока PDU.	Указываем значение 0x01 или 0x41. RP=0 UDHI=0/1 SRR=0 VPF=00 RD=0 MTI=01. RP=0 - обратный адрес не указан, UDHI=0/1 - наличие блока заголовка, SRR=0 - не запрашивать отчёт о доставке, VPF=00 - нет блока срока жизни сообщения, RD=0 - не игнорировать копии данного сообщения, MTI=01 - данное сообщение является исходящим.
				strBuffer += "00";																			// MR	(Message Reference)						//	Байт													Указываем значение 0x00. 
				strBuffer += _char(PDU_DA_LEN/16);			strBuffer += _char(PDU_DA_LEN%16);				// DAL	(Destination Address Length)			//	Байт указывающий размер адреса получателя.				Указываем значение количество цифр в номере получател (без знака + и т.д.). +7(123)456-78-90 => 11 цифр = 0x0B.
				strBuffer += "91";																			// DAT	(Destination Address Type)				//	Байт хранящий тип адреса получателя.					Указываем значение 0x91. Значит адрес получателя указан в международном формате: +7******... .
				_SMScoderAddr(num);																			// DAD	(Destination Address Date)				//	Блок с данными адреса получателя.						Указываем номер num, кодируя его в конец строки strBuffer.
				strBuffer += "00";																			// PID	(Protocol Identifier)					//	Байт с идентификатором протокола передачи данных.		Указываем значение 0x00. Это значение по умолчанию.
				strBuffer += _char(PDU_DCS/16);				strBuffer += _char(PDU_DCS%16);					// DCS	(Data Coding Scheme)					//	Байт указывающий схему кодирования данных.				Будем использовать значения 00-GSM, 08-UCS2 и 10-GSM, 18-UCS2. Во втором случае сообщение отобразится на дисплее, но не сохраняется на телефоне получателя.
				strBuffer += _char(PDU_UD_LEN/16);			strBuffer += _char(PDU_UD_LEN%16);				// UDL	(User Data Length)						//	Байт указывающий размер данных (размер блока UD).		Для 7-битной кодировки - количество символов, для остальных - количество байт.
			if(lngSum>1){																					// UDH	(User Data Header)						//	Если отправляемое сообщение является стоставным,		то добавляем заголовок (блок UDH)...
				strBuffer += "06";																			// UDHL	(User Data Header Length)				//	Байт указывающий количество оставшихся байт блока UDH.	Указываем значение 0x06. Это значит что далее следуют 6 байт: 1 байт IEI, 1 байт IEDL, 2 байта IED1, 1 байт IED2 и 1 байт IED3.
				strBuffer += "08";																			// IEI	(Information Element Identifier)		//	Байт указывающий на назначение заголовка.				Указываем значение 0x08. Это значит что данное сообщение является составным с 2 байтным блоком IED1 (если указать значение 0x0, то блок IED1 должен занимать 1 байт).
				strBuffer += "04";																			// IEDL	(Information Element Data Length)		//	Байт указывающий количество оставшихся байт блока IED.	Указываем значение 0x04. Это значит что далее следуют 4 байта: 2 байта IED1, 1 байт IED2 и 1 байт IED3.
				strBuffer += _char(PDU_UDH_IED1/4096);		strBuffer += _char(PDU_UDH_IED1%4096/256);		// IED1	(Information Element Data 1)			//	Блок указывающий идентификатор составного сообщения.	Все сообщения в составе составного должны иметь одинаковый идентификатор.
				strBuffer += _char(PDU_UDH_IED1%256/16);	strBuffer += _char(PDU_UDH_IED1%16);			//		(2 байта)								//
				strBuffer += _char( PDU_UDH_IED2/16);		strBuffer += _char(PDU_UDH_IED2%16);			// IED2	(Information Element Data 1)			//	Байт указывающий количество SMS в составе составного сообщения.
				strBuffer += _char( PDU_UDH_IED3/16);		strBuffer += _char(PDU_UDH_IED3%16);			// IED3	(Information Element Data 1)			//	Байт указывающий номер данной SMS в составе составного сообщения.
			}																								// UD	(User Data)								//	Блок данных пользователя (текст сообщения).				Указывается в 16-ричном представлении.
//			Передаём полученные блоки PDU:																													//
			runAT(strBuffer, 500);																															//	Выполняем отправку текста strBuffer выделяя на неё до 500 мс.
			strBuffer="";																																	//	Чистим строку strBuffer.
//			Кодируем и передаём текст сообщения по 24 символа:																								//	Передача текста частями снижает вероятность выхода за пределы "кучи" при передаче длинных сообщений.
			uint16_t txtPartSMS=0;																															//	Количество отправляемых символов за очередной проход цикла.
			uint16_t txtPosNext=0;																															//	Позиция после последнего отправленного символа в строке txt.
			while (txtLen) {																																//
				txtPartSMS=txtLen>24?24:txtLen; txtLen-=txtPartSMS;																							//
				if(PDU_DCS%16==0){txtPosNext=_SMScoderGSM ( txt, txtPosNext, txtPartSMS);}																	//	Записать в конец строки strBuffer закодированный в формат GSM  текст сообщения, текст брать из строки txt начиная с позиции txtPosNext, всего взять txtPartSMS символов (именно символов, а не байтов).
				if(PDU_DCS%16==8){txtPosNext=_SMScoderUCS2( txt, txtPosNext, txtPartSMS);}																	//	Записать в конец строки strBuffer закодированный в формат UCS2 текст сообщения, текст брать из строки txt начиная с позиции txtPosNext, всего взять txtPartSMS символов (именно символов, а не байтов).
				runAT(strBuffer, 500);																														//	Выполняем отправку текста strBuffer выделяя на неё до 500 мс.
				strBuffer="";																																//	Чистим строку strBuffer.
			}																																				//	
//			Передаём байт подтверждения отправки SMS сообщения:																								//	
			runAT("\32",10000);																																//	Отправляем символ подтверждения отправки 26 = 0xA1 = 032 = 0b10100001. На выполнение команды выделяем до 10 секунд.
//			Проверяем факт отправки сообщения:																												//	
			if( strBuffer.indexOf( F("+CMGS:" ) ) <0){return false;}																						//	Если модуль не вернул ответ +CMGS: X, значит SMS сообщение не отправлено, возвращаем false.
			return true;																																	//	
}																																							//	
																																							//	
//		ФУНКЦИЯ АВТООПРЕДЕЛЕНИЯ КОДИРОВКИ СКЕТЧА:																											//	
void	iarduino_GSM::TXTsendCodingDetect(const char* str){																									//	Аргументы функции:	строка состоящая из символа 'п' и символа конца строки.
					if(strlen(str)==2)			{codTXTsend=GSM_TXT_UTF8;		}																			//	Если символ 'п' занимает 2 байта, значит текст скетча в кодировке UTF8.
			else	if(uint8_t(str[0])==0xAF)	{codTXTsend=GSM_TXT_CP866;		}																			//	Если символ 'п' имеет код 175, значит текст скетча в кодировке CP866.
			else	if(uint8_t(str[0])==0xEF)	{codTXTsend=GSM_TXT_WIN1251;	}																			//	Если символ 'п' имеет код 239, значит текст скетча в кодировке WIN1251.
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОЛУЧЕНИЯ СОСТОЯНИЯ ГОЛОСОВЫХ ВЫЗОВОВ:																										//	Функция возвращает:	целочисленное значение uint8_t соответствующее текущему состоянию.
uint8_t	iarduino_GSM::CALLstatus(void){																														//	Аргументы функции:	отсутствуют.
            int i,j,k; 
			do{ runAT(F("AT+CLCC\r\n"),200,false);																											//	Выполняем команду получения списка текущих вызовов, выделяя на получение ответа 200 мс, запрещая досрочный выход. И повторяем указанную команду, пока условие while не станет ложным.
				i=strBuffer.indexOf(F("+CLCC:"));																											//	Получаем позицию первого символа "+CLCC:"     в строке ответа strBuffer. Если её нет, то i = -1.
				j=strBuffer.indexOf(F("ERROR") );																											//	Получаем позицию первого символа "ERROR"      в строке ответа strBuffer. Если её нет, то j = -1.
				k=strBuffer.indexOf(F("\r\nOK\r\n"));																										//	Получаем позицию первого символа "\r\nOK\r\n" в строке ответа strBuffer. Если её нет, то k = -1.
			}while( j>=0 || (i<0 && k<0) );																													//	Если в ответе есть "ERROR" или (нет "+CLCC:" и нет "\r\nOK\r\n"), то повторяем команду.
											if(i<0 && k>=0)	{	return GSM_OK;			}	i++;															//	Если в ответе нет "+CLCC:" и есть "\r\nOK\r\n", значит список вызовов пуст.
			i = strBuffer.indexOf( ',', i);	if(i<0)			{	return GSM_CALL_ERR;	}	i++;															//	Получаем позицию направления вызова (0-исходящий, 1-входящий), это позиция после первой запятой.
			i = strBuffer.indexOf( ',', i);	if(i<0)			{	return GSM_CALL_ERR;	}	i++;															//	Получаем позицию состояния вызова.
			switch( strBuffer.charAt(i) ){																													//	Получаем значение состояния вызова.
				case '0':										return GSM_CALL_ACTIVE;		break;															//	Состояние вызова - активное голосовое соединение.
				case '1':										return GSM_CALL_HELD;		break;															//	Состояние вызова - удерживаемый.
				case '2':										return GSM_CALL_OUT_DIAL;	break;															//	Состояние вызова - исходящий в режиме набора адреса.
				case '3':										return GSM_CALL_OUT_BEEP;	break;															//	Состояние вызова - исходящий в режиме дозвона.
				case '4':										return GSM_CALL_IN_BEEP;	break;															//	Состояние вызова - входящий  в режиме дозвона.
				case '5':										return GSM_CALL_IN_WAIT;	break;															//	Состояние вызова - входящий  в режиме ожидания.
				case '7':										return GSM_CALL_END;		break;															//	Состояние вызова - разъединение.
				default:										return GSM_CALL_ERR;		break;															//	Состояние текущего вызова
			}																																				//	Состояние вызова - неопределено.
}																																							//	
																																							//	
//		ФУНКЦИЯ ПРОВЕРКИ НАЛИЧИЯ ВХОДЯЩЕГО ГОЛОСОВОГО ВЫЗОВА:																								//	Функция возвращает:	флаг наличия входящих вызовов true/false
bool	iarduino_GSM::CALLavailable(char* num){																												//	Аргументы функции:	num - указатель для возврата адреса (номера) вызывающего абонента.
			runAT(F("AT+CLCC\r\n"),200,false);																												//	Выполняем команду получения списка текущих вызовов, выделяя на получение ответа 200 мс, запрещая досрочный выход.
			int i = 0, j, l;																																//	Объявляем временные переменные.
			while(1){																																		//	Проходим по всем строкам списка текущих вызовов ...
				i = strBuffer.indexOf(F("+CLCC:"),	i);	if(i<0){return false;}	 j=i; i++;																	//	Получаем позицию первого символа "+CLCC:" в строке ответа strBuffer.
				j = strBuffer.indexOf( ',',			j);	if(j<0){return false;}	 j++;																		//	Получаем позицию направления вызова (0-исходящий, 1-входящий), это позиция после первой запятой.
				j = strBuffer.indexOf( ',',			j);	if(j<0){return false;}	 j++;																		//	Получаем позицию состояния вызова (0-активный, 1-удерживаемый, 2-исходящий в режиме набора, 3-исходящий в режиме дозвона, 4-входящий в режиме дозвона, 5-входящий ожидающий, 7-сброс).
				if (strBuffer.charAt(j)=='4'){																												//	Если состояние вызова = 4 (входящий в режиме дозвона), то ...
					j = strBuffer.indexOf( '"',		j);	if(j<0){return false;}	 j++;																		//	Получаем позицию первого символа адреса (номера вызова).
					l = strBuffer.indexOf( '"',		j);	if(l<0){return false;}	 l-=j;																		//	Получаем количество символов в адресе (номере вызова).
					for(i=0; i<l; i++){num[i]=strBuffer.charAt(j+i);} num[i]=0;																				//	Сохраняем адрес (номер) вызывающего абонента по указателю num.
					return true;																															//	Возвращаем положительный ответ.
				}																																			//	
			}																																				//	
}																																							//	
																																							//	
//		ФУНКЦИЯ ПОДНЯТИЯ ТРУБКИ (ОТВЕТ НА ВХОДЯЩИЙ ВЫЗОВ):																									//	
void	iarduino_GSM::CALLup(void){																															//	Аргументы функции:	отсутствуют.
			runAT(F("ATA\r\n"));																															//	Выполняем команду ответа на входящий вызов.
}																																							//	
																																							//	
//		ФУНКЦИЯ ОПУСКАНИЯ ТРУБКИ (ЗАВЕРШЕНИЕ ЛЮБОГО ВЫЗОВА):																								//	Функция возвращает:	флаг завершения текущих вызовов true/false
void	iarduino_GSM::CALLend(void){																														//	Аргументы функции:	отсутствуют.
			runAT(F("AT+CHUP\r\n"));																														//	Выполняем команду сброса всех вызовов.
}																																							//	
																																							//	
//		ФУНКЦИЯ ИНИЦИАЛИЗАЦИИ ИСХОДЯЩЕГО ГОЛОСОВОГО ВЫЗОВА (ЗВОНОК НА НОМЕР):																				//	Функция возвращает:	флаг наличия входящих вызовов true/false
bool	iarduino_GSM::CALLdial(const char* num){																											//	Аргументы функции:	num - строка с номером вызываемого абонента
			CALLend();																																		//	Завершаем все текущие вызовы.
																	runAT( F("ATD\""), 0, false);															//	Отправляем первую часть команды, отводя на получение ответа 0 мс.
			if(strlen(num)>10){ if(num[0]!='+'){ if(num[0]!='8'){	runAT( "+",        0, false);	}}}														//	Отправляем символ '+' если [номер не короткий, символа '+' нет, номер не начинается с '8'], отводя на получение ответа 0 мс.
																	runAT( num,        0, false);															//	Отправляем номер телефона, отводя на получение ответа 0 мс.
																	runAT( "\"\r\n",   5000    );															//	Отправляем завершающую часть команды в виде кавычки и символов переноса строки, отводя на получение ответа до 5 сек.
			return (strBuffer.indexOf( F("\r\nOK\r\n") ) > -1 )? true:false;																				//	Если в ответе есть строка "\r\nOK\r\n", то возвращаем true, иначе false.
}																																							//	
																																							//	
//		ФУНКЦИЯ ВЫБОРА УСТРОЙСТВА ВВОДА/ВЫВОДА ЗВУКА:																										//	Функция возвращает:	устройство выбранное для ввода и вывода звука GSM_HEADSET / GSM_SPEAKER / GSM_MICROPHONE
uint8_t	iarduino_GSM::SOUNDdevice(uint8_t device){																											//	Аргументы функции:	device - выбираемое устройство для ввода и вывода звука GSM_HEADSET / GSM_SPEAKER / GSM_MICROPHONE
			int i;																																			//	Объявляем временную переменную.
			switch( device ){																																//	Дальнейшие действия зависят от значения device.
				case GSM_HEADSET:		runAT( F("AT+SNFS=0\r\n") );	break;																				//	Выполняем команду выбора гарнитуры в качестве устройства ввода вывода звука.
				case GSM_SPEAKER:		runAT( F("AT+SNFS=1\r\n") );	break;																				//	Выполняем команду выбора громкой связи в качестве устройства ввода вывода звука.
				case GSM_MICROPHONE:	runAT( F("AT+SNFS=2\r\n") );	break;																				//	Выполняем команду выбора микрофона громкой связи в качестве устройства получения звука.
			}							runAT( F("AT+SNFS?\r\n") );																							//	Выполняем команду запроса устройства выбранного для ввода вывода звука.
			i = strBuffer.indexOf(F("+SNFS:"));	if(i<0){return device;}																						//	Получаем позицию первого символа "+SNFS:" в строке ответа strBuffer.
			i+= 7;																																			//	Получаем позицию символа указывающего на выбранное устройство.
			switch( strBuffer.charAt(i) ){																													//	Получаем значение выбранного устройства.
				case '0':			return GSM_HEADSET;					break;																				//	Выбрана гарнитура.
				case '1':			return GSM_SPEAKER;					break;																				//	Выбрана громкая связь.
				case '2':			return GSM_MICROPHONE;				break;																				//	Выбран микрофон громкой связи.
			}																																				//	
}																																							//	
																																							//	
//		ФУНКЦИЯ УСТАНОВКИ ГРОМКОСТИ ЗВУКА:																													//	Функция возвращает:	значение установленной громкости от 0 до 7, где 0 означает что звук выключен, а 7 - максимальная громкость.
uint8_t	iarduino_GSM::SOUNDvolume(uint8_t volume){																											//	Аргументы функции:	volume - громкость от 0-нет до 7-макс / без аргументов - получить значение громкости.
			int i;																																			//	Объявляем временную переменную.
			if(volume<=7){ runAT( (String) "AT+CLVL=" + volume + "\r\n"); }																					//	Выполняем команду установки громкости.
			               runAT(        F("AT+CLVL?\r\n")             );																					//	Выполняем команду получения громкости.
			i = strBuffer.indexOf(F("+CLVL:"));	if(i<0){return volume;}																						//	Получаем позицию первого символа "+CLVL:" в строке ответа strBuffer.
			i+= 7;																																			//	Получаем позицию символа указывающего на выбранную громкость.
			return uint8_t(strBuffer.charAt(i))-48;																											//	Возвращаем значение установленной громкости.
}																																							//	
																																							//	
//		ФУНКЦИЯ ОТКЛЮЧЕНИЯ МИКРОФОНА:																														//	Функция возвращает:	флаг наличия режима mute (отключённого микрофона).
bool	iarduino_GSM::SOUNDmute(bool f){																													//	Аргументы функции:	f - флаг true-отключить микрофон / false-не отключать микрофон.
			if(f)	{ runAT( F("AT+CMUT=1\r\n") ); }																										//	Выполняем команду отключения микрофона.
			else	{ runAT( F("AT+CMUT=0\r\n") ); }																										//	Выполняем команду включения микрофона.
			return SOUNDmute();																																//	Возвращаем флаг наличия немого режима (mute).
}																																							//	
																																							//	
//		ФУНКЦИЯ ПРОВЕРКИ ОТКЛЮЧЕНИЯ МИКРОФОНА:																												//	Функция возвращает:	флаг наличия режима mute (отключённого микрофона).
bool	iarduino_GSM::SOUNDmute(void){																														//	Аргументы функции:	отсутствуют.
			runAT( F("AT+CMUT?\r\n") );																														//	Выполняем команду проверки наличия режима mute (отключённого микрофона).
			int i = strBuffer.indexOf(F("+CMUT:"));	if(i<0){false;}																							//	Получаем позицию первого символа "+CMUT:" в строке ответа strBuffer.
			i+= 7;																																			//	Получаем позицию символа указывающего на наличие немого режима.
			return (strBuffer.charAt(i)=='1')?true:false;																									//	Возвращаем флаг наличия немого режима (mute).
}																																							//	
																																							//	
//		ФУНКЦИИ ДУБЛЁРЫ:																																	//	
String	iarduino_GSM::runAT			(const char* command, uint32_t timeout, bool early)									{return runAT  (String(command), timeout, early);}
String	iarduino_GSM::runUSSD		(const char* command, uint32_t timeout            )									{return runUSSD(String(command), timeout       );}
bool	iarduino_GSM::pin			(                      String      pin_code)										{                    char _pin_code[13];                                      pin_code.toCharArray(_pin_code, 13); return pin(           _pin_code);}
bool	iarduino_GSM::puk			(String      puk_code, const char* pin_code)										{char _puk_code[13];                     puk_code.toCharArray(_puk_code, 13);                                      return puk(_puk_code,  pin_code);}
bool	iarduino_GSM::puk			(const char* puk_code, String      pin_code)										{                    char _pin_code[13];                                      pin_code.toCharArray(_pin_code, 13); return puk( puk_code, _pin_code);}
bool	iarduino_GSM::puk			(String      puk_code, String      pin_code)										{char _puk_code[13]; char _pin_code[13]; puk_code.toCharArray(_puk_code, 13); pin_code.toCharArray(_pin_code, 13); return puk(_puk_code, _pin_code);}
bool	iarduino_GSM::SMSread		(char* txt)																			{uint16_t _lngID; uint8_t _lngSUM, _lngNUM; char _tim[18], _num[17]; return SMSread(txt,_num,_tim,_lngID,_lngSUM,_lngNUM);}
bool	iarduino_GSM::SMSread		(char* txt, char* num)																{uint16_t _lngID; uint8_t _lngSUM, _lngNUM; char _tim[18];           return SMSread(txt, num,_tim,_lngID,_lngSUM,_lngNUM);}
bool	iarduino_GSM::SMSread		(char* txt, char* num, char* tim)													{uint16_t _lngID; uint8_t _lngSUM, _lngNUM;                          return SMSread(txt, num, tim,_lngID,_lngSUM,_lngNUM);}
bool	iarduino_GSM::SMSsend		(String      txt, const char* num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{char _txt[161];                txt.toCharArray(_txt, 161);                            return SMSsend(_txt,  num, lngID, lngSum, lngNum);}
bool	iarduino_GSM::SMSsend		(const char* txt, String      num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{                char _num[15];                             num.toCharArray(_num, 15); return SMSsend( txt, _num, lngID, lngSum, lngNum);}
bool	iarduino_GSM::SMSsend		(String      txt, String      num, uint16_t lngID, uint8_t lngSum, uint8_t lngNum)	{char _txt[161]; char _num[15]; txt.toCharArray(_txt, 161); num.toCharArray(_num, 15); return SMSsend(_txt, _num, lngID, lngSum, lngNum);}
bool	iarduino_GSM::CALLavailable	(void)																				{char _num[17]; return CALLavailable(_num);}
bool	iarduino_GSM::CALLdial		(String num)																		{char _num[17]; num.toCharArray(_num, 17); return CALLdial(_num);}
