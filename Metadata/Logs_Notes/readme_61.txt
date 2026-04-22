******************************
* Moses - Remote Admin Tool  *
*         By Psyon for MoDeM *
******************************

DISCLAIMER:
This software is intended for legitimate remote administration needs.  It is 
provided as is, without any support from.  MoDeM is not now and will not ever
be responsible for any uses of the software.

 CONFIGURATION
---------------
Use the configuration tool config.exe in the zip file.  It should be pretty self explanatory.
config.exe is a slimmed down version of the Bo2k Config tool.  It works, thats all that matters.

 VERSIONS
----------
1.1.5 BETA  I changed the way that the USERHOST response was being handled.  It was preventing
            the lookup of the host IP properly, so dos consoles were not working right.

1.1.2 BETA  I changed the ResolveHost() function.  Im pretty positive it was responsible for some
            errors I was encountering.

1.1.1 BETA  Just a few small bug fixes.

1.1.0 BETA  I changed the installation process.  The old way was not working on all computer.
            The installer and Moses are actually separate programs now, but they run as one.

1.0.1 BETA  I fixed a problem with the initial setup not running on all computers.  Also 
            fixed a problem with Moses not connecting to IRC when it did run.

1.0.0 BETA  This is the initial release of Moses.  It is not very complete.  It does have
            some useful features in it, like the console.  Check it out.

 COMMANDS
----------
All commands are given by messaging the bot.  If you are familiar with IRC than you know what this means, if
than stop reading and delete moses!  Commands are as follows:

COMMAND - Sends raw IRC commands to the server.  
          USAGE: COMMAND <IRC commands>
          ex: COMMAND PRIVMSG #Moses :Command used!

CONSOLE - Gives you a DOS prompt in a DCC window.  This function is extremely buggy and may
          not work on all computers.  Im looking into fixing it.
          USAGE: CONSOLE

EXECUTE - Executes a specified program or file.
          USAGE: EXECUTE <path>
          ex: EXECUTE c:\windows\notepad.exe

HELP    - Lists all currently available commands.
          USAGE: HELP

MSGBOX  - Shows a message box on the remote machine.
          USAGE: MSGBOX <text>
          ex: MSGBOX Sorry, you are about to be rebooted

QUIT    - Makes the bot quit irc and reconnect.
          USAGE: QUIT [message]
          ex: QUIT quit requested from Admin

REBOOT  - Reboots remote computer.
          USAGE: REBOOT

SEND    - Sends a series of files matching a mask via DCC. It will send one at a time
          USAGE: SEND <mask>
          ex: SEND c:\windows\*.exe

SERVER  - Makes the bot switch IRC servers.
          USAGE: SERVER <server>
          ex: SERVER irc.aohell.org

VERSION - Returns current version of Moses.
          USAGE: VERSION