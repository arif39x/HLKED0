<?php

include('config.php');

/***** FUNCTIONS *****/

//CountAllBots()
function CountAllBots()
{
	global $sql;
	
	$sql_query = mysql_query("SELECT * FROM vics");

	$res = mysql_num_rows($sql_query);
	
	if($res > 0)
	{
		return $res;
	}
	else
	{
		return 0;
	}
}

//CountOnlineBots()
function CountOnlineBots()
{
	global $sql;
	global $bots;
	
	$sql_query_2 = mysql_query("SELECT * FROM vics WHERE lastcon > '".(time()-$bots['ontime'])."'");
	$res_2 = mysql_num_rows($sql_query_2);
	
	if($res_2 > 0)
	{
		return $res_2;
	}
	else
	{
		return 0;
	}
}

//CountDeadBots()
function CountDeadBots()
{
	global $sql;
	
	$sql_query_2 = mysql_query("SELECT * FROM vics WHERE lastcon < '".(time()-604800)."'");
	$res_2 = mysql_num_rows($sql_query_2);
	
	if($res_2 > 0)
	{
		return $res_2;
	}
	else
	{
		return 0;
	}
}

//CountOfflineBots()
function CountOfflineBots()
{
	return CountAllBots() - CountOnlineBots();
}

//CountFreeBots()
function CountFreeBots()
{
	global $sql;
	global $bots;
	
	$sql_query_2 = mysql_query("SELECT * FROM vics WHERE lastcon > '".(time()-$bots['ontime'])."' AND taskid = '0'");
	$res_2 = mysql_num_rows($sql_query_2);
	
	if($res_2 > 0)
	{
		return $res_2;
	}
	else
	{
		return 0;
	}
}

//Hex2RBG($hex)
function __HEX2RGB($hex)
{
	$c[0] = hexdec(substr($hex,0,2));
	$c[1] = hexdec(substr($hex,2,2));
	$c[2] = hexdec(substr($hex,4));
	return $c;
}

//BotStatus($contime)
function BotStatus($contime)
{
	global $bots;
	
	if($contime > time()-$bots['ontime'])
	{
		return '<span class="green">Online</span>';
	}
	else
	{
		return '<span class="red">Offline</span>';
	}
}

//esc($string)
function esc($string)
{
	return mysql_real_escape_string($string);
}

//StartsWith($haystack, $needle)
function StartsWith($haystack, $needle) {
	if (substr($haystack, 0, strlen($needle)) == $needle)
	{
		return true;	
	}
	else
	{
		return false;
	}
}

//check_value($check, $value)
function check_value($check, $value)
{
	if($check == $value)
	{
		return 'selected="selected"';
	}
}
?>