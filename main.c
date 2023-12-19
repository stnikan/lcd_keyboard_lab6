#include <avr/io.h>
#define F_CPU 11059200UL
#include <util/delay.h>

#define RS 7 // выбор регистра
#define E 6  // строб передачи

#define NO_KEY 12

void LCD_cmd(uint8_t cmd)
{
//[]--------------------------------------------------[]
//| Назначение: запись кодов в регистр команд ЖКИ |
//| Входные параметры: cmd - код команды |
//[]--------------------------------------------------[]
DDRC = 0xFF;                    // все разряды PORTC на выход
DDRD |= ((1 << E) | (1 << RS)); // разряды PORTD на выход
PORTD &= ~(1 << RS);            // выбор регистра команд RS=0
PORTC = cmd;                    // записать команду в порт PORTC
PORTD |= (1 << E);              // \ сформировать на
_delay_us(5);                   // | выводе E строб 1-0
PORTD &= ~(1 << E);             // / передачи команды
_delay_ms(5);                   // задержка для завершения записи
}
void LCD_init(void)
{
//[]--------------------------------------------------[]
//| Назначение: инициализация ЖКИ |
//[]--------------------------------------------------[]
DDRC = 0xFF;                    // все разряды PORTC на выход
DDRD |= ((1 << E) | (1 << RS)); // разряды PORTD на выход
_delay_ms(100);                 // задержка для установления напряжения питания
LCD_cmd(0x30);                  // \ вывод
LCD_cmd(0x30);                  // | трех
LCD_cmd(0x30);                  // / команд 0x30
LCD_cmd(0x38);                  // 8 разр.шина, 2 строки, 5 ? 7 точек
LCD_cmd(0x0E);                  // включить ЖКИ и курсор, без мерцания
LCD_cmd(0x06);                  // инкремент курсора, без сдвига экрана
LCD_cmd(0x01);                  // очистить экран, курсор в начало
}
uint8_t TabCon[] = {0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4,
0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 0x50, 0x43, 0x54, 0xA9,
0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1,
0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB,
0xBC, 0xBD, 0x6F, 0xBE, 0x70, 0x63, 0xBF, 0x79, 0x5C, 0x78, 0xE5, 0xC0,
0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7};

uint8_t Code(uint8_t symb)
{
//[]------------------------------------------------[]
//| Назначение: перекодировка символов кириллицы |
//| Входные параметры: symb – символ ASCII |
//| Функция возвращает код отображения символа |
//[]------------------------------------------------[]
uint8_t a = symb >= 192 ? TabCon[symb - 192] : symb;
return a;
}

void LCD_data(char data)
{
DDRC = 0xFF;                    // все разряды PORTC на выход
DDRD |= ((1 << E) | (1 << RS)); // разряды PORTD на выход
PORTD |= (1 << RS);             // выбор регистра data RS=1
PORTC = data;                   // записать команду в порт PORTC
PORTD |= (1 << E);              // \ сформировать на
_delay_us(5);                   // | выводе E строб 1-0
PORTD &= ~(1 << E);             // / передачи команды
_delay_ms(1);                   // задержка для завершения записи
}

uint8_t what_key()
{
    char c[4][3] = {{3,2,1},{6,5,4},{9,8,7},{10,0,11}};
	DDRA = 0x0F;
	for (uint8_t i = 0; i <= 3; i++){
		PORTA = (1 << i);
		_delay_ms(40);
		for (uint8_t j = 0; j <= 2; j++){
			if(PIND & (1 << j)){
			    _delay_ms(15);
				if(PIND & (1 << j))
				        return(c[i][j]);
			}			
		}
	}
	return NO_KEY;
}



// выводит число со знаком
int8_t my_f(int16_t a)
{
int16_t b;
int16_t del = 1;
int8_t n = 5;
char s;
char c[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
if (a < 0)
{
LCD_data('-');
}
while (a / (del * 10) > 0)
{
del *= 10;
n -= 1;
}
while (del >= 1)
{ // k = цифра моего числа
b = a / del;
s = c[b];
LCD_data(s); // написать символ
a -= b * del;
del /= 10;
}
return n;
}

int main(void)
    {
    /* Replace with your application code */
    LCD_init();
    // LCD_cmd((1<<7) |0);
    // LCD_data(str[i])

    uint8_t i;

    char str[] = {"aaa+bbb=?"};
    LCD_cmd((1 << 7) | 0);
    for (i = 0; i < 9; i++)
        {
        LCD_data(str[i]);   
        }
    // рисуем первую строку
    uint8_t key = NO_KEY;
    LCD_cmd((1<<7) |64);
    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t flag = 0, help;
        while (1)
        {
            do{
                key = what_key();
            } while(key == NO_KEY);

            if (flag == 0){
                if (key != 10 && key != 11 && a/100 == 0){
                a = a*10 + key;
                LCD_data(key+'0');}
                else if(key == 11 && a != 0){
                    flag = 1;
                    LCD_data('+');
                }
                }
            else if (flag == 1){
                if (key != 10 && key != 11 && b/100 == 0){
                b = b*10 + key;
                LCD_data(key+'0');}
                else if(key == 10 && b!=0){
                    flag = 2;
                    LCD_data('=');
                    my_f(a+b);
                    }

                }
            else if (flag == 2){
                if(key == 10){
                    LCD_cmd((1<<7) |64);
                    for (i = 0; i < 20; i++)
                        {
                        LCD_data(' ');   
                        }
                    a = 0;
                    b = 0;
                    flag = 0;
                    LCD_cmd((1<<7) |64);
                }
            }

            
            while(what_key() != NO_KEY)
                continue;
            //what_key();
        }
    }
