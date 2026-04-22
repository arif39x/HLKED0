WIN32 users,
	Within this package, should be a file called
darkbot.exe, this is the main program, and the only
file within this package that needs to be run/launched.
If you want to run darkbot without that "dos window",
run the darkbot-noconsole.exe instead. It will load the
bot into the background.


BEFORE you can run it, you should configure your
darkbot. All the configuration files are within the
darkbot\dat directory.

This is a summary of the files:

deop.ini	This file is a list of commands to be
		run when your bot is deoped. You should
		set it to ask the channel service or another
		bot for ops - if you don't have either, maybe
		have it MSG your channel saying it's deoped.
		If you want it to do nothing, leave this
		file blank.

info2.db	This is the information your darkbot learns
		and replys to when asked questions. It's the
		main database. Format is:
			<topic> <data>
		A topic can be more than one word, but will
		be seperated with +'s instead of spaces.
		If a data starts with a +, the reply will be
		in the form of an action. If the data starts
		with a -, the output can be stacked using PIPES
		and you can use raw data, to do things such
		as NOTICE, PRIVMSG and even KICK.  The symbol
		"^" is the $nickname variable. The ^R (reverse
		control char) is the $chan variable. While
		this may seem a bit confusing... don't worry
		about it, It's very simple, just hard to
		explain in words without SHOWING you the bot
		in action.  Go to:
		http://mars.superlink.net/jason/darkbot/examples.txt
		to see some examples of darkbot in action.

perform.ini	This is a list (in the deop.ini format - all RAW)
		to send to the server when it connects (same
		as the mIRC perform list). You can make your darkbot
		join other channels, and login to channel services
		with this.

permbans.db	This is a text file, just a .db (database) file
		name extension. Use the !PERMBAN and !DELBAN
		commands from on-line to add/delete.

random.ini	This is called when your bot replys to a question
		which doesn't use any variables- edit this file
		for an explanation.

randomstuff.ini	List of random things to say. You can add more
		online by saying:  <botnick> RANDOMSTUFF <data>
		where <botnick> is the name of your bot, and
		<data> is the information to add.

seen.db		This is a list of people your bot has seen.
		You can access it with !SEEN <nick>, seen's
		over one week old are deleted.

servers.ini	This is a list of servers your bot is to connect
		to. You can list as many as you want, so if it's
		disconnected, it'll just switch servers.

setup.ini	Edit this and it should be self-explanatory.
		This is the main configuration for the bot.

userlist.db	Edit this and add yourself. Once you add yourself
		save the changes, and run darkbot.exe
		You can add more users from online with the
		!adduser command. You can delete with the !deluser
		command.

If you have any questions, get on irc2.att.net and join #irc_help

irc.superchat.org on #superchat can also offer assistance.

-play (jason@superlink.net)

