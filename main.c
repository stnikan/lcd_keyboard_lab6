/*
 * Author : stnikan
 */

#include <avr/io.h>

// #define F_CPU 11059200UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define RS 7 // выбор регистра
#define E 6  // строб передачи
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

char what_key()
{
    char c[3][4] = {{'3', '2', '1'},
                  {'6', '5', '4'},
                  {'9', '8', '7'},
                  {'#', '0', '*'}};
    DDRD |= ((1 << 0) | (1 << 1) | (1 << 2)); // 0 1 2 на выход
    DDRA &= 0xF0;
    char my_s;
    my_s = 0;
    for (uint8_t i = 0; i < 3; i++)
    {
        PORTD |= (1 << i);
        for (uint8_t j = 0; i < 4; i++)
        {
            my_s = PINA & (1 << j);
            if (my_s == 1)
            {
                LCD_cmd((1 << 7) | 0);
                LCD_data(c[j][i]);
            }
        }
    }
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
    uint8_t k = 0;
    int8_t n = 0;
    int8_t y = 0;
    int8_t T = 3;
    char str[] = {"   TEST"};
    LCD_cmd((1 << 7) | 0);
    for (i = 0; i < 7; i++)
    {
        LCD_data(str[i]);
    }
    // рисуем первую строку

    while (1)
    {

        what_key();
    }
}