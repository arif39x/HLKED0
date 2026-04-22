+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DDD      AA   MM   MM     AA   GGGGG   EEEEE  L        AA   BBB
D  D    A  A  M M M M    A  A  G       E      L       A  A  B  B
D  D   A   A  M  M  M   A   A  G   G   EEE    L      A   A  BBBB
D  D   AAAAA  M     M   AAAAA  G    G  E      L      AAAAA  B   B
DDD    A   A  M     M   A   A   GGGG   EEEEE  LLLLL  A   A  BBBBB
=================================================================

Вы скачали этот файл с http://damagelab.org
Если Вы скачали этот фаил не с damagelab.org, значит кто-то накрысятничал и пользует наш файловый архив. Заходите на http://damagelab.org, там Вам всегда рады.























РУС: Xinch - бесплатная альтернатива Pinch 2.5x, с набором наиболее важных функций
ENG: Xinch - free alternative of Pinch 2.5x, with more imortant features

РУС:
[--==Xinch Official ReadMe по Русски==--]

Фичи:
- ICQ99b-2003a/Lite/ICQ2003Pro 
- Miranda ICQ
- Trillian ICQ&AIM 
- &RQ
- Mozilla
- Opera
- E-Dialer
- Becky
- The Bat!, The Bat! 2 (почта) 
- Outlook/Outlook Express (pop3/imap) 
- IE автозаполнение & защищённые сайты & ftp (поддерживаются 9x/Me/2k/xp) 
- FAR Manager (ftp) 
- Win/Total Commander (ftp) 
- CuteFTP
- WS_FTP
- Интернет (поддерживаются 9x/Me/2k/xp) 
- Системная информация: OS, memory, CPU, hard drives, logged user, host name, IP 
- Завершение процессов (фаерволов и антивирусов)
- Обход фаервола
- Отправка e-mail через SMTP
- Отправка на HTTP (в зависимости от скрипта, несколько возможностей)
- Автозагрузка
- E-mail сообщения криптуются
- Самоудаление
- Консоль (Telnet прилагается)
- Закачка и запуск файла
- Добавление иконки
- Упаковка тремя способами
- Три вида отправки паролей или "не отправка"
- Размер троя после упаковки около 25Kb (это при включении всех модулей!)
- Модульная система, возможность отключать модули для уменьшения размера
- Невидимость антивирусам (наверное не на всегда)
- Xinch будет обновляться
- Ещё несколько возможностей ;)

-------------------------------------------------------------------------------

Директории:

1. DBase - директория для бд Parser'а

2. Scripts - скрипты для отправки по HTTP (читайте там ReadMe)

3. Xinch - основные исходники и компилятор

4. Hide - утилиты для скрытия Xinch от антивирусов (описание ниже)

5. Telnet - для подключения к консоли

6. Reports - папка откуда Parser сканирует отчёт (просканированные файлы удаляются!)

-------------------------------------------------------------------------------
Немного советов от создателя:

Для эффективного "впаривания" Xinch нужно:
1. Ищем в инете классную заставу в формате .scr
2. Достаём из неё иконку с помощью любого редактора ресурсов
3. Ставим в билдере заставку как аттач и иконку от заставки
4. Сжимаем с помощью FSG 1.33
5. Прячем от антивирусов (как это сделать написано чуть ниже)
6. Переименовываем файл .exe в .scr
7. Получаем заставку, в которой очень трудно различить вирус
8. Выкладываем на сайт как очень классную заставку

.exe файлы можно переименовывать в .scr,.bat,.com
Если вы знаете ещё во что можно их переименовать то пишите xepakc@mail.ru!!!

Новые версии Xinch сильно не ждать, но они скорее всего будут
-------------------------------------------------------------------------------

Запустите Builder.exe чтоб скомпилить трой
Запустите Parser.exe чтоб расшифровать пароли
Little_Parser.exe для быстрого преобразования report.bin в читабельный out.txt

[--==Xinch Official FAQ==--]

Вопрос: Как пользоваться утилитами из папки Hide?
Ответ: Если вдруг Xinch начнёт ловится антивирусами, то его придётся от них скрывть,
делается это так:

Внимание!! Проверено только над антивирусом Касперского

1) При компиляции выбираем упаковщик FSG 1.33
2) Обрабатываем файл через CryptXinch.exe:
Выбираем файл, выбираем в списке tElock, нажимаем ProtectEP
3) Обрабатываем файл через ClearXinch.exe
Указываем путь к файлу и жмём кнопку..
Готово.

Утилита Hiew вообщем то и не нужна, так как вместо неё я сделал автоматический
ClearXinch.exe, который всё то что нужно было бы делать вручную в Hiew делает
сам автоматом. ths to me =)

Внимание!! Этот метод не действует, если поставлены модули, который создают Dll,
ловящиеся антивирусом. На время издания релиза Xinch 1.10 Касперский не ловил ни 
единого Dll, ни единого модуля.

На счёт других антивирусов пользуйтесь поиском yandex.ru

-------------------------------------------------------------------------------

