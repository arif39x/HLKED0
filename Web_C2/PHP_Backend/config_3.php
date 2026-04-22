<?php

	// CONFIGURATION ******************************************************************************
  // User Interface BackConnect

   $dbHost     = "localhost";	    // MySQL host
   $dbUser     = "root";			    // MySQL username
   $dbPass     = "";			        // MySQL password
   $dbDatabase = "backconnect";	  // MySQL database name
	
   $username   = "Test";				  // Login Username
   $password   = "";				      // Login Password

   $logspage   = 50;					    // Number of logs per page

	// ********************************************************************************************

  // interface with BackConnect executable, define the applications, define the error codes
  $ApplicationCount = 23;
	$aplications = array("MSN Messenger", "Google Talk", "Trillian/MSN", "Trillian/AIM", "Trillian/Yahoo", "Pidgin/Gaim", "Paltalk Scene", "Steam",
					"No-IP" , "DynDNS", "Firefox", "Internet Explorer", "FileZilla", "FlashFXP", "Key", "Outlook", "IMVU", "IDM", "Chrome", "RapidShare Downloader",
					"SmartFTP", "CoreFTP", "FTP Commander");
	$installation = array("Successful", "Standalone", "Computer Start");
	        
  // this special define is used to hide computer restarts from the Suspects page
  $InstallCodeRestart = 2;

?>