Edrin@gmx.net

 	Skydance Version 2.291 Beta Release

	->  nEWSET vERSIONS : goto http://skd.box.sk

			- USE SKYDANCE AND IT`S COMPONENTS ON YOUR OWN RISK -

Read this First!
Introduction:
	This Program is FREEWARE!
	SkyServer is a component of Skydance!
	Skydance is designed to control other Win95/98/NT systems in tcp/ip-nets like Internet and Intranets.
	I suggest you only use it on Systems you own/administrate. ;) 
	Skydance 2.291 Beta Release is a program that will remain "under construction" and i will NEVER be
	responsible for any HARM or harmfull things that happend because SKYDANCE was USED! That means I do NOT 	guarantee for any features and security-thing	and i do not guarantee that this program works proper!!!
	Indeed i guarantee for nothing ;)

	After these hard words...

Do not harm, help people!

Known Bugs:
 	the treeview does not get updated automatically... maybe next time :)
	When updating the server you must reboot the server twice until you can delete the temporary server file

New features in SKD 2.291: Bugs fixed!
	Use Update Button to put new version ono pc ;)

New features in SKD 2.29:
	This is mostly an update! 2.25 has the problem that server is not hidden from tasklist.
	
	Update Button:
		This new feature can be used to easily update an old server.

New features in SKD 2.25:
	ICQ support 
		->Specials-Button: add your UIN and then Set the new settings!
		The Icq-pager will send the ip 20 seconds after dial-in!
	RegEditor
		->RegEdit-Button
		Enables you to Request and set Registry information (f.e. Windows CD-key (no DWORD/Binary yet))

	RemoveServer-function
		->Specials-Button: Press to remove SkyServer Registry Data from server.
	
	Be Aware... you are resetting all! so check other settings, too
	The Icq-pager will send the ip 30 seconds after dial-in!
	there seems to be several providers that do not support this function. i do not know why.

General features:
	KeyLog-function! (still a bit buggy but does most of its job...)
	RemoveServerRegistryData
	Good administration of files on the server.
	server-client communictaion-window that list you all events.
	ICQ-Pager
	RegEditor
	Reboot/Shutdown Server
	Swap Mous-Buttons and some jokes
	Execute Files
	Download/Upload
	MessageBox
	Task-Viewer/Manager
	Access to all HDs even Network-HDs
	Password-Protection
	Visible/Invisible Server


FAQs:	-Some people asked me if there is a master-password in the application... no! there is NO MasterPassword!
	-Some called Skydance a Trojan Horse. Anyway, it depends what you are going to do with it.
		>Usually WE use it to administrate servers on lan-partys!
	-Some asked how to use a vbs (VisualBasicScript). 
	All you need to do is to use the wscript.exe in	your <windir>
	-Some asked... NO, IT IS NOT WRITTEN IN VB, it is C++!

	-AND SOME ASKED: MAY I HAVE THO SOURCE-CODE PLEASE ? Respond: BlaBla BLAAA blabal BLUB!

Howto use the Keylog-function:
>>>>	If you want to use the KeyLog, you must upload the "skd.dll" to the directory where skd is in or to the 
>>>>	<windir> or just to the specified directory. In addition to that you need to set the LogFileName and
>>>>	the Dll-FileName in "Specials" of Skydance-Client...	

Howto open a music/txt/something file:
	For example to run a mp3 do something like this:
	in the run-command-line: c:\program files\windows mediaplayer\mplayer2.exe d:\mp3\MathRules.mp3
	<executable-program> <data-file>

Howto use a vbs on server:
	get and find wscript.exe in server´s windir (usualy c:\windows\wscript.exe)
	then simply run: <windir>\wscript.exe <full vbs-path>

HowTo Install Server:
	All you have to do is to start "SkyServer"
	these things are performed then:(probably only win95/98/Me)
			
		-a copy of SkyServer will be done in <windir>\skd.exe
		
		-Registry-Entrys

		-Standards: Pass=Skydance, Port=4000, Dialog=invisible !
	
	be aware, Skydance will now be run every time you restart windows! Deleting the file will
	stop it, of course(the <windir>\skd.exe )!!!

Do never harm people!

HowTo Use Client:
	just connect by using correct ip+port and pass.
	In Menu:
	"Updated Mode" Updated every time when clicking on a tree;
	"Discovery Mode" will load the dir/files only one time.
	"Stealth Mode" will not open unloaded dirs. (So HD will become more silent)

	ContextMenu: CopyPath can be used to get the path of an exe/com and than you can combine it
			with execute...
	Download/Upload: all files you transfer will be added. That means if file
			allready exists, the data you receive will be APPENDED!!!!!!!!
			mail me and tell me if it works fine?
	Run: As it says

	Specials: use Specials to configure SKD 2.0! set password, port and KeyLog etc.
			Decide if server is visible or not
					Shutdown the server by using "Shutdown Server" ;)
			"name" renames the server-file (<windir>\SKD.exe) to a new and updates 						registry! the server is copied to the specified < dir\name.exe > 
			use a exe NAME!!!! the running server-file will still
			exist! delete it later(next time server is run, usually after a reboot)!
		
	Tasks: it is similar like the usual task-list of win95/98... there are some windows shown you should not 			close. For example Task Manager is your Task-Bar ;)
			You should create a directory c:\skd2\ for downloads, on your system!
			Be Aware, all you are doing is no joke! do not access systems without permission.
	For example to run a mp3 do something like this:
	in the run-command-line: c:\program files\windows mediaplayer\mplayer2.exe d:\mp3\MathRules.mp3

Thx for reading, and now have fun,
	Edrin

	And you have any idea about improving SKD ? write me a Mail

Edrin@gmx.net