Вопрос: Трой не компилится!!! Помогите!!!
Ответ: Отошлите на xepakc@mail.ru файл Xinch\Report.inc, поясните всегда ли трой
не компилится, напишите полную версию Windows, компилируется ли обычный Pinch, 
возможную причину или любые другие нужные пояснения
Если программа выдаёт очень странные надписи с ошибками, то прикрепляйте скриншоты

-------------------------------------------------------------------------------

Вопрос: Как пользоваться консолью?
Ответ: Открываем telnet и вводим open "Ip_адрес_где_запущена_консоль [порт]"

-------------------------------------------------------------------------------

Вопрос: Почему трой так мал?
Ответ: Потому что он не расшифровывает пароли, а только отсылает их хеши

-------------------------------------------------------------------------------

Вопрос: Как обойти фаервол?
Ответ: Используйте HTTP метод отправки паролей

-------------------------------------------------------------------------------
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Трой написан только для ознакомления, автор не несёт никакой ответственности
за использование троя или его частей. Чтобы защититься от троя используйте
антивирусы (например AVP) с новейшими базами и забудьте про Internet Explorer,
используйте лучше замечательную Opera 8
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-------------------------------------------------------------------------------

Что побудило меня написать этот трой?
1. Хотел изучить Masm32
2. Создать что нибудь своё, для пользы общества ;)
3. так что:

Желающим помочь разработке, пишите на admin@x120-club.ru
Требуется либо знание Masm32, либо знание алгоритмов доставания паролей из программ
-------------------------------------------------------------------------------

(c) Divl
http://xinch.x120-club.ru

ENG:
[-==Xinch Official ReadMe in English==-]

Features:
- ICQ99b-2003a/Lite/ICQ2003Pro 
- Miranda ICQ
- Trillian ICQ*AIM 
- *RQ
- Mozilla
- Opera
- E-Dialer
- Becky
- The Bat!, The Bat! 2 (mail) 
- Outlook/Outlook Express (pop3/imap) 
- IE autofilling and the protected sites and ftp (are supported 9x/Me/2k/xp) 
- FAR Manager (ftp) 
- Win/Total Commander (ftp) 
- CuteFTP
- WS_FTP
- The Internet (are supported 9x/Me/2k/xp) 
- The system information: OS, memory, CPU, hard drives, logged user, host name, IP 
- End of processes (antiviruses)
- Firewall bypassing
- Sending e-mail through SMTP or HTTP
- Autoloading
- E-mail messages crypth
- Self-removal(self-distance)
- The console (Telnet it is applied)
- Donwload and start of a file
- Addition of an icon
- Packing by three ways
- Three kinds of sending of passwords or "not sending"
- The size of troy after packing about(near) 15Kb (do not speak that it is impossible:P) 
- Modular system, an opportunity to disconnect modules for reduction of the size
- Invisible to antiviruses (probably not on always)
- Xinch it will be updated
- It is a little more features ;)

-------------------------------------------------------------------------------

Directories:

1. DBase - a directory for Parser's db

2. Scripts - scripts for sending on HTTP (read there ReadMe)

3. Xinch - the basic sources and the compiler

4. Hide - utilities for concealment Xinch from antiviruses (the description is lower)

5. Telnet - for connection to the console

-------------------------------------------------------------------------------

Start Builder.exe to compile troy
Start to decipher passwords Parser.exe

[ - == Xinch Official FAQ ==-]

Question: How to use utilities from folder Hide?
The answer: If suddenly Xinch will start it is caught by antiviruses it will come from them invisible,
It is made so:
1) At compilation packer FSG 1.33 is chosen
2) We process a file through CryptXinch.exe:
We choose a file, we choose in the list tElock, we press ProtectEP
3) We process a file through ClearXinch.exe
We indicate a way to a file and we press the button..
It is ready.

Utility Hiew is not necessary, as instead of it I have made automatic
ClearXinch.exe, which that it would be necessary to do(make) all that manually in Hiew does(makes)
Itself the automatic device. ths to me =)

-------------------------------------------------------------------------------

Question: How to use the console?
The answer: we Open telnet and it is entered open "IP_where running console [23]"

-------------------------------------------------------------------------------

Question: Why troy is so small?
The answer: Because it(he) does not decipher passwords but only sends them hashes

-------------------------------------------------------------------------------

Question: How to bypass firewall?
The answer: Use HTTP a method of sending of passwords

-------------------------------------------------------------------------------
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Troy is written only for acquaintance, the author does not bear(carry) any responsibility
For use troy or its parts. To be protected from troy use
Antiviruses (for example AVP) with the newest bases also forget about Internet Explorer,
Use better remarkable Opera 8
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-------------------------------------------------------------------------------

What has induced me to write this troy?
1. Wanted to study Masm32
2. To make something for society
3. so:

Wishing to help development, write on admin@x120-club.ru
It is required either knowledge Masm32, or knowledge of algorithms gets passwords from programs

http://xinch.x120-club.ru

!!Претензии по поводу перевода обязательно отправляйте представителям корпорации PROMT ;) !!