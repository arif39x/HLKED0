version: version 2.9 - Release #3 Build 1710
e-mail: support@blackmoonftpserver.com
homepage: http://www.blackmoonftpserver.com/
register: http://secure.emetrix.com/order/product.asp?PID=9876557

System Requirements
==================
Windows2000,XP,2003 or later (NT5+)
MDAC 2.7 or higher (http://www.microsoft.com/data/download_270RTM.htm)

Third-Party plugins
-------------------
(c) SAWZip Control created by S.A.W. - Franky Braem (www.braem17.yucom.be)
(C) shazammp3 control - Erv Walter (http://www.ewal.net/shazam.php)

HISTORY
=======

Version 2.9.3 Build 1710
-------------------------
-Added OnQuotaExceeded event to internal scripting
-Fixed MDTM setting the wrong time
-Updated help file
-Updated email notify script to email on upload or quotaexceeded
-Updated welcome script to show total server uptime

Version 2.9.2 Build 1709
-------------------------
-Added import and export of GUI customizations to xml file
-Added LoginRules for Auto OS account types
-Fixed disappearing virtual folders in Auto OS Accounts
-Fixed MDTM filetime reporting bug
-Added MDTM support for setting the file time
-MDTM and MLST commands will return time in UTC format regardless of server settings
-Normal file listings will return time in UTC format unless the use local time option is set
-Bandwidth report now separated into download and upload reports
-Fixed NT accounts import bug when some NT accounts already existed in the database
-Client logging option now turned on by default
-Fixed passdown flags automatically being applied to all subfolders regardless of settings
-Added Mini Monitor application

Version 2.9.1 Build 1708
-------------------------
-Added disconnect option to GUI menu
-Added option to deny login if home folder was invalid
-Admin Login dialog will auto close if the login is successful
-Group accounts now have their own quota, ratio and iprules settings
-Replaced the domains tab with a new iprules tab (check your domains settings after reinstall)
-IP Rules can be enabled or disabled individually
-BlackMoon Helper now saves the changes on a successful connection string test
-Offline icon will appear in the taskbar when the site is closed
-Fixed bug where deleting a group could delete the ratio&quota settings of another group or account
-Fixed copy account not copying upload ratio value
-Fixed bug where changing the admin password failed
-Updated service executable for better compatibility with realtimestats services
-Ban Management will pop up a warning when an attempt is made to use it remotely

[webadmin]
-Added group and group user editing to web admin
-Added server settings configuration settings to web admin
-Fixed web admin statistics not working for SQL server/msde databases
-Added server ban management to webadmin
-Added iprules, scripts configuration to webadmin
-Added configurable settings in webadmin for defaultpath for new special/virtual/mergefolders

[bmftprealtimestats updates]
-BMFTPRealTimeStats will use the correct paths when creating user or account folders
-BMFTPRealTimeStats Server.ConnectionString doesn't require ftp service to be running anymore
-Fixed BMFTPRealTimeStats Server.DeleteAccount corrupting the parent group when used to delete a group account
-Added logfiletransfers,ratioproperties,quotaproperties,iprulesproperties,useiprules,blockunlistedmasks properties to user object
-Added getipbanlist,removeipban methods to server objects for server ban management

Version 2.8.9 Build 1707
------------------------
-Added beta WebAdmin installation in the /external folder
-Fixed DeleteFolder com object bug introduced in build 1706
-Added ChangePassword method to user object
-Depreciated PasswordHash method and added PasswordFormat method for user object
-Added ReloadServerSettings method to Server object
-ClearUserDBCache method in Server Object now updates the server listening ports
-Corrected several mistakes in com object documentation

Version 2.8.8 Build 1706
------------------------
-Server will query the BDC if user info server cannot be reached for AD authentication
-Fixed bug where moving folders caused access denied errors
-Disk quota calculations how include virtual folders
-Fixed group bandwidth settings not being shared for group accounts
-Fixed clear complete menu option not working for transfer lists
-Added completion time column to transfer views
-ListViews now remember column sizes and order

BMFTPRealTimeStats Fixes and Additions
--------------------------------------
-Fixed "connectionretryinterval" "customlistenport" put properties of user/group object
-Fixed "bantype" "baninterval" "idletype" put properties of user/group object
-Added "connectionloginattempts" property to user/group object
-Added "enablecustomlisten" property to user/group object
-Added "siteenableuser" and "sitedisableuser" properties to user/group object
-Added "DeleteFolder" method to user/group object
-Added "ShowQuotaUpdates" property to quota object
-Added "GroupID" property to the group object
-Added "UserID" property to the user object
-Corrected error in "OnLogin" event documentation in client object

version 2.8.7 Build 1705
------------------------
-Fixed dynamic bandwidth not saving all settings for the week and related event log errors
-Fixed transfer logging error if filename had invalid characters in them
-Added retry ban and hammer ban exclusions to the exclusions list
-Added support for XCRC "FileName" StartPos Length
-Connections from lan ips are not subject to pasv ip settings

version 2.8.6 Build 1704
------------------------
-Fixed quota calculations not taking into account REST command before blocking a download

Build 1703 Fixes
----------------
-Auto GUI startup setting is now stored in the Run key rather than the RunOnce key
-Uptime log starttime is updated whenever the log entry is made

version 2.8.5 Build 1703
------------------------
-Added support for MLST,MLSD and OPT MLST options (draft-ietf-ftpext-mlst-16)
-Added option to delete files to recycle bin
-Taskbar icon will reappear when the taskbar is restored after explorer crashes
-Moved some options from the config file into the database
-Added selectable ip addresses for local admin connections
-Added editing of ftp session properties (right click menu)
-Added support for independent server settings for multiple computers sharing the same user database
-replaced case-sensitive usernames and password option with separate case sensitive username or password option
-GUI autostart on login is now an option in the server options
-fixed errors in sfv checker when multiple users entered folder
-fixed webbrowser viewer not working on some xp and 2k systems
-fixed instances where server settings failed to be updated after being sent to the ftp service
-server uptime is logged every six hours (and on exit)

version 2.8.4 Build 1702
------------------------
-Fixed broken disk quota enforcement
-Fixed minor bugs in ratio calculations
-Added ratio and quota grids in the client data view if enabled
-Added popup to add session credits if ratio was enabled for the client
-Added checks to prevent the repretition of io completion port errors
-Added tags to data transfer failure replies for better debugging
-Percentage progress display now properly displays for resumed file transfers
-Added configurable data transfer connect timeout values instead of the 300 second hardcoded default

version 2.8.3 Build 1701
------------------------
-Fixed GUI disconnecting service
-Fixed hammering ban not working
-Tweaks for fast lan transfers removed by default
-Added temporary ban on consecutive login failures option (see config.xml file)
-Server will send status messages when it temp bans an ip address for hammering or failing logins
-Added option to show file ownership. Only works on file systems that support file streams (ntfs etc)
-Server now loads alternative taskbar icons on startup if set
-Entering blank password will popup a message box and an automatically random password will be generated in place
-Fixed group accounts password being stored as cleartext regardless of password format settings
-Fixed site kick option remaining checked in the user editor even when it was unchecked
-Uninstaller will now check if GUI is running before proceeding
-Fixed crash in GUI if started while previous instance was in the middle of exiting
-Added "Kick After Download Complete" and "Mass Kick After Download Complete" options to client popup
-Added check to see if quota was enabled before logging quotas
-Fixed server trying to access the database while still impersonating an active-directory/nt user
-After an upload the server will display bytes recorded to file rather than bytes received
-Fixed SSL bug where server wouldn't save last block of data if decrypting that block resulted in a shutdown detection
-Server now correctly implements activeshutdown/shutdown detection for ssl connections
-Added download percentage progress and current and overall transfer speeds to transfer displays
-Added switch to apply max users to either the entire group or individual accounts in a group
-Fixed site who not showing which users where transferring files

version 2.8.2 Build 1700
------------------------
-Fixed Memory leak problems in v2.8.1

version 2.8.1 Build 1699
------------------------
-Fixed daily quota not resetting at the end of the day
-Added configurable file transfer buffer options to the config.xml file
-Fixed duplicate entries for client connections

version 2.7.8 Build 1698
------------------------
-switched from crypto api generated hashes to native calculated hashes
-network password is masked and encrypted (depending on password format settings in server options)


version 2.7.7 Build 1697
-----------------------------------------
-Selection of individual ip addresses to listen on has been expanded from the max of 25 to unlimited
-Fixed bug where you could select an ip address for a user account but the server wouldn't listen on it
-Fixed broken group accounts in build 1696
-Added alternative taskbar icon selection in server config options

version 2.7.6 Build 1696
-----------------------------------------
-Changing the database type now takes immediate effect instead of on a server restart
-Added more quota options (daily,weekly & monthly file/kb upload/download quotas. Only one can be used at a time)
-Added an option to quota information as additional info in ciient interactions
-Redesigned the way internal scripts are threaded and added option to select between script thread per client or one global thread for all clients. Default is script thread per client (it is faster)
-Fixed log folder settings being ignored in previous build
-Default status update set to realtime and options are now properly saved in the database
-Fixed logs not being rotated at the end of the day in previous build
-Switch from application managed to OS managed thread pools in this version
-Added ADO connection caching for lower database chatter and lower memory usage over time
-Server logs will now show if and why a transfer failed instead of the previous "finished downloading" message
-Status information will be sent to the gui if a timeout occurs while waiting for scripts to be executed
-Added Server controlling methods to the bmftprealtimestats.server object. See RealtimeStats docs under external scripting (CloseServer,OpenServer,CloseSite,OpenSite,KickID,KickIP,KickUser,ClearUsersDBCache)
-Removed mirc scripts from the server. DDE just doesn't work for service applications
-Removed mirc announcing from sfv script
-Removed extra line feed from file not found error message when a file deletion was attempted. This was causing dreamweaver to panic and disconnect when any attempt was made to delete a file
-Added transfer logging option to individual accounts. Default global file transfer logging is now set to false. Transfer logging is required for top10 stats but it can slow down your server as they increase.
-Added option that makes detecting ip address changes options (server options->interface page)
-Fixed not being able to see transfer progress in client view if transfer started before opening the view
-To improve performance, download/upload quotas are calculated only once and cached in the server instead of calculating each time
-Fixed date format bug for international language versions of windows. Date format for sql server or msaccess can also be modified in the config.xml file

version 2.7.5 Build 1695
-----------------------------------------
-Corrected reintroduction of duplicate log entries problem in build 1694
-Added session id values to file and gui logs for better session identifications
-Fixed bug where a client that disconnected while waiting to login would appear as phantom user in the gui and take up a slot
-Fixed bug in realtimestats where the password the Create method of the User object always returned false due to errors in setting the password format
-Fixed bug in user editor where setting the password would fail on some systems
-Fixed several bugs in the bmftprealtimestats user/group com objects
-Frequency settings can now be changed from the server options->config tab
-Added and documented user object "UseParentFolder", "UseAccountFolder" and "MergeAccountParentFolder" properties
-Added and documented folder object "GetMergeFolders", "AddMergeFolder" and "DeleteMergeFolder" methods
-updated "addgroupaccount.asp" file and added "addaccount.asp" and "showaccountfolders.asp"

version 2.7.4 Build 1694
-----------------------------------------
-Added Hostname/IP exclusion lists for bypassing FXP blocks (server and client) and bandwidth caps
-Added Hostname caching for faster ip to name resolutions
-Fixed realtime statistics bug in active download counting
-Removed auto-restart-on-timeout mechanism in GUI because it was restarting the service for no reason in busy periods
-Switched IPC communications to their own threadpool since they made the gui and service freeze during busy states
-Fixed negative active download count in some instances where file transfers failed to connect
-Added options to reduce "data spamming" between ftp service and the gui (see config.xml file)
-Fixed memory leak in GUI text logging views
-GUI text logs now limits to 2000 lines max and transfer logs limit to 200 max
-File logging now displays reason why user disconnected
-Fixed bug where a hammering user could evade the logins per ip limit
-Realtime stats will now only store a maximum of the latest 200 lines for the server and client activity log

version 2.7.3 Build 1693
-----------------------------------------
-Serialized transfer logging to the database (Makes enabling transfer logging safe but slower)
-Fixed "Row cannot be located for updating" errors that were logged in the application event logs
-Fixed mismatched actions and events in blackmoon helper 
-Fixed statistics not being displayed at all in build 1692
-Updated "Install & Uninstall Service Dependency" in blackmoon helper so any/multiple dependencies can be added/removed
-For local connections statistics are now retrieved directly from the database instead of using disconnected recordsets

version 2.7.2 Build 1692
-----------------------------------------
-Fixed free files not working properly with ratios and quotas
-Added "LoginName", "LoginPassword", "RemoteHostName" properties to Client Object
-Added "IsSQLServerDB" property to Server Object
-Modified default eggdrop config settings so irc reporting is set to off
-Fixed unhandled out of memory exceptions in build 1691
-Server now releases used memory faster and more efficiently
-Client text log messages kept in server and sent to gui is now restricted to 64k instead of unlimited to save memory
-Server now defaults to displaying generic login failure message whether name or password is incorrect
-Server is compiled with full optimizations on for this build rather than no optimization in all the previous builds
-Fixed SFV checking script incorrectly parsing sfv files with extra whitespace in them

version 2.7.1 Build 1691
-----------------------------------------
-max users in group settings only applies to individual accounts not the group
-Added support for EPRT and EPSV command (rfc2428)
-Fixed incomplete response sent back to the ftp client when a syntax error occurred for a built-in site command
-Corrected site kick documentation
-Fixed site kick_xxx commands so they work now
-Fixed site adduser and site deluser internal site commands
-Fixed memory leak in client event handling section
-Fixed instances where an incomplete connection negotiated resulted in objects not being freed
-Fixed script config xml file not being displayed
-Added buttons to edit script source and config files from the gui. Local connections only
-Recoded the entire ratio and quota system
-Added an option in BlackMoon Helper to install MSSQLSERVER as a service dependency so it is started before the FTP Service when an SQL Server database was being used
-Server site state is saved whenever changes are made now
-Updated realtimestats control so it creates group accounts properly
-Documented the folder,ratio & quota objects in external scripting
-Fixed server and license manager retreving blank domain names in some instances when the server was part of a domain
-Fixed bug in server where service would crash when gui exited
-Fixed event logs and server logs being populated with various repetitive error logs

version 2.6.5 Build 1690
-----------------------------------------
-Changed server time activation mechanism
-Added version checking to the GUI
-Added column sorting to upload/download/client listviews
-Added support for SSL FXP
-Added option to remember server site state on startup/shutdown
-Fixed intermittent crash dumb when a user logged out of the server
-Redesigned the thread subsystem that handles internal scripting for more stability

version 2.6.4 Build 1689 - Beta
-----------------------------------------
-User IP Bans now uses true regular expression matching code for matching ips
-Crash dump is prefixed with the edition from which the dump was generated from
-Added day/hour login time settings for user and group accounts (user editor, advanced tab)
-Added day/hour dynamic bandwidth control settings for server bandwidth caps (server options, options tab)
-Fixed incorrect speed calculations when bandwidth caps was enabled for SSL uploads
-Fixed changes in server caps not being applied to current transfers if the transfer started when caps weren't enabled (not changes in user caps while transfer is taking place still won't apply until the user reconnects)

version 2.6.3 Build 1688 - Beta
-----------------------------------------
-Only one instance of the ftp server can be run now. If a previous instance is detected, it will automatically be activated
-Added option to create a self signed certificate for the ftp server
-The SSL certificate for the ftp server is now selected from a list in a combo box instead of typing in the common name
-Added 3 levels of client certificate verification; No verification, Any Trusted Authority or Specific Trusted Authority
-Added view certificate buttons to view selected certificates
-Merge folders can now merge subdirectories under the merge folders. Merge folders are truely merged now
-Fixed bug where file couldn't be renamed or deleted if it was in one of the merge folders
-Added option to save password as plaintext or md5/sha1 hash
-Increased garbage collector interval to allow objects to properly terminate
-Added default folder selection to the server options
-FTP Client Scripts can now retrieve data (ciphers, algorithms, key strengths etc) about an SSL connection
-Added a utility to request and install certificates from 3rd party certificate authorities
-fixed "automatically create group account folders" option in the groups not working
-when creating a new group, you're prompted for a name instead of an automatic random group name generated previously
-external com scripting object now has methods to create groups, users and users in existing group accounts.
-the "shared-group" folder now has an option to do a complete merge with the client folders
-plugged tiny memory in object managing the server timers

version 2.6.2 Build 1687 - Release
-----------------------------------------
-File handles are closed prior to call scripts after a transfer is complete
-Updated error in timer object documentation
-Offline/Online combobox now works
-virtual folders now apply when you specify a filemask for directory listings
-fixed memory leak if realtimestats was not enabled and the realtimestats service wasn't running
-fixed duplicate entries in the server logs when the gui was activated
-added options to specify database type, save transfer and/or user info in the database
-fixed ratios only working in the free edition and not in other edition instead of the other way around
-added a utility called blackmoon helper that can graphically edit the config.xml settings and do misc things like compact databases and clear logs and passwords etc
-removed support for setting default folder settings for sql server databases
-changed the control connection timeout checking code to fix some strange client timeout problems

version 2.6.1 Build 1686 - Release
-----------------------------------------
[summary of additions from the 2.5 beta versions]

[additions]
-Added SQL database support. You can now store everything in an sql server database instead of the default ms access .mdb file
-Added remote user editing for SQL Server databases only
-Added ban management to the main gui. This will let you edit user bans (not server bans)
-Help file has been updated with the latest info

[notices]
-Location of logs can now be changed from the defaults
-Server and user bans now have a variable that determines the number of times a user can connect within the interval before the ban is activated.
-"Server.PrintStatusMessage" can show multiline text in the gui now
-DDE strings are properly terminated. To use DDE in the scripts, enable the "interact with desktop" option in the service properties

[fixes]
-Corrected multiple admin login bug. Server will now tell you if you cannot login because another admin is already logged in. This fix also fixes related bugs such as not seeing the progress of file transfers after logging into a running server
-moved all the realtimestatistics reporting into a separate thread pool for better stability
-ftp server now closes the connection before running client scripts to speed up folder listings on some clients 
-Fixed wrong crc32 calculations for normal and SSL/TLS uploads
-Fixed network connections that required a username and password not working on group folders
-Fixed "Client.AddUser" and "Client.DelUser" not working when used with an SQL server database
-fixed bug where server would append to a file even when the REST command specified otherwise
-fixed instances where pasv ports were not closed if the transfer failed before a connection could be established
-added exception handlers on setting default values for columns in db table. This should correct the incomplete table problems on some win2k systems
-Corrected some instances where 0.0.0.0 addresses were sent to the gui

[final beta fixes]
-fixed error dialog popping up when folders with ' in them were manipulated
-fixed being unable to add multiple entries in the domain list
=fixed wrong domain name retrieval in license manager and ftp server
=added login name/password in reason why a login attempt failed

version 2.5.5 Build 1685 - Beta
----------------------------------
-Added ban management to the main gui. This will let you edit user bans (not server bans)
-Server and user bans now have a variable that determines the number of times a user can connect within the interval before the ban is activated.
-fixed bug where server would append to a file even when the REST command specified otherwise
-fixed not being able to set the database connection string to a blank string after initial change without restarting the server
-Added "Mass Kick" and "Mass Kick Ban" option to right click menu in the client tree
-Added "ScriptSocket" object to ftp scripts. You can use that to connect to eggdrops etc
-Fixed "Client.AddUser" and "Client.DelUser" not working when used with an SQL server database
-"Server.PrintStatusMessage" can show multiline text in the gui now
-Updated sfvchecking script (announcing in mirc, eggdrop, some other stuff)
-Updated welcome script to display only sections you want displayed
-Added an mirc login script for people who are present only in a certain irc channel
-Corrected some errors in the scripting API documentation
-Updated SQL queries used to build graph views for SQL server databases

version 2.5.4 Build 1684 - BETA
-------------------------------
-recognizes the MSDASQL provider if you use an ODBC DSN for your sql connection string
-updated help file
-fixed broken pasv in previous build
-fixed server not resolving hostnames in the previous build
-fixed instances where pasv ports were not closed if the transfer failed before a connection could be established
-added exception handlers on setting default values for columns in db table. This should correct the incomplete table problems on some win2k systems

version 2.5.3 Build 1683 - BETA
-------------------------------
-plugged memory leak in build 1682 when realtimestats option was enabled
-DDE strings are properly terminated. To use DDE in the scripts, enable the "interact with desktop" option in the service properties
-another sfv script update

version 2.5.2 Build 1682 - BETA
-------------------------------
-fixed intermittent crashes in license checking
-Corrected some instances where 0.0.0.0 addresses were sent to the gui
-Added user database manipulation com objects to the realtimestats control. This is for standalone user accounts only, not groups or group accounts
-config.xml file can now be edited in the gui, and remotely too
-gui should shut down the service and exit a lot quicker now

version 2.5.1 Build 1681 - BETA
-------------------------------
-Added MS SQL Server support (mssql server & msde only - no mysql or other)
-database connection string can now be modified in the config.xml file
-Added remote user editing for SQL Server databases. See beta.readme.txt file for more details
-location of server & client logs can be changed from the config.xml
-Rechecked the user editor (copy/join group/detach group operations)
-Removed the prompts for valid numeric input from the server options
-Fixed multiple admin login bug. Server will now tell you if you cannot login because another admin is already logged in. This fix also fixes related bugs such as not seeing the progress
of file transfers after logging into a running server
-moved all the realtimestatistics reporting into a separate thread pool
-fixed ado error dialog showing up if you tried to add an empty script
-added exception wrappers to license integrity checking routines
-reduced the frequency of license integrity checking
-ftp server now closes the connection before running client scripts to speed up folder listings on some clients (experimental)
-fixed wrong crc32 calculations for sfv file checking
-fixed wrong crc32 calculations for SSL/TLS uploads
-updated the sfv checking script to allow certain files to be uploaded
-fixed network connections that required a username and password not working on group folders
-fixed admin password hash not being saved causing remote logins to fail if local login hadn't been attempted before

version 2.4.1 Build 1680
------------------------
-The FTP Server will no longer start the bmftprealtimestats service. it is now up to the administrator to autostart the service or manual start it if the administrator wants realtime stats
-The Trial Version is now version sensitive

version 2.4.1 Build 1679
------------------------
-reverted back to shutting down a connection the forceful way (fixes the disconnect bugs)
-server will now return a response for strings with empty required parameters

version 2.4.1 Build 1678
------------------------
-fixed group folder OS authentication settings resetting itself when opened for editing
-server will ignore any NOOP commands from clients like flashfxp that like to do that unnecessary thing
-fixed group copy/join/detach options in the user editor
-fixed user account bans not working
-ftp server now works on windows.net server
-corrected autostart bug where server wouldn't start because of the bmftprealtimestats service

version 2.4.1 Build 1677
------------------------
-changed the code so the realtimestats is started in a separated thread when the server starts

version 2.4.1 Build 1676
------------------------
-server options not being updated when in client views
-account names are now removed from the server tree when the last client under that name disconnects
-fixed group max login limit not working

version 2.4.1 Build 1675
------------------------
-added version numbering to realtimestats so uninstaller always uninstalls it
-added exception wrappers to all realtimestats code in the service to prevent realtimestat com errors from bringing it down
-added top10 files uploads/downloads in the statistics view
-added top10 downloaders/uploaders in the statistics view
-changed Client.username in the bmftprealtimestats control to Client.ProfileName
-modified client view code so it doesn't freeze when loading long transfer listings
-added right click option to remove orphaned clients from the server tree

version 2.4.1 Build 1674
------------------------
-fixed crash if an ftp upload was aborted in the previous build
-added profile names in addition to login names
-fixed duplicate transfer entries in the views if upload/download was aborted then restarted
-added "ClientName" and "ProfileName" properties to the FTP Client scripts
-fixed server ignoring option to disable service on exit
-improved responsiveness of large client logs. it will still freeze a bit if there are too many file transfers tho
-added "StartService" and "StopService" methods and "ServiceStatus" property to the bmftprealtimestats control 

version 2.4.1 Build 1673
------------------------
-fixed realtimestatistics com control problems
-fixed service hanging sometimes starting when it was set to automatically start
-almost all the gui elements can be modified, except for the graphs view
-rewrote the file transfer code for better performance on lans

version 2.4.1 Build 1672
------------------------
-added a new account type called AutoOS accounts. It can use the NT user manager or active directory to authenticate users automatically without importing them into the ftp server database, while at the same time allowing you to set extended options for these accounts.
-autoos authentication accounts can use the user's home folder or create a homefolder under a root directory of your choice.
-os authentication now supports client impersonation allowing the service to run in the security context of a client account
-fixed bug where editing the server options while a user account view was active failed to update the options in the service
-you can now cancel the import nt user accounts dialog
-added option to make the server binds to server port -1 before initiating an active connection as specified in rfc959
-added option to bind to ip of control connection before data transfer
-fixed not being able to set folder permissions if the gui process the user editor run under didn't have permission to access that folder
-server can now detect PORT commands that ask the server to connect to private ip address and readjust the ip to the control connection ip (the ip which connected to the server)
-replaced the previous blackmoon activex control with bmrealtimestats activex control that runs as a service. you can retrieve server and client data, like logs, ip addresses etc. it can also be enabled disabed from the server gui
-added application event log reporting support

version 2.3.1 Build 1671
-------------------------
-Added a connection message
-Fixed "use case sensitive" option working the opposite way
-Added support for the python scripting language
-increased data timeout to 300seconds
-user download bandwidth cap now correctly divides among downloads

version 2.3.1 Build 1670
------------------------
-server properties button will be disabled until the recordsets are received
-fixed crashes when invalid commands were entered
-service starts errors now correctly reported
-client logs now show which scripts have been loaded for the client
-added a jscript site changepassword sample script

version 2.3.1 Build 1669
------------------------
-Associating groups and accounts with scripts is now done through the gui
-Added graphs to display server statistics
-Added domain allow/reject list to main server
-Added option to deny PORT or PASV command
-Added option to display local filetime listing instead of the default UTC listing
-Added option to automatically create folders under the main group folder for group accounts
-Added "kick_ip" "kick_id" "kick_user" to site commands
-Added buffer security check handler to detect and log buffer overruns
-Added LoginTime property to client scripts
-Added support for JScript and ActivePerl scripting languages
-Virtual and Root folder properties can now be edited
-Site can be closed or opened while the server is online
-Server doesn't go online when changes are made to user or server settings anymore
-Messages explaining why site is closed or account is disabled added
-Groups can now be disabled, disabling all accounts in the group
-Bandwidth caps now correctly share among uploaders and downloaders rather than all transfers
-Mini-crash dump files now have the version in the filename
-Files up to 9 terabytes in size can now be transferred to/from the server
-Data transfer stats will auto adjust to KB/MB/GB for easier readings
-Extended server configuration now uses config.xml in the main ftp server directory
-User logs now displays multiline text properly
-Added option log client activity to disk
-Added server uptime logging and file transfer detail logging into the database
-ADO database exceptions are now logged in the main server log
-Fixed virtual folders listing bug for group account with "use parent settings"
-Fixed garbage characters when cancelling browse for folder dialog box for group accounts
-Fixed bug where folders couldn't be renamed even with all permissions checked
-Fixed minimize bug
-Fixed error when trying to autologin to a remote ip address
-Fixed regular expression matching string for ip addresses and files
-Fixed incorrect login time in userlogs
-Fixed Client_OnLogout event not being fired
-The 15 day shareware limit is now enforced. The program will refuse to start when the time expires

version 2.2.3 Build 1668
------------------------
-Fixed incorrect stripping of characters in UNC pathnames causing file not found errors
-Add NT User Account Dialog can now remeber settings and not prompt again
-Shared Folder Names for GROUPS can now be set for each group
-Fixed prompt to create folders in the gui for network paths

version 2.2.2 Build 1667
------------------------
-Fixed active data transfers not working when service was running without the gui admin logged in
-minimize and maximize commandline parameters now work properly with persistent frames
-The server now waits for all data to be written to disk before announcing an upload completion
-users can now be added to the Windows NT user accounts by right clicking on their names
-optimized SSL and non SSL data transfer code for higher throughput
-fixed user editor errors when using folders with the character ' in them
-server ip hammer ban now lasts for the ban interval instead of the retry interval
-fixed errorcode #2 when starting the ftp server if installation didn't complete correctly
-added Server.DDEPoke and Server.DDERequest to scripting API so scripts can send and recieve data from DDE servers like MIRC
-added OnCreateFolder event to scripting API 
-changed OnChangeFolder and OnCreateFolder event in scripts to be called after a folder was changed or created
-OnChangeFolder and OnCreateFolder events now take a real directory path as their parameters instead of the previous virtual paths
-updated sfv checking script and added a sample zip commment/addfile script
-fixed blackmoon COM control reporting invalid active upload/download values


version 2.2 - Release #1 Build 1666
-----------------------------------
-removed deadlock that occurred if access violation occurred while executing a script callback
-added an activex control that allows you to retrieve realtime stats from the ftp server to display in webpages or external vb scripts
-added sample asp webpages to add users and change passwords of users in the database
-modified the dialy download limit script
-account history can now be reset
-added password button to generate a new random password
-corrected prompts to create a new directory for root drives like d:\
-updated outdated database queries that caused database corruption when copying/etc accounts
-gui now remembers positions of windows and frames

version 2.1 - Release #5 Build 1665
-----------------------------------
-data connection now binds to the ip of the control connection
-added ADO error reporting to most of the elements in the user manager
-fixed bug where using hostname for server ip crashed when hostname couldn't be resolved after five seconds
-fixed crashed that occured when global bandwidth caps were enabled
-added command line params to auto-login and minimize/maximize the server (eg blackmoon.exe [autologin] [minimize|maximize] )
-added option to send or broadcast messages to connected users
-added option to send email to an account or all accounts under a group
-user tree now sorted by name
-added separate group and account icons
-added free files and file type blocking to the user/group accounts
-removed the code the immediately closed the file when a disconnection was detected after a file transfer
-if you get an "Invalid bookmark" error in the user editor it means the database is corrupt. For those who are upgrading, make sure you stop the ftp service and gui before replacing the blackmoon.mdb with the backup file you saved before upgrading.

version 2.1 - Release #4 Build 1664
-----------------------------------
-Fixed maximum user logins per ip limit bug in 1663
-AddUser call in scripts now correctly sets the parent-folder option
-The "Shared-Group" folder string can now be changed in the bmconfig.xml file in the <GROUPMERGE> tag
-Removed logging of invalid user account ids
-Added loopback address to custom listen interfaces
-Added server messages editing in the bmconfig.xml file
-Added two third party script dlls for manipulating zip files and retrieving id3 tags from mp3 files
-Added sample mp3 tag sorting script
-Data connection will bind to a particular interface if you set which interfaces the server should listen on in the server options.

version 2.1 - Release #3 Build 1663
-----------------------------------
-Added SSL/TLS support. explicit connections using AUTH (see quickinfo.txt for setup)
-Added option for group accounts that determines which directories to use
-Fixed time activation bug where the "activation day" was ignored
-Fixed bug where left over transfer connections didn't report the correct transfer rate
-Added separate upload/download bandwidth options for user accounts
-Added option to select which ip the main server will listen on
-IP address changes now correctly detected
-Added new server script property "Server.InstallFolder"
-Added two new scripts. Daily download limit script and file extension filter script.
-Changed CInt to CLng in welcome script
-Added moving file support through renaming

version 2.1 - Release #2 Build 1662
-----------------------------------
-Fixed bug where accounts using parent folders displayed wrong directories
-Added a sample script that logs in users on the fly based on a password (disabed by default)
-Fixed bug where the config scripts couldn't be parsed because file was in use

version 2.1 - Release #2 Build 1661
-----------------------------------
-Added blackmoon.tlb to installation to enable scripting
-Resume permission now checks for REST command along with APPE
-Fixed timout timer not activating or resetting to 5mins
-Fixed speed up in uptime calculation in the server view

version 2.1 - Release #1 Build 1660
----------------------------------------------------------
-Added anti-hammering ban to server options
-Admin IP and password can now be changed from the server options
-Added Error Dialog for ADO exceptions
-Added a resume file transfer permission
-Added support for network shares without mapping network drives
-Added Main server separate upload & down bandwidth caps
-Added ADO 2.7 Merge Module for win2k users
-Added Minidump generation to help debugging on program crash
-Passwords now blanked out in spy view and logs
-Updated stored procedures for safer account manipulation
-Pasv IP/Host option now shows pasv ip regardless of whether the ip is availble on the machine 
-Group settings multiple ip login option now enables/disables appropriately
-Server name can now be entered when importing NT user accounts
-Default folder permissions now applied to group folders too
-Fixed default folder permissions not begin set on some systems
-Fixed multiple ip login bug when user caching was disabled
-Fixed virtual folders not being copied when account was copied
-Fixed several bugs related to disabling the user cache
-Fixed bug where multiple failed logins disabled logging in at all
-Fixed server not recognizing high ascii characters in registered usernames
-Fixed shutdown detection problem with clients like dreamweaver
-Fixed R6025 Virtual Function call errors

version 2.0 - Release #2 Build 1658 (quick fix for 1657)
----------------------------------------------------------
-pasv ip can now be a hostname too. like mystuff.ftpserver.com (you'll need to disable fxp)
-added log files that rotated every day
-fixed missing logs when gui was not running
-fixed server priority resetting back to normal when service was started

version 2.0 - Release #2 Build 1657
------------------------------------
-windows 2000 users who experience problems adding uses should download 
 http://www.microsoft.com/data/download_270RTM.htm for updated ado dlls
-the folder change notifications seemed like the cause of all the problems in the versions
 listed below. No more folder notifications, see how it holds up. so far so good


version 2.0 - Release #2 Build 1656
------------------------------------
-added passive ip address option
-added option not to cache users from database (use if modifying dbase externally)
-added option to set default flags for created folders
-enabled folder editbox so folders can be directly entered
-prompts to create folder if folder does not exist
-fixed some data not begin saved in build 1655
-tentative fix for non-responding server after long use
-fixed internal settings for folder permissions to their proper flags

version 2.0 - Release #2 Build 1654
------------------------------------
-fixed server not responding after long uptimes
-fixed not being able to set subfolder permissions on root folders

version 2.0 - Release #1 Build 1653
--------------------------------
-fixed helpfile display from application

version 2.0 - Beta #1 Build 1652
--------------------------------
-added merge folders so multiple folders can be shown as a single folder
-fixed sub-level virtual folders not working with CDUP and relative dir operations
-included scripting api documentation in the help file
-tentative fix for disappearing control panel tree items

version 2.0 - Beta #1 Build 1651
--------------------------------
-fixed the service not working when installed to another hd

version 2.0 - Beta #1 Build 1650
-------------------------------------------------
-virtual folders can now be specified for sub level folders too
-added folder settings to group accounts
-added option to run script when account was created under a group account
-added vbscript scripting support (sfv processing script etc. (NO SITEBOT-too unstable))
-added domain allow & reject lists (different from hammering ban list)
-server supports pasv connection port ranges
-import nt user accounts & nt user authentication support added
-account expiration added (expired accounts are automatically deleted)
-random passwords are generated by default
-fixed server bandwidth cap not working
-server now supports site commands (adduser, deluser, etc)
-option to use non-case sensitive username & passwords
-dir listings now return the modified file time instead of the creation time
-directory listings are now sorted by name
-fixed bug where joining an account to a group more than once failed
-converted database code from DAO to ADO (only accessdb supported)
-site command access listings added to account properties
-explorer tree is now sorted properly
-using windows installer instead of the old installshield
-detects if ip changes (not tested yet. need feedback)

version 1.5 - Release #2 Build 1550
------------------------------
-fixed bug in parser (Thejian)

version 1.5 - Release #1 Build 1547
------------------------------
-handles "opts" and "feat" commands
-fixed jumbled user logs in status view
-removed default perm ban for hammering users
-properly accounts for timezone adjustments in remote admin uptime calculation & login time calculations
-when using remote admin, status view displays remote server address and timezone
-ftp server displays more informative error messages when errors occur
-you can now copy server logs and user logs from the edit menu into in the clipboard
-tooltips added for ftp user account dialog
-status view lists active interfaces and ports
-"online"/"offline" toolbar button indicates the proper state of the server now

version 1.4 - Beta #1 Build 1472
-----------------------------
-fixed download and upload resume
-fixed disappearing control nodes from the server tree
-fixed logged in user counter resetting itself when account was edited

version 1.3 - Beta #1 Build 1465
-----------------------------
-fixed full path rename bug
-fixed constant file read and dirlist permissions
-allows extended ascii chars
-admin login port has been removed
-corrected passdown flags for checked subfolders not working
-fixed occasional corrupted file on uploads at slow speeds
-added service application priority control code. (starts at normal but can be changed through gui)
-first stab at fixing the 100% cpu usage on exit

version 1.2 - Release #1 Build 1452
------------------------------
-Allows ftp clients that change to a folder then try to delete it (crystal ftp 2.5 etc) to work
 properly when deleting directories

version 1.2 - Release #1 Build 1451
------------------------------
-Fixed the bug where deleting a directory that had not been accessed would disconnect the client
-Status message when unable to delete a directory is more informative

version 1.2 - Release #1 Build 1450
-----------------------------------
-Fixed server not saving settings when system rebooted
-The login to service system has been updated to display more information
-Fixed the crash on exit bug
-Fixed the wrong statistics log entries. This version will clear any previous statistics when installed
-Prompt on exit to stop the ftp service or keep it running
-Added a local explorer in the explorer pane
-When a browse for folder is cancelled it doesn't display garbage anymore
-added NLST command for mget implementations to work
-NLST and LIST now support pathnames
-new commands XMKD,XRMD,XPWD,XCUP,NLST to supports clients and commands like MGET
-zero byte files are handled properly
-no more hanging on invalid folders
-enforces the use of proper filenames and dirs
-add speed limit for accounts (shared or individual) or ftp server (shared or individual)
-gui can detect an unresponsive ftp service and restart it automatically if necessary
-system logging takes place in "blackmoon.log" in the application folder
-fixed the delete directory bug
-fixed timer bug that slowed transfer speeds
-fixed the weird crash on upload bug in the previous version
-fixed the excessive amount of memory usage by the GUI frontend
-fixed the case sensitive directory enforcement for virtual folders in the older versions
-fixed the uninstaller bug that prevented it from uninstalling when installed on a different drive

version 1.1 - Release #1 Build 1131
-----------------------------------
-Update to Version 1.0 release
-Fixed wrong log time in log views
-Fixed bad uptime values (related to incorrect time parsing)
-Fixed the ban system (again) to prevent deadlocks and dead users
-Uninstaller will now prompt you to backup your database file

version 1.0 - Release #1 Build 1115
-----------------------------------
-Server runs as a LOCAL SYSTEM service in this version
-Automatically maps network drives to virtual folders
-Changed to user database to an access db for easier faster management
-Added FTP Server remote administration (no user database manipulation in remote)
-Fixed Disk Quota system
-Fixed Max Server Userlimit bug
-XP Theme enabled
-Fixed DAO multithreading problems
-Added Group accounts and account management functions such as copying, joining and detaching

version 1.0 - Beta #1 Build 313
---------------------------------
-Added access violation exception code handling to prevent crashes  
-Fixed dangling PASV connections and crashing when user disconnected  
-Fixed transmitfile() weirdness
-Fixed bug that let banned users login if there were 2 or more perm bans
-New anti-idle options to help defeat anti-idling clients
-Fixed numerous bugs that caused the previous builds to crash at various times
-Fixed dead locks that made the server stop accepting connections
-Fixed crashes when an interface ip was changed
-Hidden files and directories do not appear in the dir listings
-Login message file option added

Version 1.0 - Beta #1 Build 126
---------------------------------
-Fixed crash on upload
-Fixed corrupt uploaded files bugs
-Now correctly reports transferred file sizes
-Ability to copy client log text to the clipboard like mirc added

Version 1.0 - Beta
------------------
-Initial Release

THINGS YOU SHOULD BE AWARE OF
=============================

-Remote Admin-
If you do not set a password locally you cannot remotely admin the FTP Server. The password
cannot be changed, but you can open the access database and erase the password from the
"ServerSettings" table.

-Uninstaller-
THe uninstaller removes everything except registry entries. Registry entries hold important
information such as your admin password and registration details. During uninstall you will
be prompted to backup your user database file. The backed up file is stored on same drive
as the application was installed in, in the folder "BlackMoon FTP Database Backup".
eg. "C:\BlackMoon FTP Database Backup". If a backup already exists the new backup is given
a different name.

-owner created folders-
Enable owner create access allows users who create directories to be able to have full permissions
on the files in the folders they created. This will allow them to be able to delete aborted uploads
etc. They can only create directories in the folder has create directory permissions on them.
