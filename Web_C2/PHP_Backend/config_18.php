<?php
	$admin_user = "admin";          //Login Name
    $admin_pass = "admin";          //Login Password
    $setminutes = 2;                //Connection Intervall in minutes (you chose in Builder)
	$mysql_server = "localhost";
	$mysql_user = "root";
	$mysql_pw = "";
	$mysql_db = "mpac";
    $panel_ver = "1.0";             // Dont change
    $bot_ver = "0.1.0";             // Dont change
	$link = mysql_connect($mysql_server, $mysql_user, $mysql_pw);
	mysql_select_db($mysql_db, $link);
?>