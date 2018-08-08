Testovani
=========

Na testovani je nejlepsi http://qmodbus.sourceforge.net/

V Linuxu je treba ho zkompilovat (bez problemu) a ma dokonce vic funkcni, nez zkompilovana verze pro Windows.

Komunikace pomoci http://www.modbusdriver.com/modpoll.html mi nefungovala, zkousel jsem:

```
/modpoll -b 9600 -d 8 -s 1 -p none -m rtu -a 1 -r 500 -c 1 /dev/ttyUSB1
```

Zapojeni
========

Zapojeni cinskeho RS485 modulu:

![Ukazka](https://raw.githubusercontent.com/tuxmartin/arduino_modbus/master/modbus_usb.jpg)


Precteni vsech registru:

![Ukazka](https://raw.githubusercontent.com/tuxmartin/arduino_modbus/master/precteni_vseho.png)

Rozsviceni LED:

![Ukazka](https://raw.githubusercontent.com/tuxmartin/arduino_modbus/master/rozsviceni_led.png)


