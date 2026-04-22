---------------------------------------
MailReactor E-mail bulkmailer tool.
---------------------------------------

Open Source Software for MS-Windows and X-Windows

By Niek Albers (nieka@dsv.nl) 1999

Homepage: http://www.dsv.nl/~nieka/MailReactor

Built using: Fast Light Toolkit (http://www.fltk.org)
Standard Function Library (http://www.imatix.com)
TextEdit control (Maarten de Boer)

Released: v0.9 16 November 1999

-----------
What is it?
-----------

MailReactor is a tool to send one e-mail to a number of e-mail addresses.
It can be used to send e-mail to all people who have applied to a notification
service, like program updates, announcements etc. It is of course not meant to
send unsolicited e-mails to random people. It will send a personal e-mail to all
the receivers.

-----------
Legal stuff
-----------

This is Opensource Software, All components that I have used are
opensource too. MailReactor is available under the terms of the
GNU Library General Public License
(http://www.fsf.org/copyleft/lgpl.html).
I would like this software to have pre-compiled binaries for as many
possible systems, so if you made a binary for an exotic OS please send
it to me, so I can add it to my list!

------------------
How does it work?
------------------

Very simple. You can enter all the information about you email:

 - SMTP server (outgoing e-mail server, probably your ISP's mail
   server).
 - SMTP port number, almost always 25.
 - E-mail address of the sender (your e-mail address).
 - Subject of the e-mail
 - Delay, a delay after each e-mail that has been sent in milliseconds.
   Use this to prevent the SMTP server from overloading.
 - Mails/connection. The number of e-mails sent per connection. When
   this number is reached MailReactor closes the connection and opens
   a new one.
 - Text: the text to send.
 - Receivers: All e-mail addresses to send to.

When An error occurs or you want to start sending e-mail from a
certain e-mail address you can use the 'Start from' slider to
select a line number of the receivers to start sending from.

You can abort and resume the process.

-------------------------------------------------------------------------
It's all that simple.....

